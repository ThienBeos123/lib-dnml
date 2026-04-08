// Functions to compare
#include "../../../intrinsics/arm64/_arm64_conn.h"
#include "../../../intrinsics/zvanillc/_vanillc_conn.h"
// Utilities
#include "../../../test_ui/lowui.h"
#include "../low_setup.h"
// STDLIB utilities
#include <stdint.h>
#include <stdio.h>

#define u64 uint64_t
#define u32 uint32_t


int main(int argc, char **argv) {
    // ----------------- PRE-TEST SETUP ----------------- //
    // Parse terminal args + Setup env constants
    uint32_t rcount = 100;
    // Buffer Setup
    _libdnml_case ecases[20]; _libdnml_case rcases[rcount];
    uint64_t rinbuf[rcount][3]; _dnml_pair resbuf[resbuf_size(rcount, 20)];

    // -------------- THE ACTUAL TESTS -------------- //
    // Testing ARM64 Add Carry - _arm64_add64c
    _libdnml_suite add64c_suite; addc_setup(
        &add64c_suite, "add64c - ARM64", ecases, 
        rcases, rcount, rinbuf, resbuf, 
        "../logs/arm64_arith.txt",
        _arm64_add64c, _cintrin_add64c);
    // Testing ARM64 Sub borrow - _arm64_sub64b
    _libdnml_suite sub64b_suite; subb_setup(
        &sub64b_suite, "sub64b - ARM64", ecases, 
        rcases, rcount, rinbuf, resbuf, 
        "../logs/arm64_arith.txt",
        _arm64_sub64b, _cintrin_sub64b
    );

    return 0;
}