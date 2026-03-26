#ifndef ____DNML_VANILLC_MODOP
#define ____DNML_VANILLC_MODOP



#include <stdint.h>
#include "../../system/compiler.h"
#include "_vanillc_arith.h"

// 64 bit Modular Inverse of Modulus 2^64
static inline uint64_t _cintrin_modinv64(uint64_t x) {
    uint64_t res = 2 - x;
    for (uint8_t i = 0; i < 5; ++i) res *= 2 - x * res;
    return res;
}

// 64 bit Modular Multiplication of Modulus n
static inline uint64_t _cintrin_modmul64(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t hi, lo;
    lo = _cintrin_wmul128(a, b, &hi);
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

// 64 bit Modular Exponentiation of Modulus n
static inline uint64_t _cintrin_modexp64(uint64_t base, uint64_t exp, uint64_t mod) {
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


#endif