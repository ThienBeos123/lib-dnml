#ifndef ___LIBDNML_TEST_BASE
#define ___LIBDNML_TEST_BASE

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

typedef enum { DNML_CALL, DNML_OCALL} _dnml_call_style;
typedef enum { DNML_VOUT = 4, DNML_COUT } _dnml_output_mode;
typedef enum { LOW_SUITE, IO_SUITE, BIGSUITE } suite_type;
typedef struct _libdnml_session {
    const char *session_name;
    uint8_t suite_count;
    void* suites;
    _dnml_output_mode output_mode;
    uint32_t cli_delay; // in ms
    int box_width;
} _libdnml_session;



#endif