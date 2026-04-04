#ifndef ___LIBDNML_TEST_UI
#define ___LIBDNML_TEST_UI

#include "../sconfigs/_ctx.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



//* =========== TYPE DEFINITIONS =========== *//
typedef struct {
    const char *suite_name;

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
    const char *session_name;
    uint8_t suite_count;
    _libdnml_suite *suites;
    uint32_t cli_delay; // in ms
} _libdnml_session;



//* =================== TESTING FUNFCTIONS =================== *//
static inline void destroy_suite(_libdnml_suite *suite, calc_ctx fail_storage, size_t suite_mark) {
    scratch_reset(&fail_storage, suite_mark);
    suite = NULL;
}
static inline void create_suite(
    _libdnml_suite *curr_suite, const char *suite_name,
    uint8_t edge_count, uint8_t rand_count, 
    calc_ctx fail_storage, size_t *suite_mark
) {
    curr_suite->suite_name = suite_name;
    curr_suite->edge_cases_count = edge_count;
    *suite_mark = scratch_mark(&fail_storage);
    curr_suite->fail_edge_in1 = (uint64_t*)(scratch_alloc(&fail_storage, edge_count));
    curr_suite->fail_edge_in2 = (uint64_t*)(scratch_alloc(&fail_storage, edge_count));
    curr_suite->fail_edge_res = (uint64_t*)(scratch_alloc(&fail_storage, edge_count));
    curr_suite->fail_edge_exp = (uint64_t*)(scratch_alloc(&fail_storage, edge_count));

    curr_suite->random_cases_count = rand_count;
    curr_suite->fail_rand_in1 = (uint64_t*)(scratch_alloc(&fail_storage, rand_count));
    curr_suite->fail_rand_in2 = (uint64_t*)(scratch_alloc(&fail_storage, rand_count));
    curr_suite->fail_rand_res = (uint64_t*)(scratch_alloc(&fail_storage, rand_count));
    curr_suite->fail_rand_exp = (uint64_t*)(scratch_alloc(&fail_storage, rand_count));
}

static inline void start_session(
    _libdnml_session *curr_session,
    const char *session_name, uint8_t cli_delay,
    uint8_t suite_count, _libdnml_suite *suite_list
) {
    curr_session->session_name = session_name;
    curr_session->cli_delay = cli_delay;
    curr_session->suite_count = suite_count;
    curr_session->suites = suite_list;
}



//* =================== INTERFACE/UI TEXT =================== *//
#define BOX_TL      "┌"
#define BOX_TR      "┐"
#define BOX_BL      "└"
#define BOX_BR      "┘"
#define BOX_H       "─"
#define BOX_V       "│"
#define BOX_DIV_L   "├"
#define BOX_DIV_R   "┤"
#define BOX_WIDTH   60


//* ============== SUITE BOX FUNCTIONS ============== *//
static inline void _dnml_box_divider(void) {
    printf(BOX_DIV_L);
    for (int i = 0; i < BOX_WIDTH; i++) printf(BOX_H);
    printf(BOX_DIV_R "\n");
}
static inline void _dnml_box_top(const char* suite_name) {
    printf(BOX_TL); size_t namelen = strlen(suite_name);
    printf(" %s ", suite_name);
    for (int i = 0; i < BOX_WIDTH - namelen - 2; i++) printf(BOX_H);
    printf(BOX_TR "\n");
}
static inline void _dnml_box_bottom(void) {
    printf(BOX_BL);
    for (int i = 0; i < BOX_WIDTH; i++) printf(BOX_H);
    printf(BOX_BR "\n");
}
static inline void _dnml_box_line(const char *text) {
    int len = (int)strlen(text);
    int pad = BOX_WIDTH - len;
    if (pad < 0) pad = 0;
    printf(BOX_V " %.*s%*s" BOX_V "\n", BOX_WIDTH - 1, text, pad - 1, "");
}


//* ============== SESSION PROGRESS/FEATURES FUNCTIONS ============== *//
static inline void _dnml_delay_ms(uint32_t ms) {
    struct timespec ts = { 
        .tv_sec = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000L 
    }; nanosleep(&ts, NULL);
}
static inline void _dnml_loading(const char *label, uint32_t delay, uint32_t ticks) {
    const char *frames[] = { "|", "/", "-", "\\" };
    for (uint32_t i = 0; i < ticks; i++) {
        printf("\r  %s  %s ", frames[i % 4], label);
        fflush(stdout);
        _dnml_delay_ms(delay);
    }
    printf("\r%*s\r", BOX_WIDTH + 4, "");   // clear line
    fflush(stdout);
}
static inline void _dnml_session_progress(uint8_t done, uint8_t total, const char *session_name) {
    int barw = 40;
    int filled = (total) ? (done / total * barw) : 0;

    printf("  %s\n  Session progression: [", session_name);
    for (int i = 0; i < barw; i++)
        printf((i < filled) ? "#" : " ");
    printf("] %" PRIu8 "%\n\n", (total) ? (uint8_t)(done / total * 100) : 0);
    fflush(stdout);
}


//* ============== FULL SUITES/SESSIONS RENDER FUNCTIONS ============== *//
static inline void _dnml_render_suite(
    const _libdnml_suite *s,
    uint8_t suite_num,
    uint32_t delay_ms
) {
    _dnml_box_top(s->suite_name);
    _dnml_box_divider();
    _dnml_delay_ms(delay_ms);

    // ------ edge cases line ------
    char edge_line[BOX_WIDTH];
    snprintf(
        edge_line, sizeof(edge_line), "Edge case: %d/%d",
        s->edge_cases_correct, s->edge_cases_count
    ); _dnml_box_line(edge_line);
    _dnml_delay_ms(delay_ms);

    // print failed edge cases
    int fail_edge = s->edge_cases_count - s->edge_cases_correct;
    for (int i = 0; i < fail_edge; ++i) {
        char fail_line[BOX_WIDTH];
        snprintf(
            fail_line, sizeof(fail_line),
            "+) Case %d: Expected: 0x%016" PRIx64 " | Got: 0x%016" PRIx64 "",
            i + 1,
            (unsigned long long)s->fail_edge_exp[i],
            (unsigned long long)s->fail_edge_res[i]
        ); _dnml_box_line(fail_line);
        _dnml_delay_ms(delay_ms);
    }


    _dnml_box_divider();
    // ------ random cases line ------
    char rand_line[BOX_WIDTH];
    snprintf(rand_line, sizeof(rand_line), "Random case: %d/%d",
             s->random_cases_correct, s->random_cases_count);
    _dnml_box_line(rand_line);
    _dnml_delay_ms(delay_ms);

    // print failed random cases
    int fail_rand = s->random_cases_count - s->random_cases_correct;
    for (int i = 0; i < fail_rand; i++) {
        char fail_line[BOX_WIDTH];
        snprintf(
            fail_line, sizeof(fail_line),
            "+) Case %d: Expected: 0x%016" PRIx64 " | Got: 0x%016" PRIx64 "",
            i + 1,
            (unsigned long long)s->fail_rand_exp[i],
            (unsigned long long)s->fail_rand_res[i]
        ); _dnml_box_line(fail_line);
        _dnml_delay_ms(delay_ms);
    }
    _dnml_box_bottom();
    printf("\n");
    fflush(stdout);
}
static inline void render_session(const _libdnml_session *session) {
    // loading animation before session starts
    _dnml_loading("Loading session...", session->cli_delay, 12);
    for (uint8_t i = 0; i < session->suite_count; i++) {
        // update progress bar before each suite
        _dnml_session_progress(i, session->suite_count, session->session_name);
        // loading animation between suites
        if (i > 0) _dnml_loading("Running suite...", session->cli_delay, 8);
        _dnml_render_suite(&session->suites[i], i + 1, session->cli_delay);
    }
    // final progress bar at 100%
    _dnml_session_progress(session->suite_count, session->suite_count, session->session_name);
}




#endif