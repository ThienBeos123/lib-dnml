#ifndef ___DNML_INTRIN_UTIL
#define ___DNML_INTRIN_UTIL



#include <stdint.h>
#include <math.h>
#include "../system/compiler.h"
#include "../sconfigs/numeric_config.h"

static inline uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp) {
    if (exp == 0) return 1;
    if (exp == 1) return 1;
    if (exp == 2) return (base <= (1ULL << 32) - 1);
    return (double)exp * log2((double)base) < (double)(BITS_IN_UINT64_T);
}
static inline uint8_t __IS_2POW__(uint64_t x) { 
    return (x) && !(x & (x - 1)); 
}





#endif