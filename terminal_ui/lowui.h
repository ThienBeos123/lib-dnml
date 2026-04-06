#ifndef ___LIBDNML_LTEST_UI
#define ___LIBDNML_LTEST_UI

#include "../sconfigs/_ctx.h"
#include "../sconfigs/__threads_t.h"
#include "../adynamol/big_numbers/bigNums.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include <stdatomic.h>



//* =========== TYPE DEFINITIONS =========== *//
// Helper Types
typedef enum { DNML_CALL, DNML_OCALL} _dnml_call_style;
typedef enum { DNML_VOUT, DNML_COUT } _dnml_output_mode;
typedef enum { TEST_CASUAL, TEST_HEAVY } _dnml_test_mode;
typedef struct { uint64_t first; uint64_t second; } _dnml_pair;

typedef struct {
    _dnml_pair exp;
    uint8_t input_count;
    uint64_t *in;
} _libdnml_case;

typedef struct {
    const char *suite_name;
    void *fn_test;
    void *fn_ref;
    _dnml_call_style call_style;
    const char *log_path;

    _libdnml_case *edge_cases;
    uint32_t edge_cases_count;
    uint32_t edge_cases_correct;
    __itomic edge_done;

    _dnml_pair *fail_edge_res;
    _dnml_pair *fail_edge_exp;

    _libdnml_case *rand_cases;
    uint32_t rand_cases_count;
    uint32_t rand_cases_correct;
    __itomic rand_done;

    uint8_t rand_nin;
    uint64_t **fail_rand_in;
    _dnml_pair *fail_rand_res;
    _dnml_pair *fail_rand_exp;
} _libdnml_suite;
typedef struct {
    _libdnml_suite  *suite;
    uint32_t        delay;
} _dnml_thread_arg;
typedef struct {
    _libdnml_suite *suites;
    uint8_t         count;
    _Atomic int    *all_done;
    int             box_width;
} _dnml_progress_arg;

typedef struct {
    const char *session_name;
    uint8_t suite_count;
    _libdnml_suite *suites;

    uint32_t cli_delay; // in ms
    int box_width;
    _dnml_output_mode output_mode;
    _dnml_test_mode   test_mode;
    uint8_t           thread_count;
} _libdnml_session;



//* =================== TEST CREATION FUNFCTIONS =================== *//
static inline void create_suite(
    _libdnml_suite *curr_suite, 
    const char *suite_name,
    uint8_t edge_count, uint8_t rand_count,
    uint8_t rand_nin, uint64_t **fail_randin, _dnml_pair *res_storage, 
    _libdnml_case *edge_bank, _libdnml_case *rand_bank,
    const char *log_path
) {
    curr_suite->suite_name = suite_name;
    curr_suite->edge_cases_count = edge_count;
    curr_suite->rand_cases_count = rand_count;
    curr_suite->rand_nin = rand_nin;
    curr_suite->log_path = log_path;
    // Filling in the banks
    curr_suite->edge_cases = edge_bank;
    curr_suite->rand_cases = rand_bank;
    // Result storage requires at least 2ecount + 4rcount
    curr_suite->fail_edge_res = &res_storage[0];
    curr_suite->fail_edge_exp = &res_storage[edge_count];
    curr_suite->fail_rand_in  = fail_randin;
    curr_suite->fail_rand_exp = &res_storage[2 * edge_count + rand_count];
    curr_suite->fail_rand_res = &res_storage[2 * edge_count + 2 * rand_count];
}
static inline void create_session(
    _libdnml_session *curr_session,
    const char *session_name, uint8_t cli_delay,
    uint8_t suite_count, _libdnml_suite *suite_list,
    _dnml_output_mode output_mode
) {
    curr_session->session_name = session_name;
    curr_session->cli_delay = cli_delay;
    curr_session->suite_count = suite_count;
    curr_session->suites = suite_list;
    curr_session->output_mode = output_mode;
    curr_session->box_width = _dnml_box_width();
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
#define BOX_WIDTH   80
//* ============== SUITE BOX FUNCTIONS ============== *//
static inline void _dnml_box_divider(int bw) {
    printf(BOX_DIV_L);
    for (int i = 0; i < bw; i++) printf(BOX_H);
    printf(BOX_DIV_R "\n");
}
static inline void _dnml_box_top(const char* suite_name, int bw) {
    printf(BOX_TL " %s ", suite_name); 
    size_t namelen = strlen(suite_name);
    for (int i = 0; i < bw - (int)namelen - 2; i++) printf(BOX_H);
    printf(BOX_TR "\n");
}
static inline void _dnml_box_bottom(int bw) {
    printf(BOX_BL);
    for (int i = 0; i < bw; i++) printf(BOX_H);
    printf(BOX_BR "\n");
}
static inline void _dnml_box_line(const char *text, int bw) {
    int len = (int)strlen(text);
    int pad = bw - len;
    if (pad < 0) pad = 0;
    printf(BOX_V " %.*s%*s" BOX_V "\n", bw - 1, text, pad - 1, "");
}
//* ============== SESSION PROGRESS/FEATURES FUNCTIONS ============== *//
static inline int _dnml_itosn(uint32_t x, char *buf, int buflen) {
    if (!buflen) return 0;
    int i = buflen - 1, xlen = 0;
    while (x) { if (i < 0) { break; }
        buf[i] = '0' + char(x % 10);
        x /= 10; --i;
    } return xlen;
}
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
    int filled = (total) ? (uint8_t)(done * barw / total) : 0;

    printf("  %s\n  Session progression: [", session_name);
    for (int i = 0; i < barw; i++)
        printf((i < filled) ? "#" : " ");
    printf("] %" PRIu8 "%\n\n", (total) ? (uint8_t)(done * 100 / total) : 0);
    fflush(stdout);
}
static inline int _dnml_twidth(void) {
    #if defined(_WIN32) || defined(_WIN64)
        #include <windows.h>
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            return (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        } return 80;
    #elif defined(__unix__) || defined(__APPLE__)
        #include <sys/ioctl.h>
        #include <unistd.h>
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 
        && w.ws_col > 0) return (int)w.ws_col;
        return 80;
    #else
        return 80;
    #endif
}
static inline int _dnml_box_width(void) {
    int tw = _dnml_twidth();
    if (tw < 60) tw = 60;
    if (tw > 120) tw = 120;
    return tw - 4;
}



//* =================== TEST CREATION FUNFCTIONS =================== *//
static inline int _comp_pair(const _dnml_pair a, const _dnml_pair b) {
    return (a.first == b.first && a.second == b.second);
}
typedef uint64_t (*_fn1_t)(uint64_t);
typedef uint64_t (*_fn2_t)(uint64_t, uint64_t);
typedef uint64_t (*_fn3_t)(uint64_t, uint64_t, uint64_t);
typedef uint64_t (*_fn4_t)(uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t (*_fn1o_t)(uint64_t, uint64_t*);
typedef uint64_t (*_fn2o_t)(uint64_t, uint64_t, uint64_t*);
typedef uint64_t (*_fn3o_t)(uint64_t, uint64_t, uint64_t, uint64_t*);
#define DNML_FCALL_(fn_ptr, case_ptr, out) \
    do { \
        switch ((case_ptr)->input_count) { \
            case 1: (out).first = ((_fn1_t)(fn_ptr))((case_ptr)->in[0]); break;    \
            case 2: (out).first = ((_fn2_t)(fn_ptr))(  \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1]                   \
            ); break; \
            case 3: (out).first = ((_fn3_t)(fn_ptr))(  \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1],                  \
                (case_ptr)->in[2]                   \
            ); break; \
            case 4: (out).first = ((_fn4_t)(fn_ptr))(  \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1],                  \
                (case_ptr)->in[2],                  \
                (case_ptr)->in[3]                   \
            ); break; \
        } \
    } while(0)
#define DNML_OFCALL_(fn_ptr, case_ptr, out) \
    do { \
        switch ((case_ptr)->input_count) { \
            case 1: (out).first = ((_fn1o_t)(fn_ptr))((case_ptr)->in[0], &(out).second); break; \
            case 2: (out).first = ((_fn2o_t)(fn_ptr))( \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1],                  \
               &(out).second                        \
            ); break; \
            case 3: (out).first = ((_fn3o_t)(fn_ptr))( \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1],                  \
                (case_ptr)->in[2],                  \
               &(out).second                        \
            ); break; \
        } \
    } while(0)



//* ============== FULL SUITES/SESSIONS RENDER FUNCTIONS ============== *//
static inline void _dnml_run_suite(_libdnml_suite *s) {
    //* ======== 1. EDGE CASE TESTING ======== *//
    for (uint32_t i = 0; i < s->edge_cases_count; ++i) {
        _dnml_pair got = {0}, exp = s->edge_cases->exp;
        if (s->call_style == DNML_CALL) DNML_FCALL_(s->fn_test, &s->edge_cases[i], got);
        else DNML_OFCALL_(s->fn_test,&s->edge_cases[i], got);
        if (_comp_pair(got, exp)) s->edge_cases_correct += 1;
        else {
            uint32_t findex = (i + 1) - s->edge_cases_correct;
            s->fail_edge_res[findex] = got;
            s->fail_edge_exp[findex] = exp;
        } itomic_increment(&s->edge_done);
    }

    //* ======== 2. RAND CASE TESTING ======== *//
    for (uint32_t i = 0; i < s->rand_cases_count; ++i) {
        _dnml_pair got = {0}, exp = {0};
        if (s->call_style == DNML_CALL) {
            DNML_FCALL_(s->fn_test, &s->rand_cases[i], got);
            DNML_FCALL_(s->fn_ref,  &s->rand_cases[i], exp);
        } else {
            DNML_OFCALL_(s->fn_test, &s->rand_cases[i], got);
            DNML_OFCALL_(s->fn_ref,  &s->rand_cases[i], exp);
        } if (_comp_pair(got, exp)) s->rand_cases_correct += 1;
        else {
            uint32_t findex = (i + 1) - s->rand_cases_correct;
            s->fail_rand_res[findex] = got;
            s->fail_rand_exp[findex] = exp;
        } itomic_increment(&s->rand_done);
    }
}
static inline void _dnml_log_suite(_libdnml_suite *s) {
    uint32_t fail_edge = s->edge_cases_count - s->edge_cases_correct;
    uint32_t fail_rand = s->rand_cases_count - s->rand_cases_correct;

    if ((fail_edge + fail_rand) == 0 || !s->log_path) return;
    FILE *f = fopen(s->log_path, "w"); if (!f) return;
    fprintf(f, "======== %s FAIL LOG ========", s->suite_name);
    //* PRINTS EDGE CASES *//
    for (uint32_t i = 0; i < fail_edge; ++i) {
        fprintf(f, "o) Edge case %" PRIu32 ":\n", i + 1);
        for (uint8_t j = 0; j < s->edge_cases[i].input_count; ++j) {
            fprintf(f, "     in[%" PRIu8 "]: 0x%016" PRIx64 "\n", j, s->edge_cases[i].in[j]);
        } fprintf(f, "     expected: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_edge_exp[i].first, s->fail_edge_exp[i].second);
        fprintf(f, "     got: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_edge_res[i].first, s->fail_edge_res[i].second);
    }

    //* PRINTS RANDOM CASES *//
    for (uint32_t i = 0; i < fail_rand; ++i) {
        fprintf(f, "o) Rand case %" PRIu32 ":\n", i + 1);
        for (uint8_t j = 0; j < s->rand_nin; ++j) {
            fprintf(f, "     in[%" PRIu8 "]: 0x%016" PRIx64 "\n", j, s->fail_rand_in[i][j]);
        } fprintf(f, "     expected: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_rand_exp[i].first, s->fail_rand_exp[i].second);
        fprintf(f, "     got: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_rand_res[i].first, s->fail_rand_res[i].second);
    } fclose(f);
}
static inline void _dnml_run_sthread(void *arg) {
    _dnml_thread_arg *a = (_dnml_thread_arg*)arg;
    _dnml_run_suite(a->suite);
    _dnml_log_suite(a->suite);
}
static void *_dnml_progress_thread(void *arg) {
    _dnml_progress_arg *a = (_dnml_progress_arg*)arg;
    const char *spin[] = { "|", "/", "-", "\\" };
    uint32_t tick = 0;
    while (!itomic_load(a->all_done)) {
        // move cursor up N lines to redraw
        printf("\033[%dA", a->count + 1);

        for (uint8_t i = 0; i < a->count; i++) {
            _libdnml_suite *s = &a->suites[i];
            uint32_t total = s->edge_cases_count + s->rand_cases_count;
            uint32_t done  = itomic_load(&s->edge_done)
                           + itomic_load(&s->rand_done);
            int bw    = a->box_width - 20;
            int filled = total ? (done * bw / total) : 0;

            printf("  %s %-16s [", spin[tick % 4], s->suite_name);
            for (int j = 0; j < bw; j++)
                printf(j < filled ? "#" : " ");
            printf("] %5" PRIu32 "/%-5" PRIu32 "\n", done, total);
        } fflush(stdout); tick++; _dnml_sleep(100); // 100ms refresh
    } return NULL;
}
static inline void _dnml_render_csuite(_libdnml_suite *s) { // Render a "COMPACT" Suite
    uint8_t fail_edge = s->edge_cases_count - s->edge_cases_correct;
    uint8_t fail_rand = s->rand_cases_count - s->rand_cases_correct;
    char status = (fail_edge + fail_rand == 0) ? '+' : '-';
    printf("  [%c] %-20s     %2" PRIu8 "/%-2" PRIu8 " edge   %2" PRIu8 "/%-2" PRIu8 " random",
        status, s->suite_name,
        s->edge_cases_correct, s->edge_cases_count,
        s->rand_cases_correct, s->rand_cases_count
    ); if (fail_edge + fail_rand > 0) printf("  -> %s_fails.log", s->suite_name);
    putchar('\n'); fflush(stdout);
}
static inline void _dnml_render_esuite( // Render the "edge half" of a verbose suite
    _libdnml_suite *s,
    uint8_t suite_num,
    uint32_t delay_ms,
    int session_bw
) {
    // ------ edge cases line ------
    char edge_line[BOX_WIDTH];
    snprintf(
        edge_line, sizeof(edge_line), "Edge case: %d/%d",
        s->edge_cases_correct, s->edge_cases_count
    ); _dnml_box_line(edge_line, session_bw);
    _dnml_delay_ms(delay_ms);

    // print failed edge cases
    int fail_edge = s->edge_cases_count - s->edge_cases_correct;
    char curr_index[10], fail_line[BOX_WIDTH];
    for (int i = 0; i < fail_edge; ++i) { fail_line[BOX_WIDTH];
        int curri_len = (i, curr_index, sizeof(curr_index));
        snprintf( fail_line, sizeof(fail_line),
            "o) Case %.*s: Expected: <0x%016" PRIx64 ", 0x%016" PRIx64 "> | Got: <0x%016" PRIx64 ", 0x%016 " PRIx64 ">",
            curri_len, curr_index,
            s->fail_edge_exp[i].first,
            s->fail_edge_exp[i].second,
            s->fail_edge_res[i].first,
            s->fail_edge_res[i].second
        ); _dnml_box_line(fail_line, session_bw);
        _dnml_delay_ms(delay_ms);
    } fflush(stdout);
}
static inline void _dnml_render_rsuite( // Render the "rand half" of a verbose suite
    _libdnml_suite *s,
    uint8_t suite_num,
    uint32_t delay_ms,
    int session_bw
) {
    // ------ random cases line ------
    char rand_line[BOX_WIDTH];
    snprintf(rand_line, sizeof(rand_line), "Random case: %d/%d",
             s->rand_cases_correct, s->rand_cases_count);
    _dnml_box_line(rand_line, session_bw);
    _dnml_delay_ms(delay_ms);

    // print failed random cases
    int fail_rand = s->rand_cases_count - s->rand_cases_correct;
    char curr_index[10], fail_line[BOX_WIDTH];
    for (int i = 0; i < fail_rand; i++) {
        int ilen = _dnml_itosn(i, curr_index, sizeof(curr_index));
        snprintf(
            fail_line, sizeof(fail_line),
            "o Case %.*s: Expected: <0x%016" PRIx64 ", 0x%016" PRIx64 "> | Got: <0x%016" PRIx64 ", 0x%016" PRIx64 ">",
            ilen, curr_index,
            s->fail_rand_exp[i].first,
            s->fail_rand_exp[i].second,
            s->fail_rand_res[i].first,
            s->fail_rand_res[i].second
        ); _dnml_box_line(fail_line, session_bw);
        _dnml_delay_ms(delay_ms);
    } _dnml_box_bottom(session_bw);
    putchar('\n');
    fflush(stdout);
}
static inline void start_session(const _libdnml_session *session) {
    int bw = session->box_width;
    //* ---- COMPACT MODE ---- *//
    if (session->output_mode == DNML_COUT) {
        printf("\n  -- %s ", session->session_name);
        int pad = 44 - (int)(strlen)(session->session_name);
        for (int i = 0; i < (pad - (pad & 1)) >> 1; ++i) fputs("--", stdout);
        putchar('\n');
        for (uint8_t i = 0; i < session->suite_count; ++i) {
            _dnml_run_suite(&session->suites[i]);
            _dnml_render_csuite(&session->suites[i]);
        } return;
    } 
    //* ---- VERBSOE/FULL MODE ---- *//
    // loading animation before session starts
    _dnml_loading("Loading session...", session->cli_delay, 12);


    for (uint8_t i = 0; i < session->suite_count; ++i) {
        // update progress bar before each suite
        _dnml_session_progress(i, session->suite_count, session->session_name);
        // loading animation between suites
        if (i > 0) _dnml_loading("Running suite...", session->cli_delay, 8);
        _dnml_run_suite(&session->suites[i]);
        _dnml_log_suite(&session->suites[i]);
        // Draw the top of the "suite's box"
        _dnml_box_top(session->suites[i].suite_name, bw);
        _dnml_box_divider(bw);
        _dnml_delay_ms(session->cli_delay);
        // Render the "edge case part" of suite[i]
        _dnml_render_esuite(&session->suites[i], i + 1, session->cli_delay, bw);
        // Line break into "random cases part" of suite[i]
        _dnml_box_divider(bw);
        _dnml_render_rsuite(&session->suites[i], i + 1, session->cli_delay, bw);
    }
    // final progress bar at 100%
    _dnml_session_progress(session->suite_count, session->suite_count, session->session_name);
}




#endif