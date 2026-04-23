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
#include "../sconfigs/memory/_scratch.h"
#include "../util/util.h"
#include "_test_base.h"


//* =========== TYPE DEFINITIONS =========== *//
// Small, supporting types
typedef enum res_type { BIGINT, STRING } operated_types;
typedef enum { INVERSE, EVAL, PROPRETY, NONE } rcheck_mode;
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
    union { bigInt bi; size_t len; } data;
    size_t cap;
    char str[];
} str_res;

// Cases & Suites
typedef struct _libdnml_scase {
    rcheck_mode mode;
    void* in;
    str_res exp;
    str_res res;
    void* recons;
} _libdnml_scase;

//* =========== TYPE-SPEECIFIC UTILITIES =========== *//
#define STR_PREVIEW 64
#define BIGINT_PREVIEW 4

static inline bool _comp_str_res(const str_res *a, const str_res *b) {
    if (a->status != b->status) return false;
    if (a->status != BIGINT_SUCCESS || a->status != STR_SUCCESS) return true;
    if (a->type != b->type) return false;
    switch (a->type) {
        case STRING: return strcmp(a->str, b->str) == 0; break;
        case BIGINT: return (__BIGINT_INTERNAL_COMP__(&a->data.bi, &b->data.bi) == 0); break;
        default: return true; break;
    };
}
static inline void _print_str(FILE *f, const char *s, size_t len, int tab_depth) {
    /* Example:
        - Input: <
            +) Input 1 - <char*>: "123456789abcdef...123456789abcdef" (Length = 2048)
            +) Random Text ....
            +) Random Text ....
        >
    */
    if (len <= STR_PREVIEW) {
        fprintf(f, "< \"%.*s\">" , len, s);
    } else {
        fputs("<\n", f); char buf[STR_PREVIEW];
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        memcpy(buf, s, STR_PREVIEW); 
        fprintf(f, "--- Low Segment:  \"%.*s\"...\n", STR_PREVIEW, buf);
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        memcpy(buf, s[len - STR_PREVIEW - 1], STR_PREVIEW);
        fprintf(f, "--- High Segment: \"%.*s\"...\n", STR_PREVIEW, buf);
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fprintf(f, "--- Length: %zu\n", len);
        for (int i = 0; i < tab_depth; ++i) fputs(TAB, f);
        fputc('>', f);
    }
}
static inline void _print_bigint(FILE *f, const bigInt *x, int tab_depth) {
    /* Example:
        +) .... I love femboy text ...: <
            --- Limb Count: 1024 (limbs)
            --- Sign: 1 (Positive +)
            --- Low Limbs: [12345678901234567890, ...]
            --- High Limbs: [..., 12345678901234567890]
        >
    */
    fputs("<\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "--- Limb Count: %zu\n", x->n);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "--- Sign: %" PRIu8 " (%s %c)\n", x->sign, 
        (x->sign == 1) ? "Positive" : "Negative", 
        (x->sign == 1) ? '+' : '-'
    );
    if (x->n <= BIGINT_PREVIEW) {
        // Full Limbs
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fputs("--- Limbs: [", f); 
        for (size_t i = 0; i < x->n; ++i) {
            fprintf(f, "%016" PRIx64 ", ", x->limbs[i]);
        } fputs("]\n", f);
    } else {
        // Low Limbs
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fputs("--- Low Limbs: [", f); 
        for (size_t i = 0; i < BIGINT_PREVIEW/2; ++i) {
            fprintf(f, "%016" PRIx64 " ", x->limbs[i]);
        } fputs("]\n", f);

        // High Limbs
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fputs("--- High Limbs: [", f);
        for (size_t i = x->n - BIGINT_PREVIEW/2; i < x->n; ++i) {
            fprintf(f, "%016" PRIx64 " ", x->limbs[i]);
        } fputs(']', f);
    }
    // The final close-bracket ">"
    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f);
    fputc('>', f);
}
static inline void _print_str_res(const str_res *a, FILE *f, int tab_depth) {
    fputs("< -- STR-RES --\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Status: ", f); _print_dnml_status(a->status, f); fputc('\n', f);
    fputs("+) Data:   ", f); switch (a->type) {
        case BIGINT: _print_bigint(f, &a->data.bi, tab_depth); break;
        case STRING: _print_str(f, a->str, a->data.len, tab_depth); break;
    } for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}


//* =================== FUNCTION-GENERALIZATION DISPATCHER =================== *//
typedef void (*dnml_exec_fn)(const void *in, str_res *out, void *ctx);
typedef bool (*dnml_prop_fn)(const void *in, str_res *out);
// Evaluators & Inverses
typedef void (*dnml_eval_fn)(const void *in, str_res *exp, void *ctx);
typedef void (*dnml_inv_fn)(const void *in, const str_res out, void *reconstructed, void *ctx);
// Comparisons
typedef bool (*dnml_stat_fn)(const void *in, dnml_status res_stat);
typedef bool (*dnml_cmp_inv_fn)(const void *original, const str_res *out, const void *recon, void *ctx);
typedef bool (*dnml_cmp_eval_fn)(const str_res *full, const str_res *out);
// Printing & Formatting
typedef void (*dnml_fmt_in_fn)(FILE *f, const void *in, int tab_depth);
typedef void (*dnml_fmt_recon_fn)(FILE *f, const void* recon, int tab_depth);

static str_res *alloc_res(dnml_dratch *a, size_t len) {
    str_res *r = (str_res*)(dratch_alloc(a, sizeof(str_res) + len + 1));
    r->type = STRING;
    r->data.len = len;
    return r;
}
static inline void *run_case(_libdnml_scase *c, dnml_exec_fn *fn, void *ctx) {
    (*fn)(c->in, &c->res, ctx);
}
static inline void *run_eval(_libdnml_scase *c, dnml_eval_fn *fn, void *ctx) {
    (*fn)(c->in, &c->exp, ctx);
}
static inline void *run_inverse(_libdnml_scase *c, dnml_inv_fn *fn, void *ctx) {
    (*fn)(c->in, c->res, &c->recons, ctx);
}



//* =================== TEST CREATION FUNFCTIONS =================== *//
typedef struct _libdnml_str_suite {
    const char *suite_name;
    void *ctx;
    const char *log_path;
    rcheck_mode func_mode;

    dnml_exec_fn *fn_test; 
    // Random-based Oracle Functions
    dnml_inv_fn *fn_inv;                dnml_eval_fn *fn_eval;          dnml_stat_fn *fn_stat;
    dnml_cmp_inv_fn *inv_cmp;           dnml_cmp_eval_fn *eval_cmp;     
    dnml_fmt_in_fn *fmtin_fn;   
    dnml_fmt_recon_fn *fmtrecon_fn; 
    // Property-based Functions
    dnml_prop_fn *fn_prop;

    // Edge cases storage
    _libdnml_scase *edge;
    uint8_t ecount;     uint8_t ecorrect;
    str_res *fail_eres; str_res *fail_eexp;

    // Random cases storage
    _libdnml_scase *rand;
    uint16_t rcount;       uint16_t rcorrect;
    void* *fail_rin;       str_res *fail_rres; void* *fail_rrecons;

    int fail_enums[];
} _libdnml_str_suite;


static inline void create_str_suite(
    _libdnml_str_suite *curr_suite, const char *name,
    uint8_t ecount, uint16_t rcorrect,
    _libdnml_scase *ebank, _libdnml_scase *rbank,
    rcheck_mode mode,
    void** *fail_inbuf, str_res *fail_resbuf,
    void* *fail_rrecons, const char *log_path
) {
    curr_suite->suite_name = name;
    curr_suite->ecount = ecount;
    curr_suite->rcorrect = rcorrect;
    curr_suite->log_path = log_path;
    // Filling in the banks
    curr_suite->edge = ebank;
    curr_suite->rand = rbank;
    // Assigning edge-case failure storage
    curr_suite->fail_eres = fail_resbuf;
    curr_suite->fail_eexp = &fail_resbuf[ecount];
    // Assigning random-case failure storage
    curr_suite->fail_rin  = fail_inbuf;
    curr_suite->fail_rres = &fail_resbuf[2 * ecount];
    curr_suite->fail_rrecons = fail_rrecons;
}

static inline void fill_suite_prop(_libdnml_str_suite *curr_suite, bool *prop_fn) {
    curr_suite->fn_prop = prop_fn;
}
static inline void fill_suite_rand(
    _libdnml_str_suite *curr_suite, void *fn_test,
    void *fn_inv,       void *fn_eval,      bool *fn_stat,
    bool *cmp_inv,      bool *cmp_eval,   
    void *fmtin_fn, 
    void *fmtrecon_fn
) {
    curr_suite->fn_test = (dnml_exec_fn*)(fn_test);
    // Evaluators
    curr_suite->fn_inv = (dnml_inv_fn*)(fn_inv);
    curr_suite->fn_eval = (dnml_eval_fn*)(fn_eval);
    // Comparisons
    curr_suite->fn_stat = (dnml_stat_fn*)(fn_stat);
    curr_suite->inv_cmp = (dnml_cmp_inv_fn*)(cmp_inv);
    curr_suite->eval_cmp = (dnml_cmp_eval_fn*)(cmp_eval);
    // Priting & Formatting
    curr_suite->fmtin_fn = (dnml_fmt_in_fn*)(fmtin_fn);
    curr_suite->fmtrecon_fn = (dnml_fmt_recon_fn*)(fmtrecon_fn);
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



//* ================= SUITE RUNNER FUNCTIONS ================= *//
static inline void _dnml_run_edge(_libdnml_str_suite *s) {
    int enum_i = 0;
    for (uint8_t i = 0; i < s->ecount; ++i) {
        _libdnml_scase *c = &s->edge[i];
        run_case(c, s->fn_test, s->ctx);
        if (_comp_str_res(&c->res, &c->exp)) s->ecorrect++;
        else { uint8_t findex = (i + 1) - s->ecorrect;
            s->fail_eres[findex] = c->res;
            s->fail_eexp[findex] = c->exp;
            s->fail_enums[enum_i] = i + 1; ++enum_i;
        }
    }
}
static inline void _dnml_run_rand(_libdnml_str_suite *s) {
    for (uint16_t i = 0; i < s->rcorrect; ++i) {
        _libdnml_scase *c = &s->rand[i]; str_res ref;
        run_case(c, s->fn_test, s->ctx); uint8_t correct = 0;
        // HARD GATE - DO NOT ALLOW ERROR CASES TO GO TO EVALUATION
        if (c->res.status != STR_SUCCESS || c->res.status != BIGINT_SUCCESS) {
            if ((*s->fn_stat)(c->in, c->res.status)) correct = 1;
            return;
        } 
        // ACTUAL EVALUATION WORK
        if (c->mode = INVERSE && s->fn_inv) {
            run_inverse(c, s->fn_inv, s->ctx);
            if ((*s->inv_cmp)(c->in, &c->res, c->recons, s->ctx)) correct = 1;
        } else {
            run_eval(c, s->fn_eval, s->ctx);
            if ((*s->eval_cmp)(&c->exp, &c->res)) correct = 1;
        }
        if (correct) s->rcorrect++;
        else { uint16_t findex = (i + 1) - s->rcorrect;
            s->fail_rin[findex] = c->in;
            s->fail_rres[findex] = c->res;
            s->fail_rrecons[findex] = c->recons;
        }
    }
}
static inline void _dnml_run_prop(_libdnml_str_suite *s) {}
static inline void _dnml_run_suite(_libdnml_str_suite *s) {
    _dnml_run_edge(s); if (s->ecorrect == s->ecount) return;
    if (s->func_mode == INVERSE || s->func_mode == EVAL) _dnml_run_rand(s);
    else if (s->func_mode == PROPRETY) _dnml_run_prop(s);
    else;
}
//* ========= FULL SUITES/SESSIONS RENDER FUNCTIONS ========== *//
static inline void _dnml_log_suite(_libdnml_str_suite *s) {
    uint16_t fail_edge = s->ecount - s->ecorrect;
    uint16_t fail_rand = s->rcorrect - s->rcorrect;

    if ((fail_edge + fail_rand) == 0 || !s->log_path) return;
    FILE *f = fopen(s->log_path, "w"); if (!f) return;
    fprintf(f, "======== %s FAIL LOG ========", s->suite_name);
    //* PRINTS EDGE CASES *//
    for (uint8_t i = 0; i < fail_edge; ++i) {
        /* Example Result:
            o) Edge case 32:
               - Input: <"../HelloWord.bin", ...> (Whatever the format is)
               - Expected: < -- STR-RES --
                    +) Status: STRING_NULL
                    +) Data: "\0";
                >
               - Got:      < -- STR-RES --
                    +) Status: STR_SUCCESS
                    +) Data: "123456789abcdef...123456789abcdef" (Length = 1024)
                >
        */
        fprintf(f,  "o) Edge case %d:\n", s->fail_enums[i]);
        fputs(      "    - Input: ", f);
        if (s->fmtin_fn) (*s->fmtin_fn)(f, s->fail_rin[i], 1);
        else fputs("<ERROR: NO-FORMATTER>", f);
        fputc('\n', f);
        fputs(      "    - Expected: ", f);
        _print_str_res(&s->fail_eexp[i], f, 1); fputc('\n', f);
        fputs(      "    - Got:      ", f);
        _print_str_res(&s->fail_eres[i], f, 1); fputc('\n', f);
    }

    //* PRINTS RANDOM CASES *//
    for (uint16_t i = 0; i < fail_rand; ++i) {
        /* Example Result:
            o) Edge Rand 32:
               - Input: <...> (Whatever the format is)
               - Output: < -- STR-RES --
                    +) Status: STRING_SUCCESS
                    +) Data: "123456789...123456789" (Length = 256)
                >
               - Reconstructed: < -- STR-RES --
                    +) Status: BIGINT_SUCCESS
                    +) Data: < Sign: 1 | Limb Count: 32 | low=[..., ...] high=[..., ...] >
                >
        */
        fprintf(f, "o) Rand case %" PRIu16 ":\n", i + 1);
        fputs(      "    - Input: ", f);
        if (s->fmtin_fn) (*s->fmtin_fn)(f, s->fail_rin[i], 1);
        else fputs("<ERROR: NO-FORMATTER>", f);
        fputc('\n', f);
        fputs(      "    - Output: ", f);
        _print_str_res(&s->fail_rres[i], f, 1); fputc('\n', f);
        fputs(      "    - Reconstructed: ", f);
        void *idk[1] = { s->fail_rrecons[i] };
        if (s->fmtrecon_fn) (*s->fmtrecon_fn)(f, idk, 1);
        else fputs("<ERROR: NO-FORMATTER>", f);
    } fclose(f);
}
static inline void _dnml_render_csuite(_libdnml_str_suite *s) { // Render a "COMPACT" Suite
    uint8_t fail_edge = s->ecount - s->ecorrect;
    uint8_t fail_rand = s->rcount - s->rcorrect;
    char status = (fail_edge + fail_rand == 0) ? '+' : '-';
    printf("  [%c] %-20s     %2" PRIu8 "/%-2" PRIu8 " edge   %2" PRIu8 "/%-2" PRIu8 " random",
        status, s->suite_name,
        s->ecorrect, s->ecount,
        s->rcorrect, s->rcount
    ); if (fail_edge + fail_rand > 0) printf("  -> %s_fails.log", s->suite_name);
    putchar('\n'); fflush(stdout);
}
static inline void _dnml_render_esuite(_libdnml_str_suite *s, uint8_t suite_num, uint32_t delay_ms, int bw) {
    _dnml_box_top(s->suite_name, bw); _dnml_box_divider(bw); _dnml_delay_ms(delay_ms);
    // ------ edge cases line ------
    char edge_line[bw]; snprintf(
        edge_line, sizeof(edge_line), "Edge case: %d/%d",
        s->ecorrect, s->ecount
    ); _dnml_box_line(edge_line, bw);
    _dnml_delay_ms(delay_ms);

    // print failed edge cases
    /* Example:
            o) Case 5:
                - Expected: < -- STR_RES -- 
                    +) Status: ...
                    +) Data: ...
                >
                - Got: < -- STR_RES --
                    +) Status ...
                    +) Data: ...
                >
    */
    int fail_edge = s->ecount - s->ecorrect;
    char curr_index[10], fail_line[bw]; FILE *tmp = tmpfile(); 
    if (tmp == NULL) { perror("Failed to open a tmpfile(), Terminating..."); abort(); }
    for (int i = 0; i < fail_edge; ++i) { 
        memset(fail_line, (char)(1), bw);
        int curri_len = (i, curr_index, sizeof(curr_index));
        snprintf(fail_line, sizeof(fail_line), "Case %" PRIu16 ": \n", s->fail_enums[i]);
        _dnml_box_line(fail_line, bw);

        freopen(NULL, "w", tmp);
        fputs("    - Expected: ", tmp);
        _print_str_res(&s->fail_eexp[i], tmp, 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        freopen(NULL, "w", tmp);
        fputs("    - Got: ", tmp);
        _print_str_res(&s->fail_eres[i], tmp, 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        _dnml_delay_ms(delay_ms);
    } fflush(stdout);
}
static inline void _dnml_render_rsuite(_libdnml_str_suite *s, uint8_t suite_num, uint32_t delay_ms, int bw) {
    _dnml_box_divider(bw);
    // ------ random cases line ------
    char rand_line[bw];
    snprintf(rand_line, sizeof(rand_line), "Random case: %d/%d",
             s->rcorrect, s->rcount);
    _dnml_box_line(rand_line, bw);
    _dnml_delay_ms(delay_ms);

    // print failed random cases
    /* Example:
            o) Case 5:
                - Input: <...> (whatever the top-layer format is)
                - Output: < -- STR_RES --
                    +) Status ...
                    +) Data: ...
                >
                - Reconstruction: <...> (whatever the top-layer format is)
    */
    int fail_rand = s->rcount - s->rcorrect;
    char curr_index[10], fail_line[bw]; FILE *tmp = tmpfile(); 
    if (tmp == NULL) { perror("Failed to open a tmpfile(), Terminating..."); abort(); }
    for (int i = 0; i < fail_rand; i++) {
        memset(fail_line, (char)(1), bw); // Resetting buffer to ASCII 1
        int ilen = _itosn(i, curr_index, sizeof(curr_index));
        snprintf(fail_line, sizeof(fail_line), "Case %d: \n", i + 1);
        _dnml_box_line(fail_line, bw);

        freopen(NULL, "w", tmp);
        fputs("    - Input: ", tmp);
        (*s->fmtin_fn)(tmp, s->fail_rin[i], 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        freopen(NULL, "w", tmp);
        fputs("    - Output: ", tmp);
        _print_str_res(&s->fail_rres[i], tmp, 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        freopen(NULL, "w", tmp);
        fputs("    - Reconstruction: ", tmp);
        (*s->fmtrecon_fn)(tmp, s->fail_rrecons[i], 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        _dnml_delay_ms(delay_ms);
    } _dnml_box_bottom(bw);
    putchar('\n');
    fflush(stdout);
}
static inline void start_session(const _libdnml_session *session) {
    int bw = session->box_width;
    _libdnml_str_suite *session_suites = (_libdnml_str_suite*)(session->suites);
    //* ---- COMPACT MODE ---- *//
    if (session->output_mode >= DNML_COUT) {
        printf("\n  -- %s ", session->session_name);
        int pad = 44 - (int)(strlen)(session->session_name);
        for (int i = 0; i < (pad - (pad & 1)) >> 1; ++i) fputs("--", stdout);
        putchar('\n');
        for (uint8_t i = 0; i < session->suite_count; ++i) {
            _dnml_run_suite(&session_suites[i]);
            _dnml_render_csuite(&session_suites[i]);
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
        _dnml_run_suite(&session_suites[i]);
        _dnml_log_suite(&session_suites[i]);
        // Render the "edge case" & "rand case" part of suite[i]
        _dnml_render_esuite(&session_suites[i], i + 1, session->cli_delay, bw);
        _dnml_render_rsuite(&session_suites[i], i + 1, session->cli_delay, bw);
    }
    // final progress bar at 100%
    _dnml_session_progress(session->suite_count, session->suite_count, session->session_name);
}

#endif