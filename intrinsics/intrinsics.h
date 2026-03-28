#ifndef __DNML_INTRINSICS_H
#define __DNML_INTRINSICS_H


#include <math.h>
#include <stdint.h>
#include "../system/compiler.h"
#include "../system/arch.h"
#include "../sconfigs/numeric_config.h"

#include "arm64/_arm64_conn.h"
#include "x86_64/_x86_conn.h"
#include "zvanillc/_vanillc_conn.h"


#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#define max(x, y) ( ((x) > (y)) ? (x) : (y) )


//* --------------------------- *//
//*    SINGLE-LIMB ARITHMETIC   *//
//* --------------------------- *//
static inline uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint8_t *carry) {
    #if __compiler_clang
        return __builtin_addcll(a, b, *carry, carry);
    #elif __compiler_msvc
        uint64_t sum;
        *carry = _addcarry_u64((*carry) ? 1 : 0, a, b,  &sum)
        return sum;
    #elif __compiler_gcc
        uint64_t sum;
        *carry = __builtin_uaddll_overflow(a, b, &sum);
        return sum;
    #else
        #if __ARCH_X86_64__
            return _x86_add64c(a, b, carry);
        #elif __ARCH_ARM64__
            return _arm64_add64c(a, b, carry);
        #else
            return _cintrin_add64c(a, b, carry);
        #endif
    #endif
}
static inline uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint8_t *borrow) {
    #if __compiler_msvc
        uint64_t diff;
        *borrow = _subborrow_u64((*borrow) ? 1 : 0, a, b, &diff);
        return diff;
    #elif (__compiler_gcc || __compiler_clang)
        uint64_t diff;
        *borrow =  __builtin_sub_overflow(a, b, &diff);
        return diff;
    #else
        #if __ARCH_X86_64__
            return _x86_sub64b(a, b, carry);
        #elif __ARCH_ARM64__
            return _arm64_sub64b(a, b, carry);
        #else
            return _cintrin_sub64b(a, b, carry);
        #endif
    #endif
}
static inline uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi) {
    #if __HAS_int128__ // GCC & Clang
        uint128 res = ((uint128)a) * ((uint128)b);
        *hi = (uint64_t)(res >> BITS_IN_UINT64_T);
        return (uint64_t)res;
    #elif __compiler_msvc // MSVC
        return _umul128(a, b, hi);
    #else // ANY OTHER COMPILERS
        #if __ARCH_X86_64__
            return _x86_wmul128(a, b, carry);
        #elif __ARCH_ARM64__
            return _arm64_wmul128(a, b, carry);
        #else
            return _cintrin_wmul128(a, b, carry);
        #endif
    #endif
}
static inline uint64_t __DIV_HELPER_UI64__(
    uint64_t lo, uint64_t hi, uint64_t div, 
    uint64_t *rhat
) {
    #if __ARCH_X86_64__ // X86_64 - NATIVE WDIV
        return _x86_wdiv128(lo, hi, div, rhat);
    #elif __HAS_int128__ // (ARM64 / RISC-V / unknown_arch) + (GCC / Clang)
        uint128 dividend = ((uint128)(hi) << BITS_IN_UINT64_T) | lo; 
        *rhat = (uint64_t)(dividend % div);
        return (uint64_t)(dividend / div);
    #elif __compiler_msvc // (ARM64 / RISC-V / unknown_arch) + MSVC
        return _udiv128(hi, lo, div, rhat);
    #else // (ARM64 / RISC-V / unknown_arch) + unknown_compiler
        uint8_t divlz = 0;
        #if __ARCH_ARM64__
            divclz = _arm64_clz64(div);
        #elif __ARCH_RISCV_64__
            divclz = _riscv_clz64(div);
        #else
            divclz = _cintrin_clz64(div);
        #endif
        return _cintrin_wdiv128(lo, hi, div, divclz, rhat);
    #endif
}

//* ----------------------------------- *//
//*    SINGLE-LIMB MODULAR ARITHMETIC   *//
//* ----------------------------------- *//
static inline uint64_t __MODINV_UI64__(uint64_t x) {
    #if __ARCH_X86_64__
        return _x86_modinv64(x);
    #elif __ARCH_ARM64__
        return _arm64_modinv64(x);
    #else
        return _cintrin_modinv64(x);
    #endif
}
static inline uint64_t __MODMUL_UI64__(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t hi, lo;
    lo = __MUL_UI64__(a, b, &hi);
    #if __HAS_int128__
        return (uint64_t)(((unsigned __int128)hi << 64 | lo) % mod);
    #else
        if (hi == 0) return lo % mod;
        uint64_t rem = hi % mod;
        for (uinit8_t i = 63; i >= 0; --i) {
            rem = (rem >= mod - rem) ?
                    rem - (mod - rem) :
                    rem + rem;
            if ((lo >> i) & 1) {
                ++rem;
                if (rem >= mod) rem -= mod;
            }
        }
    #endif
}
static inline uint64_t __MODEXP_UI64__(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 1) return 0;
    if (exp == 0) return 1;
    if (exp == 1) return base;
    base %= mod;
    uint64_t res = 1;
    while (exp > 0) {
        if (exp & 1) res = _cintrin_modmul64(res, base, mod);
        base = _cintrin_modmul64(base, base, mod);
        exp >>= 1;
    } return res;
}


//* --------------------------- *//
//*      GENERAL UTILITIES      *//
//* --------------------------- *//
static inline uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp) {
    if (exp == 0) return 1;
    if (exp == 1) return 1;
    if (exp == 2) return (base <= (1ULL << 32) - 1);
    return (double)exp * log2((double)base) < (double)(BITS_IN_UINT64_T);
}
static inline uint8_t __IS_2POW__(uint64_t x) { return (x) && !(x & (x - 1));  }
static inline uint8_t __CLZ_UI64__(uint64_t x) {        // COUNT LEADING ZEROS
    #if x == 0
        return 64;
    #endif
    // The actual code
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_clzll(x);
    #elif __compiler_msvc
        return _lzcnt_u64(x);
    #else
        #if __ARCH_X86_64__
            return _x86_clz64(x);
        #elif __ARCH_ARM64__
            return _arm64_clz64(x);
        #else
            return _cintrin_clz64(x);
        #endif
    #endif
}
static inline uint8_t __CTZ_UI64__(uint64_t x) {        // COUNT TRAILING ZEROS
    #if x == 0
        return 64;
    #endif
    // The actual code
    #if (__compiler_gcc || __compiler_clang) 
        return __builtin_ctzll(x);
    #elif __compilter_msvc
        return tzcnt_u64(x);
    #else
        #if __ARCH_X86_64__
            return _x86_ctz64(x);
        #elif __ARCH_ARM64__
            return _arm64_ctz64(x);
        #else
            return _cintrin_ctz64(x);
        #endif
    #endif
}
static inline uint64_t __BSWAP_UI64__(uint64_t x) {     // BYTESWAP
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_bswap64(x);
    #elif __compiler_msvc
        return _byteswap_uint64(x);
    #else
        #if __ARCH_X86_64__
            return _x86_bswap64(x);
        #elif __ARCH_ARM64__
            return _arm64_bswap64(x);
        #else
            return _cintrin_bswap64(x);
        #endif
    #endif
}

#endif