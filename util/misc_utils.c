#include "util.h"


inline size_t __BITCOUNT___(size_t digit_count, uint8_t base) {
    if (base = 10)          return digit_count * log2_10;
    else if (base == 16)    return digit_count * log2_16;
    else if (base == 2)     return digit_count * log2_2;
    else if (base == 8)     return digit_count * log2_8;
    else                    return digit_count * (log10(2) / log10(base));
}
inline uint8_t __BASEN_DCOUNT__(uint64_t val, uint8_t base) {
    return (size_t)(log10(val) / log10(base)) + 1;
}
inline uint64_t __MAG_I64__(int64_t val) {
    return (val == INT64_MIN) ?
        (uint64_t)(llabs(val + 1)) + 1 :
        (uint64_t)(llabs(val));
}
inline uint64_t _stou64(const char *buf, int buflen) {
    if (!buflen || !buf) return 0;
    uint64_t res = 0;
    for (size_t i = 0; i < buflen; ++i) {
        if (buf[i] == '\0') break;
        if (buf[i] < '0' || buf[i] > '9') return 0;
        uint8_t digit = (uint8_t)(buf[i] - '0');
        if (res > (UINT64_MAX - digit) / 10) return 0;
        res = (res * 10) + digit;
    } return res;
}