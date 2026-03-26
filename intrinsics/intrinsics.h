#ifndef __DNML_INTRINSICS_H
#define __DNML_INTRINSICS_H


#include <math.h>
#include <stdint.h>
#include "../system/compiler.h"
#include "../sconfigs/numeric_config.h"


#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#define max(x, y) ( ((x) > (y)) ? (x) : (y) )


//* --------------------------- *//
//*    SINGLE-LIMB ARITHMETIC   *//
//* --------------------------- *//
static inline uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint64_t *carry) {
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
        uint64_t sum, temp_carry_out;
        uint8_t carry_in = (*carry) ? 1 : 0;
        // ---- FUNCTION HERE ---- //
        *carry = temp_carry_out;
        return sum;
    #endif
}
static inline uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint64_t *borrow) {
    #if __compiler_msvc
        uint64_t diff;
        *borrow = _subborrow_u64((*borrow) ? 1 : 0, a, b, &diff);
        return diff;
    #elif (__compiler_gcc || __compiler_clang)
        uint64_t diff;
        *borrow =  __builtin_sub_overflow(a, b, &diff);
        return diff;
    #else
        uint64_t diff;
        uint64_t temp_borrow_out;
        uint8_t borrow_in = (*borrow) ? 1 : 0;
        // ---- FUNCTION HERE ---- //
        *borrow = temp_borrow_out;
        return diff;
    #endif
}
static inline uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi) {
    #if __HAS_int128__
        uint128 res = ((uint128)a) * ((uint128)b);
        *hi = (uint64_t)(res >> BITS_IN_UINT64_T);
        return (uint64_t)res;
    #else
        // Seperate a and b into 2 different halves
        uint64_t a_low = a & 0xFFFFFFFF;        uint64_t b_low = b & 0xFFFFFFFF; // Extract the 32 lower bits
        uint64_t a_high = a >> 32;              uint64_t b_high = b >> 32; // Extract the 32 upper bits

        uint64_t first_mul = a_low * b_low;
        uint64_t second_mul = a_low * b_high;
        uint64_t third_mul = a_high * b_low;
        uint64_t fourth_mul = a_high * b_high;
        
        // Lower Half Calculation
        uint64_t mid = second_mul + third_mul;
        uint64_t mid_carry = (mid < second_mul); // Handles mid overflow (0 <= mid < 2^65)
        uint64_t mid_low = (mid & 0xFFFFFFFF) << 32; // Extract and Isolate the lower 32 bit of mid
        uint64_t res = first_mul + mid_low; // Return the lower 64 bits
        /* Lower half of a bit is attained by the formula:  Res % 2^64 = low
        *   +) (a x b % 2^64) = (first_mul + mid * 2^32 + fourth_mul * 2^64) % 2^64
        *                     = (first_mul + mid * 2^32) % 2^64 (1)
        * 
        *   +) mid                      = mid_low + mid_high * 2^32
        *      mid * 2^32               = mid_low * 2^32 + mid_high * 2^64
        *      (mid * 2^32) % 2^64      = (mid_low * 2^32 + mid_high * 2^64) % 2^64
        *      (mid * 2^32) % 2^64      = mid_low * 2^32                                
        *                               = mid_low << 32 (2)
        * 
        * ----> (a x b % 2^64)      = first_mul + mid_low * 2^32
        * ----> (a x b) lower bits  = first_mul + mid_low * 2^32
        */

        // Upper Half Calculation
        uint64_t carry1 = (res < first_mul); // Check if adding mid_low to first_mul overflows to carry to the higher half
        uint64_t mid_high = mid >> 32; // The upper 32 bits of mid
        /* Higher half of a bit is attained by the formula:  floor(Res / 2^64) = high
        *   +) floor(a x b / 2^64) = floor((first_mul + mid * 2^32 + fourth_mul * 2^64) / 2^64)
        *                          = floor((first_mul / 2^64) + (mid * 2^-32) + fourth_mul)
        * 
        *   +) mid                  = mid_low + mid_high * 2^32
        *      mid * 2^-32          = (mid_low + mid_high * 2^32) / 2^32
        *      mid * 2^-32          = (mid_low / 2^32) + mid_high
        *      floor(mid * 2^-32)   = floor((mid_low / 2^32) + mid_high)
        *      floor(mid * 2^-32)   = floor({0 <= mid_low / 2^32 < 1} + mid_high)  (0 <= mid_low < 2^32)
        *      floor(mid * 2^-32)   = mid_high
        * 
        *   -----> floor(a x b / 2^64) = floor(first_mul / 2^64) + mid_high + fourth_mul
        *                              = mid_high + fourth_mul
        */

        // Takes the carry from lower half + the overflowed mid bit
        *hi = fourth_mul + carry1 + mid_high + (mid_carry << 32);
        return res;
    #endif
}


//* ----------------------------------- *//
//*    SINGLE-LIMB MODULAR ARITHMETIC   *//
//* ----------------------------------- *//
static inline uint64_t __MODINV_UI64__(uint64_t x) {
    uint64_t res = 2 - x;
    for (uint8_t i = 0; i < 5; ++i) res *= 2 - x * res;
    return res;
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
        if (exp & 1) res = __MODMUL_UI64__(res, base, mod);
        base = __MODMUL_UI64__(base, base, mod);
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
        uint8_t n = 0;
        while (!(x & (1ULL < 63))) { x <<= 1; ++n }
        return n;
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
        uint8_t n = 0;
        while (!(x & 1)) { x >>= 1; ++n; }
        return n;
    #endif
}
static inline uint64_t __BSWAP_UI64__(uint64_t x) {     // BYTESWAP
    #if (__compiler_gcc || __compiler_clang)
        return __builtin_bswap64(x);
    #elif __compiler_msvc
        return _byteswap_uint64(x);
    #else
        return ((x << 56) |
            ((x << 40) & 0x00FF000000000000ULL) |
            ((x << 24) & 0x0000FF0000000000ULL) |
            ((x << 8)  & 0x000000FF00000000ULL) |
            ((x >> 8)  & 0x00000000FF000000ULL) |
            ((x >> 24) & 0x0000000000FF0000ULL) |
            ((x >> 40) & 0x000000000000FF00ULL) |
            (x >> 56));
    #endif
}

#endif