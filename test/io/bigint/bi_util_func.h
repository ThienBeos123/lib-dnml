#ifndef _BIGINT_UTIL_FUNC_H
#define _BIGINT_UTIL_FUNC_H


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "bi_indef.h"
#include "../../../test_ui/_strui.h"
#include "../../../adynamol/big_numbers/bigNums.h"


//* ========================= COMPARISONS WRAPPERS ======================= *//
// Invsere Comparator - BITOS
stinl bool cmp_inv_bitos_conv(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const bitos_conv_in *orinbi = (bitos_conv_in*)orin;
    const bigInt *reconbi = (bigInt*)recon;
    return (__BIGINT_INTERNAL_COMP__(&orinbi->x, reconbi) == 0);
}
stinl bool cmp_inv_bitos_put(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const bitos_print_in *orinbi = (bitos_print_in*)orin;
    const bigInt *reconbi = (bigInt*)recon;
    return (__BIGINT_INTERNAL_COMP__(&orinbi->x, reconbi) == 0);
}
stinl bool cmp_inv_bitos_fwrite(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); } 
stinl bool cmp_inv_bitos_serial(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); } 
// Inverse Comparator - STOBI
stinl bool cmp_inv_stobi_init(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_initb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const stobi_init_in *orins = (stobi_init_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_conv(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_convb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const stobi_conv_in *orins = (stobi_conv_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_assign(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_assignb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const stobi_assign_in *orins = (stobi_assign_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_scan(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_scanb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    const stobi_scan_in *orins = (stobi_scan_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)dratch_alloc(ctx->buf, len * BYTES_IN_UINT64_T);
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_fread(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); }
stinl bool cmp_inv_stobi_deserial(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); }
// Evaluation Comparator
stinl bool cmp_eval_bitos(csres *exp, csres *res) {
    // Property Check
    if (res->type != exp->type) return false;
    if (res->status != exp->status) return false;
    if (res->status != STR_SUCCESS || res->status != STR_TRUNC_SUCCESS) return true;
    // Main Check
    if (res->data.len == exp->data.len) return strcmp(res->str, exp->str) == 0;
    else if (res->data.len > exp->data.len) return false;
    else return strncmp(res->str, exp->str, res->data.len) == 0;
}
stinl bool cmp_eval_stobi(csres *exp, csres *res) {
    if (res->status != exp->status) return false;
    else if (res->status == STR_SUCCESS || res->status == STR_TRUNC_SUCCESS) {
        return (__BIGINT_INTERNAL_COMP__(&res->data.bi, &exp->data.bi));
    } else return true;
}




#endif