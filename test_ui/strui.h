#ifndef ___LIBDNML_IOTEST_UI
#define ___LIBDNML_IOTEST_UI


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include "../adynamol/big_numbers/bigNums.h"
#include "../sconfigs/dnml_status.h"
#include "_test_base.h"


//* =========== TYPE DEFINITIONS =========== *//
typedef enum {
    _BITOS_CASE_CONV,
    _BITOS_CASE_ASSIGN,
    _BITOS_CASE_PRINT,
    _BITOS_CASE_SERIAL, // Includes fwrite & serialize
    _BITOS_CASE_DUMP,

    _STOBI_CASE_INIT,
    _STOBI_CASE_CONV,
    _STOBI_CASE_ASSIGN,
    _STOBI_CASE_SCAN,
    _STOBI_CASE_DESERIAL,
    _STOBI_CASE_FREAD
} _dnml_case_type;
typedef struct {
    enum { BIGINT, STRING, STATUS };
    union { bigInt bi; const char* str; dnml_status stat; };
} str_res;

typedef union input_cases{
    // BigInt --> String (BI TO S)
        struct { const bigInt x; uint8_t base; bool uppercase; } bitos_conv;
        struct { char* str; const bigInt x; uint8_t base; size_t len; } bitos_assign;
        struct { FILE *stream; const bigInt x; uint8_t base; bool uppercase; } bitos_print;
        struct { FILE *stream; const bigInt x; } bitos_serial;
        struct { const bigInt x; bool uppercase; } bitos_dump;
        // String --> BigInt (S TO BI)
        struct { bigInt *x; const char* str; uint8_t base; size_t len; } stobi_init;
        struct { const char* str; uint8_t base; size_t len; dnml_status *err; } stobi_conv;
        struct { bigInt *x; const char *str; uint8_t base; size_t len; } stobi_assign;
        struct { FILE *stream; bigInt *x; uint8_t base; } stobi_scan;
        struct { FILE *stream; const char* str; size_t len; dnml_status *err; } stobi_deserial;
        struct { FILE *stream; bigInt *x; } stobi_fread;
} input_cases;
typedef struct _libdnml_scase {
    uint8_t inc;
    _dnml_case_type case_type;
    input_cases in;
    union {
        const char *str;
        bigInt x;
        dnml_status err;
    } exp;
} _libdnml_case;


typedef struct _libdnml_lsuite {
    const char *suite_name;
    void *fn_test;
    void *fn_ref;
    _dnml_call_style call_style;
    const char *log_path;

    // Edge cases storage
    _libdnml_scase *edge;
    uint8_t ecount;
    uint8_t ecorrect;
    str_res *fail_eres;
    str_res *fail_eexp;

    // Random cases storage
    _libdnml_scase *rand;
    uint16_t rcount;
    uint16_t rcorrect;
    uint8_t rnin;
    input_cases *fail_rin;
    str_res *fail_rres;
    str_res *fail_rexp;
} _libdnml_lsuite;


//* =================== TEST CREATION FUNFCTIONS =================== *//



#endif