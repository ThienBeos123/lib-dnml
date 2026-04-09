#ifndef _LMACRO_H
#define _LMACRO_H


#include "../../test_ui/lowui.h"
// STDLIB utilities
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define bin32 (sizeof(uint32_t) * CHAR_BIT)
#define U32m UINT32_MAX
#define U64m UINT64_MAX
#define U63m (U64m >> 1)
#define U62m (U64m >> 2)

#define alt64_1 0xAAAAAAAAAAAAAAAA
#define alt64_2 0x5555555555555555
#define half64_1 0x00000000FFFFFFFF
#define half64_2 0xFFFFFFFF00000000


//* ======================================= *//
//* ========= GENERIC SETUP MACRO ========= *//
//* ======================================= *//
#define _DNML_INFER_STYLEE(fn) _Generic((fn),   \
    _fn1o_t: DNML_OCALL,                        \
    _fn2o_t: DNML_OCALL,                        \
    _fn3o_t: DNML_OCALL,                        \
    default: DNML_CALL                          \
)

#define _DNML_SUITE_SETUP( \
    suite, name,                \
    ecount, rcount, rin,        \
    edge, rand, ribuf, resbuf,  \
    test_fn, ref_fn, logp       \
) do { create_suite( \
    suite, name,            \
    ecount, rcount, rin,    \
    edge, rand,             \
    ribuf, resbuf, logp     \
); (suite)->fn_test = (void*)(test_fn);  \
(suite)->fn_ref    = (void*)(ref_fn);   \
(suite)->call_style = _DNML_INFER_STYLE(test_fn); \
} while (0); \

uint64_t barebone_rand(void) {
    uint64_t res = 0;
    for (int i = 0; i < 5; i++) {
        res = (res << 15) | (rand() & 0x7FFF);
    } return res;
}




//* ======================================= *//
//* ======= ARITHMETIC SUITE SETUP ======== *//
//* ======================================= *//
static inline void addc_setup(
    _libdnml_suite *s, const char *name,
    _libdnml_case *ecases, _libdnml_case *rcases, uint32_t rcount, 
    uint64_t **ribuf, _dnml_pair *resbuf, const char *addclp,
    uint64_t (*test_fn)(uint64_t, uint64_t, uint8_t*),
    uint64_t (*ref_fn)(uint64_t, uint64_t, uint8_t*)
) { 
    srand(time(NULL)); static uint8_t addc_in = 2;
    // Basic Tests
    ecases[0] = (_libdnml_case){ .in = {0, 0, 0}, .exp = {0, 0}, .input_count = addc_in };
    ecases[1] = (_libdnml_case){ .in = {0, 0, 1}, .exp = {1, 0}, .input_count = addc_in };
    ecases[2] = (_libdnml_case){ .in = {1, 2, 0}, .exp = {3, 0}, .input_count = addc_in };
    ecases[3] = (_libdnml_case){ .in = {1, 2, 1}, .exp = {4, 0}, .input_count = addc_in };
    // Rollover/Edge tests
    ecases[4] = (_libdnml_case){ .in = {U64m, 0, 0}, .exp = {U64m, 0}, .input_count = addc_in };
    ecases[5] = (_libdnml_case){ .in = {U64m, 1, 0}, .exp = {0, 1}, .input_count = addc_in };
    ecases[6] = (_libdnml_case){ .in = {U64m, 0, 1}, .exp = {0, 1}, .input_count = addc_in };
    ecases[7] = (_libdnml_case){ .in = {U64m-1, 0, 0}, .exp = {U64m, 0}, .input_count = addc_in };
    ecases[8] = (_libdnml_case){ .in = {U64m, 1, 1}, .exp = {1, 1}, .input_count = addc_in };
    ecases[9] = (_libdnml_case){ .in = {1, U64m, 1}, .exp = {1, 1}, .input_count = addc_in };
    ecases[10] = (_libdnml_case){ .in = {U64m, U64m, 0}, .exp = {U64m-1, 1}, .input_count = addc_in };
    ecases[11] = (_libdnml_case){ .in = {U64m, U64m, 1}, .exp = {U64m,   1}, .input_count = addc_in };
    // Bitwise Activity tests
    ecases[12] = (_libdnml_case){ .in = {alt64_1, alt64_2, 0},   .exp = {U64m, 0}, .input_count = addc_in };
    ecases[13] = (_libdnml_case){ .in = {alt64_1, alt64_2, 1},   .exp = {0, 1}, .input_count = addc_in };
    ecases[14] = (_libdnml_case){ .in = {half64_1, half64_2, 0}, .exp = {U64m, 0}, .input_count = addc_in };
    ecases[15] = (_libdnml_case){ .in = {half64_1, half64_2, 1}, .exp = {0, 1}, .input_count = addc_in };
    ecases[16] = (_libdnml_case){ .in = {U64m>>1, U64m>>1, 0},       .exp = {0, 1}, .input_count = addc_in };
    // Signed Arithmetic overlap / ALU tests
    ecases[17] = (_libdnml_case){ .in = {alt64_1, alt64_2, 1}, .exp = {0, 1}, .input_count = addc_in };
    ecases[18] = (_libdnml_case){ .in = {5,         U64m - 4,  0}, .exp = {0, 1}, .input_count = addc_in };
    ecases[19] = (_libdnml_case){ .in = {U64m>>1, 0, 1}, .exp = {U64m>>1+1, 0}, .input_count = addc_in };
    // Randomly Generated Cases filling
    for (uint32_t i = 0; i < rcount; ++i) {
        uint64_t in3 = (barebone_rand() & 1) ? 1 : 0;
        rcases[i] = (_libdnml_case){ 
            .in = {barebone_rand(), barebone_rand(), in3}, 
            .input_count = addc_in 
        };
    } _DNML_SUITE_SETUP(s, name, 20, rcount, 
        addc_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, addclp
    ); return;
}
static inline void subb_setup(
    _libdnml_suite *s, const char *name,
    _libdnml_case *ecases, _libdnml_case *rcases, uint32_t rcount, 
    uint64_t **ribuf, _dnml_pair *resbuf, const char *subblp,
    uint64_t (*test_fn)(uint64_t, uint64_t, uint8_t*),
    uint64_t (*ref_fn)(uint64_t, uint64_t, uint8_t*)
) { 
    srand(time(NULL)); static uint8_t subb_in = 2;
    // Basic Tests
    ecases[0] = (_libdnml_case){ .in = {0, 0, 0}, .exp = {0, 0}, .input_count = subb_in };
    ecases[1] = (_libdnml_case){ .in = {1, 0, 0}, .exp = {1, 0}, .input_count = subb_in };
    ecases[2] = (_libdnml_case){ .in = {5, 5, 0}, .exp = {0, 0}, .input_count = subb_in };
    ecases[3] = (_libdnml_case){ .in = {9, 4, 1}, .exp = {4, 0}, .input_count = subb_in };
    ecases[4] = (_libdnml_case){ .in = {9, 8, 1}, .exp = {0, 0}, .input_count = subb_in };
    // Rollover/Underflow tests
    ecases[5] = (_libdnml_case){ .in = {16, 20, 0},     .exp = {U64m - 4, 1}, .input_count = subb_in };
    ecases[6] = (_libdnml_case){ .in = {0,  1,  0},     .exp = {U64m, 1}, .input_count = subb_in };
    ecases[7] = (_libdnml_case){ .in = {15, U64m, 1},   .exp = {14, 1}, .input_count = subb_in };
    ecases[8] = (_libdnml_case){ .in = {0,  0, 1},      .exp = {U64m, 1}, .input_count = subb_in };
    ecases[9] = (_libdnml_case){ .in = {10, 10, 1},     .exp = {U64m, 1}, .input_count = subb_in };
    // Near the edge tests
    ecases[10] = (_libdnml_case){ .in = {U64m, U64m, 0}, .exp = {U64m-1, 1}, .input_count = subb_in };
    ecases[11] = (_libdnml_case){ .in = {U64m, U64m, 1}, .exp = {U64m, 1}, .input_count = subb_in };
    ecases[12] = (_libdnml_case){ .in = {U64m, 0, 0},    .exp = {U64m, 1}, .input_count = subb_in };
    ecases[13] = (_libdnml_case){ .in = {U64m, 0, 1},    .exp = {U64m-1, 0}, .input_count = subb_in };
    ecases[14] = (_libdnml_case){ .in = {0,    U64m, 0}, .exp = {1, 1}, .input_count = subb_in };
    ecases[15] = (_libdnml_case){ .in = {0,    U64m, 1}, .exp = {0, 1}, .input_count = subb_in };
    // Bitwise Stress Test
    ecases[16] = (_libdnml_case){ .in = {alt64_1, alt64_2, 0}, .exp = {alt64_2, 0}, .input_count = subb_in };
    ecases[17] = (_libdnml_case){ .in = {alt64_2, alt64_1, 1}, .exp = {alt64_1+1, 1}, .input_count = subb_in };
    ecases[18] = (_libdnml_case){ .in = {U64m>>1+1, U64m>>1, 1},   .exp = {0, 0}, .input_count = subb_in };
    // API/Function Logic test
    ecases[19] = (_libdnml_case){ .in = {5, U64m, 1}, .exp = {5, 1}, .input_count = subb_in };
    // Randomly Generated Cases filling
    for (uint32_t i = 0; i < rcount; ++i) {
        uint64_t in3 = (barebone_rand() & 1) ? 0 : 1;
        rcases[i] = (_libdnml_case){ 
            .in = {barebone_rand(), barebone_rand(), in3}, 
            .input_count = subb_in
        };
    } _DNML_SUITE_SETUP(s, name, 20, rcount,
        subb_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, subblp
    ); return;
}
static inline void wmul_setup(
    _libdnml_suite *s, const char *name,
    _libdnml_case *ecases, _libdnml_case *rcases, uint32_t rcount, 
    uint64_t **ribuf, _dnml_pair *resbuf, const char *wmullp,
    uint64_t (*test_fn)(uint64_t, uint64_t, uint64_t*),
    uint64_t (*ref_fn)(uint64_t, uint64_t, uint64_t*)
) { 
    srand(time(NULL)); static uint8_t wmul_in = 2;
    // Basic Tests
    ecases[0] = (_libdnml_case){ .in = {0, 0}, .exp = {0, 0}, .input_count = wmul_in };
    ecases[1] = (_libdnml_case){ .in = {1, 1}, .exp = {1, 0}, .input_count = wmul_in };
    ecases[2] = (_libdnml_case){ .in = {1, 0}, .exp = {0, 0}, .input_count = wmul_in };
    ecases[3] = (_libdnml_case){ .in = {9, 8}, .exp = {72, 0}, .input_count = wmul_in };
    // Extreme Casees
    ecases[4] = (_libdnml_case){ .in = {U64m, U64m},     .exp = {1, U64m-2}, .input_count = wmul_in };
    ecases[5] = (_libdnml_case){ .in = {U63m+1, 2},      .exp = {0, 1},      .input_count = wmul_in };
    ecases[6] = (_libdnml_case){ .in = {U63m+1, U63m+1}, .exp = {0, U62m+1}, .input_count = wmul_in };
    ecases[7] = (_libdnml_case){ .in = {U32m, U32m}, .exp = {0xFFFFFFFE00000001, 0}, .input_count = wmul_in };
    ecases[8] = (_libdnml_case){ .in = {U32m+1, U32m+1}, .exp = {0, 1}, .input_count = wmul_in };
    ecases[9] = (_libdnml_case){ .in = {half64_2, half64_1}, .exp = {U32m+1, half64_1-1}, .input_count = wmul_in };
    ecases[10] = (_libdnml_case){ 
        .in =  {alt64_1,          alt64_2         }, 
        .exp = {0x1C71C71C71C71C72, 0x38E38E38E38E38E3}, 
        .input_count = wmul_in 
    };
    ecases[11] = (_libdnml_case){ .in = {U64m, U64m-1}, .exp = {2, U64m-2}, .input_count = wmul_in };
    ecases[12] = (_libdnml_case){ 
        .in = {U64m,                0x0001000100010001}, 
        .exp = {0xFFEFFFEFFFEFFFFF, 0x0001000100010000},
        .input_count = wmul_in 
    };
    ecases[13] = (_libdnml_case){ .in = {U32m, U64m}, .exp = {half64_2+1, half64_1-1}, .input_count = wmul_in };
    ecases[14] = (_libdnml_case){ 
        .in =  {0xFFFFFFFFFFFFFFC5, 3}, 
        .exp = {0xFFFFFFFFFFFFFF4F, 2}, 
        .input_count = wmul_in 
    };
    // Randomly Generated Cases filling
    for (uint32_t i = 0; i < rcount; ++i) {
        rcases[i] = (_libdnml_case){ 
            .in = {barebone_rand(), barebone_rand(), 0},
            .input_count = wmul_in
        };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        wmul_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}
static inline void wdiv_setup(
    _libdnml_suite *s, const char *name,
    _libdnml_case *ecases, _libdnml_case *rcases, uint32_t rcount, 
    uint64_t **ribuf, _dnml_pair *resbuf, const char *wmullp,
    uint64_t (*test_fn)(uint64_t, uint64_t, uint64_t*),
    uint64_t (*ref_fn)(uint64_t, uint64_t, uint64_t*)
) {
    srand(time(NULL)); static uint8_t wdiv_in = 3;
    // Upper Limits
    ecases[0] = (_libdnml_case){ .in = {U64m, U64m-1, U64m}, .exp = {U64m, U64m-1}, .input_count = wdiv_in };
    ecases[1] = (_libdnml_case){ .in = {U64m-1, 1, U64m},    .exp = {2, 0}, .input_count = wdiv_in };
    ecases[2] = (_libdnml_case){ .in = {U64m,   1, U64m},    .exp = {2, 1}, .input_count = wdiv_in };
    ecases[3] = (_libdnml_case){ .in = {U64m-1, 0, U64m},    .exp = {0, U64m-1}, .input_count = wdiv_in };
    /// Boundary Cases
    ecases[4] = (_libdnml_case){ .in = {0, 1, 2}, .exp = {U63m+1, 0}, .input_count = wdiv_in };
    ecases[5] = (_libdnml_case){ .in = {1, 1, 2}, .exp = {U63m+1, 1}, .input_count = wdiv_in };
    ecases[6] = (_libdnml_case){ 
        .in = {half64_2,  half64_1, U32m+1}, 
        .exp = {U64m,       0}, 
        .input_count = wdiv_in 
    }; 
    ecases[7] = (_libdnml_case){ .in = {U63m, U64m, U63m+1}, .exp = {U64m, U63m}, .input_count = wdiv_in };
    // Assymertical Tests
    ecases[8] = (_libdnml_case){ .in = {U64m, 0, 1},        .exp = {U64m, 0}, .input_count = wdiv_in };
    ecases[9] = (_libdnml_case){ .in = {U32m, 0, U64m},     .exp = {0, U32m}, .input_count = wdiv_in };
    ecases[10] = (_libdnml_case){ .in =  {U64m, 0, U64m},   .exp = {1, 0}, .input_count = wdiv_in };
    ecases[11] = (_libdnml_case){ .in =  {U64m-1, 0, U63m}, .exp = {1, U63m}, .input_count = wdiv_in };
    // Bitwise Handling Tests
    ecases[12] = (_libdnml_case){ 
        .in = {half64_1-1,  half64_2+1, U32m}, 
        .exp = {half64_2,   1},
        .input_count = wdiv_in 
    };
    ecases[13] = (_libdnml_case){ 
        .in = {alt64_1, alt64_2, alt64_1}, 
        .exp = {U63m+1, alt64_2}, 
        .input_count = wdiv_in
    };
    ecases[14] = (_libdnml_case){ 
        .in = {0x13579BDF2468ACE0, 0x123456789ABCDEF0, U63m+1},
        .exp = {0x2468ACF13579BDE0, 0x13579BDF2468ACE0},
        .input_count = wdiv_in 
    };
    // Randomly Generated Cases filling
    for (uint32_t i = 0; i < rcount; ++i) {
        uint64_t in2 = barebone_rand();
        while (!in2) in2 = barebone_rand();
        rcases[i] = (_libdnml_case) {
            .in = {barebone_rand(), in2, 0},
            .input_count = wdiv_in
        };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        wdiv_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}





#endif