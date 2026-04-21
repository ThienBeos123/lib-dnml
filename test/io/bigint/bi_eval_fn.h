#ifndef _BIGINT_INV_FUNC_H
#define _BIGINT_INV_FUNC_H


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "../../../sconfigs/memory/_scratch.h"
#include "../../../test_ui/_strui.h"
#include "../../../adynamol/big_numbers/bigNums.h"
#include "../../../adynamol/big_numbers/bigInt_func.h"
#include "bi_indef.h"


/* Note 1 - Inverses:
*   +) A rule of thumb for inverses functions are: 
*      - Function with deterministic and self-contained base, which that includes 
*        purely base-10 or with base-prefix, goes also with inverses function that
*        accepts and scans for self-contained base
*
*      - Eg 01: bigInt_to_strf() outputs a string with a base-prefix, meaning its bases are
*               deterministic and self-contained. Therefore, it inverses are bigInt_get_strn(),
*               where it scans for base-prefixes
*
*      - Eg 02: bigInt_to_str() outputs a purely base-10 decimal with no base-prefix, meaning its bases
*               are deterministic. Therefore, bigInt_get_strn() is also its inverse due to the function's
*               interpreting the string as base-10 by default if no base-prefix were detected
*
*      - Eg 03: bigInt_to_strb() outputs an arbitrary base string with no base-prefix, meaning its bases
*               are indeterministic, and therefore needs further clarifying from user's input. Therefore,
*               bigInt_get_strnb() is its inverses due to it not needing to scan for base-prefix, and
*               outputs a bigInt based on the user's base input

*   +) Lossless functions, functions in which full input information
*      might be lost, will NOT have an inverse, for the input
*      is unrecoverable autonomously, and therefore, 
*      such functions resort to EVALUATION (see Note 2)
*      ------> Truncative Variants do NOT have inverses
*/


/* Note 2 - Evaluation:
*   +) Each evaluation function are semantic/format based
*      ---> Each evaluation function fits one format of output
*   +) Example: tto_str & tto_strn has output of purely base-10 with no base-prefix
*      -------> We use to_strn as the full output evaluator since
*               its output is also purely base-19 with no base-prefix
*/


/* Note 3 - Allocation:
*   1) Most of the evaluation and inverse wrappers in bi_eval_fn.h
*      utilizes scratch allocation seeded from vctx/ctx.
*   -----> Entails tester and main testing units, when seeding
*          temporary scratch spaces for testing, to always:

            1. Mark the initial offset:

                    size_t unit_start = dratch_mark(&ctx_);

            2. Reset towards that initial offset at
               the end of the testing unit:

                    dratch_reset(&ctx_, unit_start);

*   2) Due to there being a lot of functions to test, with a
*      wide variety of function categories, testers may implement
*      either a single universal scratch buffer for every functions,
*      a buffer for each category, or a buffer for each function. The choice
*      depends on the amount of functions being tested

*   3) For every scratch buffers initialized, its setup and cleanup
*      MUST follow step 1 to ensure memory-safe and efficient test runs
*/


//* ========================= BITOS EVALUATION WRAPPERS ======================= *//
// BITOS Conversions Inverses & Evaluators
static inline void inv_bitos_conv_nob(const void *vin, const str_res *out, void *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)(dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T));
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    bigInt_get_strn(&tmp, out->str, out->data.len);
    *(bigInt*)recon = tmp;
}
static inline void inv_bitos_conv_b(const void *vin, const str_res *out, void *recon, void *vctx) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)(dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T));
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    bigInt_get_strnb(&tmp, out->str, out->data.len, in->base);
    *(bigInt*)recon = tmp;
}
static inline void eval_bitos_tto_str(const void *vin, str_res *exp, void *vctx) {
    // For: tto_str, tto_strn
    const bitos_conv_in *in = (bitos_conv_in*)in;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t needed = __BIGINT_COUNTDB__(&in->x, 10);
    exp->status = bigInt_to_strn(exp->str, needed + 1, in->x, &needed);
    exp->data.len = needed;
}
static inline void eval_bitos_tto_strb(const void *vin, str_res *exp, void *vctx) {
    // For: tto_strb, tto_strnb
    const bitos_conv_in *in = (bitos_conv_in*)in;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t needed = __BIGINT_COUNTDB__(&in->x, in->base);
    exp->status = bigInt_to_strnb(
        exp->str, needed + 1, in->x, 
        in->base, &needed
    ); exp->data.len = needed;
}
static inline void eval_bitos_tto_strf(const void *vin, str_res *exp, void *vctx) {
    // For: tto_strf
    const bitos_conv_in *in = (bitos_conv_in*)in;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t needed = __BIGINT_COUNTDB__(&in->x, in->base);
    exp->status = bigInt_to_strf(
        exp->str, needed + 1, 
        in->x, in->base, 
        in->uppercase, &needed
    ); exp->data.len = needed;
}
// BITOS Printing & Raw Output Inverses
static inline void inv_bitos_fput_nob(const void *vin, const str_res *out, void *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)(dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T));
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    bigInt_get_strn(&tmp, out->str, out->data.len);
    *(bigInt*)recon = tmp;
}
static inline void inv_bitos_fput_b(const void *vin, const str_res *out, void *recon, void *vctx) {
    bitos_print_in *in = (bitos_print_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t bcount = __BITCOUNT___(out->data.len, in->base);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)(dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T));
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    bigInt_get_strnb(&tmp, out->str, out->data.len, in->base);
    *(bigInt*)recon = tmp;
}
static inline void inv_bitos_serial(const void *vin, const str_res *out, void *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    size_t lsize = bigInt_deserial_size(out->str, out->data.len);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, lsize * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = lsize, .sign = 0 };
    bigInt_deserialize(&tmp, out->str, out->data.len);
    *(bigInt*)recon = tmp;
}
static inline void eval_bitos_tserialize(const void *vin, str_res *exp, void *vctx) {
    // For: bigInt_tserialize()
    const bitos_serialize_in *in = (bitos_serialize_in*)in;
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    needed = bigInt_serial_size(in->x);
    exp->status = bigInt_serialize(exp->str, needed, in->x, &tmp);
    exp->data.len = needed;
}



//* ========================= STOBI EVALUATION WRAPPERS ======================= *//
// STOBI Initialization Inverses
static inline void inv_stobi_init_nob(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for bigInt_strint & bigInt_strninit
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strn(idk, len, out->data.bi, &tmp);
    *(char**)recon = idk;
}
static inline void inv_stobi_init_b(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for bigInt_strbint & bigInt_strnbinit
    stobi_init_in *in = (stobi_init_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp);
    *(char**)recon = idk;
}
// STOBI Conversion Inverses
static inline void inv_stobi_conv_nob(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for bigInt_from_str & bigInt_from_strn
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strn(idk, len, out->data.bi, &tmp);
    *(char**)recon = idk;
}
static inline void inv_stobi_conv_b(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for bigInt_from_strb & bigInt_from_strnb
    stobi_conv_in *in = (stobi_conv_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp);
    *(char**)recon = idk;
}
// STOBI Assignment Inverses & Evaluators
static inline void inv_stobi_assign_nob(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for:
    //  - GROWS: bigInt_get_str & bigInt_get_strn
    //  - SAFE: bigInt_sget_str & bigInt_sget_strn
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strn(idk, len, out->data.bi, &tmp);
    *(char**)recon = idk;
}
static inline void inv_stobi_assign_b(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for:
    //  - GROWS: bigInt_get_strb & bigInt_get_strnb
    //  - SAFE: bigInt_sget_strb & bigInt_sget_strnb
    stobi_assign_in *in = (stobi_assign_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp);
    *(char**)recon = idk;
}
static inline void eval_stobi_tget_str(const void *vin, str_res *exp, void *vctx) {
    // For bigInt_tget_str() & bigInt_tget_strn()
    const stobi_assign_in *in = (stobi_assign_in*)in;
    io_ctx *ctx = (io_ctx*)vctx; size_t slen = strnlen(in->str, in->len);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(slen, 10));
    limb_t *tmp_limbs = (limb_t*)dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T);
    exp->data.bi = { .limbs = tmp_limbs, .n = 0, .cap = lcnt, .sign = 1 };
    exp->status = bigInt_get_strn(&exp->data.bi, in->str, in->len);
}
static inline void eval_stobi_tget_strb(const void *vin, str_res *exp, void *vctx) {
    // For bigInt_tget_strb() & bigInt_tget_strnb()
    const stobi_assign_in *in = (stobi_assign_in*)in;
    io_ctx *ctx = (io_ctx*)vctx; size_t slen = strnlen(in->str, in->len);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(slen, in->base));
    limb_t *tmp_limbs = (limb_t*)dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T);
    exp->data.bi = { .limbs = tmp_limbs, .n = 0, .cap = lcnt, .sign = 1 };
    exp->status = bigInt_get_strnb(&exp->data.bi, in->str, in->len, in->base);
}
// STOBI Scanning Inversese & Evaluators
static inline void inv_stobi_scan_nob(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for: bigInt_fscan & bigInt_fsscan
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strn(idk, len, out->data.bi, &tmp);
    *(char**)recon = idk;
}
static inline void inv_stobi_scan_b(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for: bigInt_fscanb & bigInt_fsscanb
    stobi_scan_in *in = (stobi_scan_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp);
    *(char**)recon = idk;
}
static inline void eval_stobi_ftscan(const void *vin, str_res *exp, void *vctx) {
    // For bigInt_ftscan()
    const stobi_scan_in *in = (stobi_scan_in*)in;
    io_ctx *ctx = (io_ctx*)vctx; size_t slen = 0;
    while (1) { char tmpbuf[1024];
        slen += fread(tmpbuf, sizeof(char), 1024, in->stream);
        if (feof(in->stream)) break;
    } size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(slen, 10));
    limb_t *tmp_limbs = (limb_t*)dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T);
    exp->data.bi = { .limbs = tmp_limbs, .n = 0, .cap = lcnt, .sign = 1 };
    exp->status = bigInt_fscan(in->stream, &exp->data.bi);
}
static inline void eval_stobi_ftscanb(const void *vin, str_res *exp, void *vctx) {
    // For bigInt_ftscanb()
    const stobi_scan_in *in = (stobi_scan_in*)in;
    io_ctx *ctx = (io_ctx*)vctx; size_t slen = 0;
    while (1) { char tmpbuf[1024];
        slen += fread(tmpbuf, sizeof(char), 1024, in->stream);
        if (feof(in->stream)) break;
    } size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(slen, in->base));
    limb_t *tmp_limbs = (limb_t*)dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T);
    exp->data.bi = { .limbs = tmp_limbs, .n = 0, .cap = lcnt, .sign = 1 };
    exp->status = bigInt_fscanb(in->stream, &exp->data.bi, in->base);
}
// STOBI Raw / Serial Input Inverses & Evaluators
static inline void inv_stobi_fread(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for: bigInt_fread & bigInt_fsread
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_serialize(idk, len, out->data.bi, &tmp);
    *(char**)recon = idk;
}
static inline void inv_stobi_deserial(const void *vin, const str_res *out, void *recon, void *vctx) {
    // for: bigInt_deserialize()
    io_ctx *ctx = (io_ctx*)vctx; size_t tmp = 0,
    len = bigInt_serial_size(out->data.bi);
    char* idk = (char*)dratch_alloc(ctx->buf, len);
    bigInt_serialize(idk, len, out->data.bi, &tmp);
    *(char**)recon = idk;
}
static inline void eval_stobi_ftread(const void *vin, str_res *exp, void *vctx) {
    // For bigInt_ftread()
    const stobi_fread_in *in = (stobi_fread_in*)in;
    io_ctx *ctx = (io_ctx*)vctx; size_t slen = bigInt_fscan_size(in->stream),
    lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(slen, 10));
    limb_t *tmp_limbs = (limb_t*)dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T);
    exp->data.bi = { .limbs = tmp_limbs, .n = 0, .cap = lcnt, .sign = 1 };
    exp->status = bigInt_fread(in->stream, &exp->data.bi);
}



#endif