#include "../../../intrinsics/x86_64/_x86_conn.h"
#include "../../../intrinsics/zvanillc/_vanillc_conn.h"
// Utilities
#include "../../../test_ui/lowui.h"
#include "../low_setup.h"
// STDLIB utilities
#include <stdint.h>
#include <stdio.h>



int main(int argc, char **argv) {
    // ----------------- PRE-TEST SETUP ----------------- //
    // Parse terminal args + Setup env constants
    u32 rcount = 100;
    // Buffer Setup
    case ecases[20]; case rcases[rcount];
    u64 rinbuf[rcount][3]; pair resbuf[resbuf_size(rcount, 20)];

    // -------------- ARITHMETIC TEST SUITES -------------- //
    // Testing x86_64 Add Carry - _arm64_add64c
    _libdnml_lsuite add64c_suite; addc_setup(
        &add64c_suite, "add64c - x86_64", ecases, 
        rcases, rcount, rinbuf, resbuf, 
        "../logs/x86_arith.txt",
        _x86_add64c, _cintrin_add64c
    );
    // Testing x86_64 Sub borrow - _arm64_sub64b
    _libdnml_lsuite sub64b_suite; subb_setup(
        &sub64b_suite, "sub64b - x86_64", ecases, 
        rcases, rcount, rinbuf, resbuf, 
        "../logs/x86_arith.txt",
        _x86_sub64b, _cintrin_sub64b
    );
    // Testing x86_64 128 bit Wide Multiplication
    _libdnml_lsuite wmul128_suite; wmul_setup(
        &wmul128_suite, "wmul128 - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_arith.txt",
        _x86_wmul128, _cintrin_wmul128
    );
    _libdnml_lsuite wdiv128_suite; wdiv_setup(
        &wdiv128_suite, "wdiv128 - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_arith.txt",
        _x86_wdiv128, _cintrin_wdiv128
    );


    // -------------- MODULAR ARITHETMIC TEST SUITES -------------- //
    _libdnml_lsuite modinv64_suite; modinv_setup(
        &modinv64_suite, "modinv64 - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_marith.txt",
        _x86_modinv64, _cintrin_modinv64
    );


    // -------------- BITWISE OPERATIONS TEST SUITES -------------- //
    _libdnml_lsuite clz64_suite; clz_setup(
        &clz64_suite, "clz64e - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_bitops.txt",
        _x86_clz64e, _cintrin_clz64
    );
    _libdnml_lsuite ctz64_suite; ctz_setup(
        &ctz64_suite, "ctz64e - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_bitops.txt",
        _x86_ctz64e, _cintrin_ctz64
    );
    _libdnml_lsuite bswap64_suite; bswap_setup(
        &bswap64_suite, "bswap64 - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_bitops.txt",
        _x86_bswap64, _cintrin_bswap64
    );
    _libdnml_lsuite pcnt64_suite; pcnt_setup(
        &pcnt64_suite, "pcnt64 - x86_64", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/x86_bitops.txt",
        _x86_pcnt64e, _cintrin_clz64
    );

    return 0;
}