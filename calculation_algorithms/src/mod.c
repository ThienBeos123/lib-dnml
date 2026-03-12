#include "../header/mod.h"

/* ----------------- BigInt ----------------- */
size_t __BIGINT_BARETT_WS__(size_t a_size, size_t n_size) {
    uint8_t object_count = 5;
    // Precomputation Temporaries
    size_t numlimbs_size = (2 * n_size + 1) * BYTES_IN_UINT64_T;
    size_t prelimbs_size = (2 * n_size + 1) * BYTES_IN_UINT64_T;
    size_t tmp_size = n_size * BYTES_IN_UINT64_T;
    // Numerator Calculation Temporaries
    size_t aaslimbs_size = (a_size - n_size + 1) * BYTES_IN_UINT64_T;
    size_t anumerator_size = (a_size + n_size + 2) * BYTES_IN_UINT64_T;
    // Final Calculation Temporaries
    size_t acopy_size = a_size * BYTES_IN_UINT64_T;
    size_t additional_size = 0;
    if (likely((2 * n_size + 1) < (a_size + 1))) {
        ++object_count; additional_size = (a_size + 1) * BYTES_IN_UINT64_T;
    }
    // Paddings & Low-level Arenas
    size_t max_align = object_count * alignof(max_align_t);
    size_t mul_divmod_size = max(
        __BIGINT_MUL_WS__(anumerator_size, n_size), max(
            __BIGINT_MUL_WS__(aaslimbs_size, prelimbs_size), 
            __BIGINT_DIVMOD_WS__(&numlimbs_size, n_size)
        )
    ); return (numlimbs_size + prelimbs_size + tmp_size 
             + aaslimbs_size + anumerator_size 
             + acopy_size + additional_size 
             + max_align + mul_divmod_size);
}
size_t __BIGINT_REDC_WS__(mont_ctx ctx) { return (2*ctx.k) * BYTES_IN_UINT64_T; }
size_t __BIGINT_MOD_WS__(size_t a_size, size_t n_size) {
    if (n_size < BIGINT_SHORT) return __BIGINT_SHORTDIV_WS__(a_size, n_size);
    else if (n_size < BIGINT_KNUTH) return __BIGINT_KNUTH_WS__(a_size, n_size);
    else if (n_size < BIGINT_BARETT) return __BIGINT_BARETT_WS__(a_size, n_size);
    else return __BIGINT_NEWTON_WS__(a_size, n_size);
}
void __BIGINT_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem, calc_ctx barett_ctx) {
    //* ---- 1. PRECOMPUTATION - μ ---- *//
    size_t barett_mark = scratch_mark(&barett_ctx), precomp_size = 2 * n->n + 1;
    limb_t *numerator_limbs = scratch_alloc(&barett_ctx, precomp_size * BYTES_IN_UINT64_T);
    limb_t *precomp_limbs = scratch_alloc(&barett_ctx, precomp_size * BYTES_IN_UINT64_T);
    limb_t *tmp_limbs = scratch_alloc(&barett_ctx, n->n * BYTES_IN_UINT64_T);
    bigInt numerator = {.limbs = numerator_limbs, .n = precomp_size, .cap = precomp_size, .sign = 1};
    bigInt precomputation = {.limbs = precomp_limbs, .n = 0, .cap = precomp_size, .sign = 1};
    bigInt tmp = {.limbs = tmp_limbs, .n = 0, .cap = n->n, .sign = 1};
    numerator.limbs[2 * n->n] = 1;
    __BIGINT_DIVMOD_DISPATCH__(&numerator, n, &precomputation, &tmp, barett_ctx);


    //* ---- 2. NUMERATOR CALCULATION ---- *//
    size_t remaining_limbs = a->n - (n->n - 1);
    limb_t *aas_limbs = scratch_alloc(&barett_ctx, remaining_limbs * BYTES_IN_UINT64_T);
    limb_t *anumerator_limbs = scratch_alloc(&barett_ctx, (remaining_limbs + precomputation.n) * BYTES_IN_UINT64_T);
    bigInt a_after_shift = {.limbs = aas_limbs, .n = remaining_limbs, .cap = remaining_limbs, .sign = 1};
    bigInt anumerator = {
        .limbs = anumerator_limbs, .sign = 1,
        .n = 0, .cap = (remaining_limbs + precomputation.n)
    }; memcpy(a_after_shift.limbs, &a->limbs[n->n - 1], remaining_limbs * BYTES_IN_UINT64_T);
    // We copy starting from the n limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIGINT_MUL_DISPATCH__(&a_after_shift, &precomputation, &anumerator, barett_ctx);

    //* ---- 3. FINAL CALCULATION ---- *//
    remaining_limbs = anumerator.n - (n->n + 1); // This value can be shortened to a->n + 1
    memcpy(anumerator.limbs, &anumerator.limbs[n->n - 1], remaining_limbs * BYTES_IN_UINT64_T);
    limb_t *acopy_limbs = scratch_alloc(&barett_ctx, a->n * BYTES_IN_UINT64_T);
    memcpy(acopy_limbs, a->limbs, a->n * BYTES_IN_UINT64_T);
    bigInt a_copy = {.limbs = acopy_limbs, .n = a->n, .cap = a->n, .sign = a->sign};
    if (unlikely(precomputation.cap >= remaining_limbs)) {
        precomputation.n = 0; precomputation.sign = 1;
        __BIGINT_MUL_DISPATCH__(&anumerator, n, &precomputation, barett_ctx);
        __BIGINT_INTERNAL_SUB__(&a_copy, &precomputation);
    } else {
        limb_t *final_limb = scratch_alloc(&barett_ctx, remaining_limbs);
        bigInt final_res = {.limbs = final_limb, .n = 0, .cap = remaining_limbs, .sign = 1};
        __BIGINT_MUL_DISPATCH__(&anumerator, n, &final_res, barett_ctx);
        __BIGINT_INTERNAL_SUB__(&a_copy, &final_res); 
    } 
    while (__BIGINT_INTERNAL_COMP__(&a_copy, n) >= 0) __BIGINT_INTERNAL_SUB__(&a_copy, n);
    __BIGINT_INTERNAL_COPY__(rem, &a_copy); scratch_reset(&barett_ctx, barett_mark);
}
void __BIGINT_MONT_REDC__(const bigInt *t, mont_ctx mredc_ctx, bigInt *rem, calc_ctx redc_ctx) {
    uint64_t m = t->limbs[0] * mredc_ctx.nprime, carry = 0;
    size_t mont_redc_mark = scratch_mark(&redc_ctx);
    limb_t *tmp_tlimbs = scratch_alloc(&redc_ctx, (2*mredc_ctx.k) * BYTES_IN_UINT64_T);
    bigInt tmp_t = {.limbs = tmp_tlimbs, .n = 0, .cap = 2*mredc_ctx.k, .sign = 1};
    // Loop basically cancels k lowest limbs
    for (size_t i = 0; i < mredc_ctx.k; ++i) {
        uint64_t lo, hi;
        lo = __MUL_UI64__(mredc_ctx.n->limbs[i], m, &hi);
        tmp_t.limbs[i] = __ADD_UI64__(t->limbs[i], lo + carry, &carry);
        carry += hi;
    } tmp_t.limbs[mredc_ctx.k] += carry;
    // Right Limb Shift by k  --  t +>> k (+>> or +<< means LIMB SHIFT)
    memcpy(&tmp_t.limbs[0], &t->limbs[mredc_ctx.k - 2], (mredc_ctx.k + 1) * BYTES_IN_UINT64_T);
    tmp_t.n = mredc_ctx.k + 1; // From 2k + 1 (upperbound) ---> k + 1 (from the limb shift)
    if (__BIGINT_INTERNAL_COMP__(&tmp_t, mredc_ctx.n) > 0) __BIGINT_INTERNAL_SUB__(&tmp_t, mredc_ctx.n);
    __BIGINT_INTERNAL_COPY__(rem, &tmp_t); scratch_reset(&mredc_ctx, mont_redc_mark);
}
void __BIGINT_MOD_DISPATCH__(
    const bigInt *a, const bigInt *n, 
    bigInt *rem, bigInt *tmp_quot, calc_ctx mod_ctx
) {
    if (n->n < BIGINT_SHORT) __BIGINT_SHORT_DIVISION__(a, n, &tmp_quot, rem, mod_ctx);
    else if (n->n < BIGINT_KNUTH) __BIGINT_KNUTH_D__(a, n, &tmp_quot, rem, mod_ctx);
    else if (n->n < BIGINT_BARETT) __BIGINT_BARETT__(a, n, rem, mod_ctx);
    else __BIGINT_NEWTON_RECIPROCAL__(a, n, &tmp_quot, rem, mod_ctx);
}