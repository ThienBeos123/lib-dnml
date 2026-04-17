#ifndef ___LIBDNML_IOTEST_UI
#define ___LIBDNML_IOTEST_UI


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include "../adynamol/big_numbers/bigNums.h"
#include "../sconfigs/dnml_status.h"
#include "_test_base.h"


//* =========== TYPE DEFINITIONS =========== *//
// Small, supporting types
typedef enum res_type { NONE, BIGINT, STRING } operated_types;
typedef enum _dnml_scase_type {
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
} _dnml_scase_type;


// Input and Results containers
typedef struct str_res {
/* Notes:
    Instead of using a union to store
    the varying return types of I/O operations,
    we seperate results into entirely different struct fields.
    This is for:
        - Using a union won't allow for the definition
            of an incomplete array, forcing the usage of pointers
            ----> Complicated testing and storing 

        - Incomplete struct fields allow for the independent
            storage of the string by the struct header, simplifying
            string storage instead of relying on external storage
*/
    operated_types type;
    dnml_status status;
    bigInt bi;
    char str[];
} str_res;
typedef union sinput_cases{
    // BigInt --> String (BI TO S)
    struct { char* str; size_t len; const bigInt x; uint8_t base; bool uppercase; } bitos_conv;
    struct { FILE *stream; const bigInt x; uint8_t base; bool uppercase; } bitos_print;
    struct { FILE *stream; const bigInt x; } bitos_serial;
    struct { const bigInt x; bool uppercase; } bitos_dump;
    // String --> BigInt (S TO BI)
    struct { bigInt *x; const char* str; size_t len; uint8_t base; } stobi_init;
    struct { const char* str; size_t len; uint8_t base; dnml_status *err; } stobi_conv;
    struct { bigInt *x; const char *str; size_t len; uint8_t base; } stobi_assign;
    struct { FILE *stream; bigInt *x; uint8_t base; } stobi_scan;
    struct { FILE *stream; const char* str; size_t len; dnml_status *err; } stobi_deserial;
    struct { FILE *stream; bigInt *x; } stobi_fread;
} sinput_cases;


// Cases & Suites
typedef struct _libdnml_scase {
    uint8_t inc;
    _dnml_scase_type case_type;
    sinput_cases in;
    str_res exp;
} _libdnml_scase;
typedef struct _libdnml_str_suite {
    const char *suite_name;
    void *fn_test;  void *fn_ref;
    _dnml_call_style call_style;
    const char *log_path;

    // Edge cases storage
    _libdnml_scase *edge;
    uint8_t ecount;     uint8_t ecorrect;
    str_res *fail_eres; str_res *fail_eexp;

    // Random cases storage
    _libdnml_scase *rand;
    uint16_t rcount;        uint16_t rcorrect;  uint8_t rnin;
    sinput_cases *fail_rin; str_res *fail_rres; str_res *fail_rexp;
} _libdnml_lsuite;


//* =================== TEST CREATION FUNFCTIONS =================== *//
static inline void create_str_suite(
    _libdnml_str_suite *curr_suite, const char *name,
    uint8_t ecount, uint16_t rcount, uint8_t rnin,
    _libdnml_scase *ebank, _libdnml_scase *rbank,
    sinput_cases *fail_rinbuf, str_res *fail_resbuf,
    const char *log_path
) {
    curr_suite->suite_name = name;
    curr_suite->ecount = ecount;
    curr_suite->rcount = rcount;
    curr_suite->rnin = rnin;
    curr_suite->log_path = log_path;
    // Filling in the banks
    curr_suite->edge = ebank;
    curr_suite->rand = rbank;
    // Assigning result storage
    curr_suite->fail_eres = &fail_resbuf[0];
    curr_suite->fail_eexp = &fail_resbuf[ecount];
    curr_suite->fail_rin  = fail_rinbuf;
    curr_suite->fail_rexp = &fail_resbuf[2 * ecount];
    curr_suite->fail_rres = &fail_resbuf[2 * ecount + rcount];
}
static inline void create_str_session(
    _libdnml_session *curr_session,
    const char *session_name, uint8_t cli_delay,
    uint8_t suite_count, _libdnml_str_suite *suite_list,
    _dnml_output_mode output_mode
) {
    curr_session->session_name = session_name;
    curr_session->cli_delay = cli_delay;
    curr_session->suite_count = suite_count;
    curr_session->suites = suite_list;
    curr_session->output_mode = output_mode;
    curr_session->box_width = _dnml_box_width();
}


//* =================== FUNCTION-GENERALIZATION DISPATCHER =================== *//
// BigInt --> String Function signatures
typedef dnml_status (*bitos_conv_fn)(char*, size_t, const bigInt, uint8_t, bool);
typedef void (*bitos_print_fn)(FILE*, const bigInt, uint8_t, bool);
typedef void (*bitos_serial_fn)(FILE*, const bigInt);
typedef void (*bitos_dump_fn)(const bigInt, bool uppercase);
// String --> BigInt Function signatures
typedef void (*stobi_init_fn)(bigInt*, const char*, size_t, uint8_t);
typedef void (*stobi_conv_fn)(const char*, size_t uint8_t, dnml_status*);
typedef void (*stobi_assign_fn)(bigInt*, const char*, size_t, uint8_t);
typedef void (*stobi_scan_fn)(FILE*, bigInt*, uint8_t);
typedef void (*stobi_deserial_fn)(FILE*, const char*, size_t, dnml_status);
typedef void (*stobi_fread_fn)(FILE*, bigInt*);

static str_res run_bitos_case(const _libdnml_scase *c, size_t bufsize, void *fn) {
    switch (c->case_type) {
        case _BITOS_CASE_CONV: {
            char buf[bufsize]; str_res ret = {.type = STRING};
            ret.status = ((bitos_conv_fn)fn)(
                buf, bufsize, 
                c->in.bitos_conv.x,
                c->in.bitos_conv.base,
                c->in.bitos_conv.uppercase
            ); snprintf(ret.str, bufsize, "%.*s", bufsize, buf);
            return ret;
        }
    };
}



//* ============== FULL SUITES/SESSIONS RENDER FUNCTIONS ============== *//



#endif