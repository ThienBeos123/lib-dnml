#ifndef ___LIBDNML_TEST_UI
#define ___LIBDNML_TEST_UI

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *suite_name;

    uint8_t edge_cases_count;
    uint8_t edge_cases_correct;
    uint64_t *fail_edge_in1;
    uint64_t *fail_edge_in2;
    uint64_t *fail_edge_res;
    uint64_t *fail_edge_exp;

    uint8_t random_cases_count;
    uint8_t random_cases_correct;
    uint64_t *fail_rand_in1;
    uint64_t *fail_rand_in2;
    uint64_t *fail_rand_res;
    uint64_t *fail_rand_exp;
} _libdnml_suite;


typedef struct {
    char *session_name;
    uint8_t suite_count;
    _libdnml_suite *suites;
    uint8_t cli_delay;
} _libdnml_session;





#endif