#include "../../../intrinsics/risc-v64/_rv64_conn.h"
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
    _libdnml_suite add64c_suite; addc_setup(
        &add64c_suite, "add64c - RISC-V 64 bit", ecases, 
        rcases, rcount, rinbuf, resbuf, 
        "../logs/rv64_arith.txt",
        _rv64_add64c, _cintrin_add64c
    );
    _libdnml_suite sub64b_suite; subb_setup(
        &sub64b_suite, "sub64b - RISC-V 64 bit", ecases, 
        rcases, rcount, rinbuf, resbuf, 
        "../logs/rv64_arith.txt",
        _rv64_sub64b, _cintrin_sub64b
    );
    _libdnml_suite wmul128_suite; wmul_setup(
        &wmul128_suite, "wmul128 - RISC-V 64 bit", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/rv64_arith.txt",
        _rv64_wmul128, _cintrin_wmul128
    );


    // -------------- MODULAR ARITHETMIC TEST SUITES -------------- //
    _libdnml_suite modinv64_suite; modinv_setup(
        &modinv64_suite, "modinv64 - RISC-V 64 bit", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/rv64_marith.txt",
        _rv64_modinv64, _cintrin_modinv64
    );


    // -------------- BITWISE OPERATIONS TEST SUITES -------------- //
    _libdnml_suite clz64_suite; clz_setup(
        &clz64_suite, "clz64e - RISC-V 64 bit", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/rv64_bitops.txt",
        _rv64_clz64, _cintrin_clz64
    );
    _libdnml_suite ctz64_suite; ctz_setup(
        &ctz64_suite, "ctz64e - RISC-V 64 bit", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/rv64_bitops.txt",
        _rv64_ctz64, _cintrin_ctz64
    );
    _libdnml_suite bswap64_suite; bswap_setup(
        &bswap64_suite, "bswap64 - RISC-V 64 bit", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/rv64_bitops.txt",
        _rv64_bswap64, _cintrin_bswap64
    );
    _libdnml_suite pcnt64_suite; pcnt_setup(
        &pcnt64_suite, "pcnt64 - RISC-V 64 bit", ecases,
        rcases, rcount, rinbuf, resbuf,
        "../logs/rv64_bitops.txt",
        _rv64_pcnt64, _cintrin_clz64
    );

    return 0;
}