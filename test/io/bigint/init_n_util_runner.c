// Functions to compare
#include "../../../adynamol/big_numbers/bigInt_func.h"
// Utilities
#include "../../../sconfigs/memory/_scratch.h"
#include "../../../util/util.h"
#include "../_ioconv.h"
// Utility Components
#include "../iobuf.h"
#include "../_ioconv.h"

// Testing framework
#include "../../../test_ui/_strui.h"
#include "bi_indef.h"
#include "bi_exec_func.h"
#include "bi_eval_fn.h"
#include "bi_util_func.h"
// STDLIB utilities
#include <stdint.h>
#include <stdio.h>

limb_t multi_val[7] = {123, 255, 42, 63, 0, 17720, 22875};
limb_t big_limbs[9] = {
    0, 1, // First bigInt
    // Second bigInt
    UINT64_C(18446744073709551615),
    UINT64_C(18446744073709551615),
    UINT64_C(279109966635548160),
    UINT64_C(2554917779393558781),
    UINT64_C(14124656261812188652),
    UINT64_C(11976055582626787546),
    UINT64_C(2537941837315),
};
_libdnml_scase ecases[15] = {
    /* -------------------------------------------------------------------------------------------- */
    // Case Number  |   Input                               |   Expected Ouput                       /
    /* ------------------------------ PREPARSE LEXICAL FAILURE CASE ------------------------------- */ 
    { // 1.         |   ""                              ---->   STR_EMPTY                            /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 2.      |   "  "                            ---->   STR_EMPTY                            /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_EMPTY, .cap = 0, INVAL_BI() }
    }, { // 3.      |   "+"                             ---->   STR_INCOMPLETE                       /
        .in = &(stobi_init_in){ .str = "  ", .len = 3, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 4.      |   "-"                             ---->   STR_INCOMPLETE                       /
        .in = &(stobi_init_in){ .str = "+", .len = 2, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 5.      |   "-0"                            ---->   STR_INVALID_SIGN                     /
        .in = &(stobi_init_in){ .str = "-0", .len = 3, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_SIGN, .cap = 0, INVAL_BI() }
    },
    /* --------------------------------- ON-THE-EDGE SUCCESS CASE --------------------------------- */ 
    { // 6.         |   "+0"                            ---->   STR_SUCCESS (0)                      /
        .in = &(stobi_init_in){ .str = "+0", .len = 3, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { // 7.      |   "0"                             ---->   STR_SUCCESS (0)                      /
        .in = &(stobi_init_in){ .str = "0", .len = 2, .base = 10 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { // 8.      |   "000000"                        ---->   STR_SUCCESS (0)                      /
        .in = &(stobi_init_in){ .str = "000000", .len = 7, .base = 10 },
        .exp = {
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { // 9.      |   "   123"                        ---->   STR_SUCCESS (0)                      /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[0], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { // 10.     |   "18446744073709551616"          ---->   STR_SUCCESS (18446744073709551616)   /
        .in = &(stobi_init_in){ .str = "18446744073709551616", .len = 21, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = big_limbs, .n = 2, .cap = 2, .sign = 1 }
        }
    }, { // 11.     |   "999999...999999" (Len 128)     ---->   STR_SUCCESS (10^128 - 1)             /
        .in = &(stobi_init_in){ .str = " \
            99999999999999999999999999999999 \
            99999999999999999999999999999999 \
            99999999999999999999999999999999 \
            99999999999999999999999999999999 \
        ", .len = 129, .base = 10 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0, 
            .data.bi = { .limbs = &big_limbs[2], .n = 7, .cap = 7, .sign = 1 }
        }
    },
    /* -------------------------- NUMERICAL-SEGMENT DIGIT-BASED FAILURE --------------------------- */
    { // 12.        |   "1234abcd"                      ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "1234abcd", .len = 9, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 13.     |   "1234    "                      ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "1234    ", .len = 9, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 14.     |   "abcd1234"                      ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "abcd1234", .len = 9, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 15.     |   "1234_5678"                     ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "1234_5678", .len = 10, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, 
    /* --------------------------------------------------------------------------------------------- */
};
_libdnml_scase ecases_bprefix[12] = {
    /* -------------------------------------------------------------------------------------------- */
    // Case Number  |   Input                               |   Expected Ouput                       /
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { // 1.         |   "0x"                            ---->   STR_INCOMPLETE                       /
        .in = &(stobi_init_in){ .str = "0x", .len = 3, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 2.      |   "0xFG"                          ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "0xFG", .len = 0, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 3.      |   "0b102"                         ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "0b102", .len = 6, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 4.      |   "0o89"                          ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "0o89", .len = 5, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 5.      |   "00x123"                        ---->   STR_INVALID_BASE_PREFIX              /
        .in = &(stobi_init_in){ .str = "00x123", .len = 0, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    }, { // 6.      |   "0{11)1234A"                    ---->   STR_INVALID_BASE_PREFIX              /
        .in = &(stobi_init_in){ .str = "0{11)1234A", .len = 11, .base = 0 },
        .exp = { .type = BIGINT, .status = STR_INVALID_BASE_PREFIX, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { // 7.         |   "0x0"                           ---->   STR_SUCCESS (0)                      /
        .in = &(stobi_init_in){ .str = "0x0", .len = 4, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[4], .n = 0, .cap = 1, .sign = 1 }
        }
    }, { // 8.      |   "0xFF"                          ---->   STR_SUCCESS (255)                    /
        .in = &(stobi_init_in){ .str = "0xFF", .len = 4, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[1], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { // 9.      |   "0b101010"                      ---->   STR_SUCCESS (42)                     /
        .in = &(stobi_init_in){ .str = "0b101010", .len = 9, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[2], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { // 10.     |   "0o77"                          ---->   STR_SUCCESS (63)                     /
        .in = &(stobi_init_in){ .str = "0o77", .len = 5, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[3], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { // 11.     |   "0{11}1234A"                    ---->   STR_SUCCESS (17720)                  /
        .in = &(stobi_init_in){ .str = "0{11}1234A", .len = 11, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { // 12.     |   "0{7}123456"                    ---->   STR_SUCCESS (22875)                  /
        .in = &(stobi_init_in){ .str = "0{7}123456", .len = 11, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[6], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* --------------------------------------------------------------------------------------------- */
};
_libdnml_scase ecases_rawb[12] = {
    /* -------------------------------------------------------------------------------------------- */
    // Case Number  |   Input                               |   Expected Ouput                       /
    /* ----------------------------------- BASE-SPECIFIC FAILURE ---------------------------------- */
    { // 1.         |   "0x"                            ---->   STR_INCOMPLETE                       /
        .in = &(stobi_init_in){ .str = "0x", .len = 3, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 2.      |   "0xFG"                          ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "0xFG", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INVALID_DIGIT, .cap = 0, INVAL_BI() }
    }, { // 3.      |   "0b102"                         ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 4.      |   "0o89"                          ---->   STR_INVALID_DIGIT                    /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 5.      |   "00x123"                        ---->   STR_INVALID_BASE_PREFIX              /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 6.      |   "0{11)1234A"                    ---->   STR_INVALID_BASE_PREFIX              /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    },
    /* ---------------------------------- BASE-SPECIFIC SUCCESSES --------------------------------- */
    { // 7.         |   "0x0"                           ---->   STR_SUCCESS (0)                      /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 8.      |   "0xFF"                          ---->   STR_SUCCESS (255)                    /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 9.      |   "0b101010"                      ---->   STR_SUCCESS (42)                     /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 10.     |   "0o77"                          ---->   STR_SUCCESS (63)                     /
        .in = &(stobi_init_in){ .str = "", .len = 0, .base = 10 },
        .exp = { .type = BIGINT, .status = STR_INCOMPLETE, .cap = 0, INVAL_BI() }
    }, { // 11.     |   "0{11}1234A"                    ---->   STR_SUCCESS (17720)                  /
        .in = &(stobi_init_in){ .str = "0{11}1234A", .len = 11, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[5], .n = 1, .cap = 1, .sign = 1 }
        }
    }, { // 12.     |   "0{7}123456"                    ---->   STR_SUCCESS (22875)                  /
        .in = &(stobi_init_in){ .str = "0{7}123456", .len = 11, .base = 0 },
        .exp = { 
            .type = BIGINT, .status = STR_SUCCESS, .cap = 0,
            .data.bi = { .limbs = &multi_val[6], .n = 1, .cap = 1, .sign = 1 }
        }
    }, 
    /* --------------------------------------------------------------------------------------------- */
};



int main(int argc, char **argv) {
    // ----------------- PRE-TEST SETUP ----------------- //
    // Parse terminal args + Setup env constants
    u8 suite_count = 8;
    u16 rcount = (argc >= 1) ? (u16)(_stou64(argv[1], strlen(argv[1]))) : 100;
    _dnml_output_mode init_omode; if (argc >= 2) {
        uint8_t sesh_count = _stou64(argv[2], strlen(argv[2]));
        init_omode = (sesh_count <= 3) ? DNML_VOUT : DNML_COUT;
    } else init_omode = DNML_VOUT;
    // Buffer Set

    
    return 0;
}