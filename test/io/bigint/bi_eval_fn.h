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
*      might be lost, will NOT have an inverse, since the input
*      is unrecoverable autonomously, and therefore resorts to EVALUATION (see Note 2)
*      ------> Truncative Variants do NOT have inverses
*/


/* Note 2 - Evaluation:
*   +) Each evaluation function are semantic/format based
*      ---> Each evaluation function fits one format of output
*   +) Example: tto_str & tto_strn has output of purely base-10 with no base-prefix
*      -------> We use to_strn as the full output evaluator since
*               its output is also purely base-19 with no base-prefix
*/


//* ========================= BITOS EVALUATION WRAPPERS ======================= *//
// BITOS Conversions Inverses
static inline void inv_bitos_to_str_det(const void *vin, const str_res *out, void *recon, void *vctx) {
    io_ctx *ctx = (io_ctx*)vctx;
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)(dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T));
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    bigInt_get_strn(&tmp, out->str, out->data.len);
    *(bigInt*)recon = tmp;
}
static inline void inv_bitos_to_str_indet(const void *vin, const str_res *out, void *recon, void *vctx) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)(dratch_alloc(ctx->buf, lcnt * BYTES_IN_UINT64_T));
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    bigInt_get_strnb(&tmp, out->str, out->data.len, in->base);
    *(bigInt*)recon = tmp;
}
// BITOS Evaluators
static inline void eval_bitos_tto_str(const void *vin, const str_res *out, void *vctx) {
    // For: tto_str, tto_strn
    const bitos_conv_in *in = (bitos_conv_in*)in;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t needed = __BIGINT_COUNTDB__(&in->x, 10);
    char *full = (char*)dratch_alloc(ctx->buf, needed + 1);
    bigInt_to_strn(full, needed + 1, in->x, &needed);
}
static inline void eval_bitos_tto_strb(const void *vin, const str_res *out, void *vctx) {
    // For: tto_strb, tto_strnb
    const bitos_conv_in *in = (bitos_conv_in*)in;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t needed = __BIGINT_COUNTDB__(&in->x, in->base);
    char *full = (char*)dratch_alloc(ctx->buf, needed + 1);
    bigInt_to_strnb(full, needed + 1, in->x, in->base, &needed);
}
static inline void eval_bitos_tto_strf(const void *vin, const str_res *out, void *vctx) {
    // For: tto_strf
    const bitos_conv_in *in = (bitos_conv_in*)in;
    io_ctx *ctx = (io_ctx*)vctx;
    size_t needed = __BIGINT_COUNTDB__(&in->x, in->base);
    char *full = (char*)dratch_alloc(ctx->buf, needed + 1);
    bigInt_to_strf(full, needed + 1, in->x, in->base, in->uppercase, &needed);
}




#endif