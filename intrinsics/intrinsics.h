#ifndef __DNML_INTRINSICS_H
#define __DNML_INTRINSICS_H


#include <math.h>
#include <stdint.h>
#include "../system/__compiler.h"
#include "../system/__arch.h"
#include "../sconfigs/numeric_config.h"

#include "arm64/_arm64_conn.h"
#include "x86_64/_x86_conn.h"
#include "risc-v64/_rv64_conn.h"
#include "zvanillc/_vanillc_conn.h"


#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#define max(x, y) ( ((x) > (y)) ? (x) : (y) )

// //* ---------------------------------------------------------------------------- *//
// //*                              IFUNC FUNCTION TABLES                           *//
// //* ---------------------------------------------------------------------------- *//
typedef struct {
    uint8_t     (*clz64)(uint64_t x);
    uint8_t     (*ctz64)(uint64_t x);
    uint64_t    (*bswap64)(uint64_t x);
    uint8_t     (*pcnt64)(uint64_t x);
} _BITOPS_FTABLE;
typedef struct {
    uint64_t (*add64c)(uint64_t a, uint64_t b, uint8_t *carry);
    uint64_t (*sub64b)(uint64_t a, uint64_t b, uint8_t *borrow);
    uint64_t (*wmul128)(uint64_t a, uint64_t b, uint64_t *hi);
    uint64_t (*wdiv128)(
        uint64_t lo, uint64_t hi, uint64_t div, 
        uint64_t *rhat
    )
} _ARITH_FTABLE;
typedef struct {
    uint64_t (*modinv64)(uint64_t x);
} _MARITH_FTABLE;
typedef struct {
} _ALG_FTABLE;

extern _BITOPS_FTABLE _libdnml_gbitops_ftable;
extern _ARITH_FTABLE _libdnml_garith_ftable;
extern _MARITH_FTABLE _libdnml_gmarith_ftable;
extern _ALG_FTABLE _libdnml_galg_ftable;

static inline void _libdnml_fill_gbitops(void) {}
static inline void _libdnml_fill_garith(void) {}
static inline void _libdnml_fill_gmarith(void) {}
static inline void _libdnml_fill_galg(void) {}


//* --------------------------------------------------------------------------------------- *//
//*                                    SINGLE-LIMB ARITHMETIC                               *//
//* --------------------------------------------------------------------------------------- *//
static inline uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint8_t *carry) { 
    #if __compiler_clang // Clang --> Always used
        return __builtin_addcll(a, b, *carry, carry);
    #elif __compiler_gcc // GCC --> Always used
        uint64_t sum;
        *carry = __builtin_uaddll_overflow(a, b, &sum);
        return sum;
    #elif __compiler_msvc // MSVC --> Only on x86_64
        uint64_t sum;
        *carry = _addcarry_u64((*carry) ? 1 : 0, a, b,  &sum)
        return sum;
    #else
        return _libdnml_garith_ftable.add64c(a, b, carry);
    #endif
}
static inline uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint8_t *borrow) {
    #if (__compiler_gcc || __compiler_clang) 
        // Clang / GCC --> Always used
        uint64_t diff;
        *borrow =  __builtin_sub_overflow(a, b, &diff);
        return diff;
    #elif __compiler_msvc // MSVC --> Only on x86_64
        uint64_t diff;
        *borrow = _subborrow_u64((*borrow) ? 1 : 0, a, b, &diff);
        return diff;
    #else
        return _libdnml_garith_ftable.sub64b(a, b, borrow);
    #endif
}
static inline uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi) {
    #if __HAS_int128__ // GCC / Clang --> ALWAYS USED
        uint128 res = ((uint128)a) * ((uint128)b);
        *hi = (uint64_t)(res >> BITS_IN_UINT64_T);
        return (uint64_t)res;
    #elif __compiler_msvc // MSVC - Only on x86/ARM64
        return _umul128(a, b, hi);
    #else
        return _libdnml_garith_ftable.wmul128(a, b, hi);
    #endif
}
static inline uint64_t __DIV_HELPER_UI64__(
    uint64_t lo, uint64_t hi, uint64_t div, 
    uint64_t *rhat
) {
    #if __HAS_int128__ // GCC / Clang
        uint128 dividend = ((uint128)(hi) << BITS_IN_UINT64_T) | lo; 
        *rhat = (uint64_t)(dividend % div);
        return (uint64_t)(dividend / div);
    #elif __compiler_msvc // MSVC
        return _udiv128(hi, lo, div, rhat);
    #else // Unknown Compiler
        return _libdnml_garith_ftable.wdiv128(lo, hi, div, rhat);
    #endif
}

//* --------------------------------------------------------------------------------------- *//
//*                                SINGLE-LIMB MODULAR ARITHMETIC                           *//
//* --------------------------------------------------------------------------------------- *//
static inline uint64_t __MODINV_UI64__(uint64_t x) { return _libdnml_gmarith_ftable.modinv64(x); }
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
        if (exp & 1) res = __MODMUL_UI64__(res, base, mod);
        base = __MODMUL_UI64__(base, base, mod);
        exp >>= 1;
    } return res;
}



//* --------------------------------------------------------------------------------------- *//
//*                                       GENERAL UTILITIES                                 *//
//* --------------------------------------------------------------------------------------- *//
static inline uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp) {
    if (exp == 0) return 1;
    if (exp == 1) return 1;
    if (exp == 2) return (base <= (1ULL << 32) - 1);
    return (double)exp * log2((double)base) < (double)(BITS_IN_UINT64_T);
}
static inline uint8_t __IS_2POW__(uint64_t x) { return (x) && !(x & (x - 1));  }
static inline uint8_t __CLZ_UI64__(uint64_t x) {
    if (!x) return BITS_IN_UINT64_T;
    // The actual code
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_clzll(x);
    #elif __compiler_msvc
        return _lzcnt_u64(x);
    #else
        return _libdnml_gbitops_ftable.clz64(x);
    #endif
}
static inline uint8_t __CTZ_UI64__(uint64_t x) {
    if (!x) return BITS_IN_UINT64_T;
    // The actual code
    #if (__compiler_gcc || __compiler_clang) 
        return __builtin_ctzll(x);
    #elif __compilter_msvc
        return tzcnt_u64(x);
    #else
        return _libdnml_gbitops_ftable.ctz64(x);
    #endif
}
static inline uint64_t __BSWAP_UI64__(uint64_t x) {
    if (!x || x == UINT64_MAX) return x;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_bswap64(x);
    #elif __compiler_msvc
        return _byteswap_uint64(x);
    #else
        return _libdnml_gbitops_ftable.bswap64(x);
    #endif
}
static inline uint64_t __PCNT_UI64__(uint64_t x) { 
    if (!x) return 0; 
    else if (x== UINT64_MAX) return BITS_IN_UINT64_T;
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_popcountll(x);
    #elif __compiler_msvc
        return __popcnt64(x);
    #else
        return _libdnml_fill_gbitops.pcnt64(x);
    #endif
}

#endif