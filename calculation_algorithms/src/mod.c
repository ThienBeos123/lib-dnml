#include "../header/mod.h"

/* ----------------- BigInt ----------------- */
void __BIGINT_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem) {
    //* ---- 1. PRECOMPUTATION - μ ---- *//
    bigInt numerator, precomputation; // Precomputation
    __BIGINT_INTERNAL_LINIT__(&numerator, 2 * n->n + 1);
    __BIGINT_INTERNAL_LINIT__(&precomputation, 2 * n->n + 1);
    numerator.limbs[2 * n->n] = 1;
    __BIGINT_DIVMOD_DISPATCH__(&numerator, n, &precomputation, rem);
    //. *rem here serves a temporary holder job


    //* ---- 2. NUMERATOR CALCULATION ---- *//
    size_t remaining_limbs = a->n - (n->n - 1);
    bigInt a_after_shift; __BIGINT_INTERNAL_LINIT__(&a_after_shift, remaining_limbs);
    memcpy(a_after_shift.limbs, &a->limbs[n->n - 1], remaining_limbs * BYTES_IN_UINT64_T);
    // We copy starting from the third limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIGINT_INTERNAL_ENSCAP__(&numerator, remaining_limbs + precomputation.n);
    __BIGINT_MUL_DISPATCH__(&numerator, &a_after_shift, &precomputation);

    //* ---- 3. FINAL CALCULATION ---- *//
    // precomputation now acts as the quotient to save memory
    remaining_limbs = numerator.n - (n->n + 1); __BIGINT_INTERNAL_ENSCAP__(&precomputation, remaining_limbs);
    memcpy(precomputation.limbs, &numerator.limbs[n->n - 1], remaining_limbs * BYTES_IN_UINT64_T);
    __BIGINT_MUL_DISPATCH__(&a_after_shift, &precomputation, n);
    __BIGINT_INTERNAL_COPY__(rem, a); __BIGINT_INTERNAL_SUB__(rem, &a_after_shift);
    int8_t comp_res = __BIGINT_INTERNAL_COMP__(rem, n);
    while (comp_res == 1 || !comp_res) __BIGINT_INTERNAL_SUB__(rem, n);
    __BIGINT_INTERNAL_FREE__(&numerator);
    __BIGINT_INTERNAL_FREE__(&precomputation);
    __BIGINT_INTERNAL_FREE__(&a_after_shift);
}
void __BIGINT_MONT_REDC__(const bigInt *a, const bigInt *n, bigInt *rem) {}
void __BIGINT_MOD_DISPATCH__(const bigInt *a, const bigInt *n, bigInt *rem) {}