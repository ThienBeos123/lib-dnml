#include "../header/mul.h"


/* BIGINT WORKSPACE SIZE */
size_t __BIGINT_KARATSUBA_WS__(size_t x_size, size_t y_size) {
    size_t m = (size_t)(max(x_size, y_size) / 2);
    size_t  x0_range = m,  x1_range = x_size - m;
    size_t  y0_range = m,  y1_range = y_size - m;

    size_t tmp1_size = max(x0_range, x1_range) + 1;
    size_t tmp2_size = max(y0_range, y1_range) + 1;
    size_t z0_size = x0_range + y0_range;
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + 2*m), x0_range + y0_range) + 1;

    return 3*(tmp1_size + tmp2_size + z0_size + z1_size + z2_size) * BYTES_IN_UINT64_T;
}
size_t __BIGINT_TOOM_WS__(size_t a_size, size_t b_size) {}
size_t __BIGINT_SSA_WS__(size_t a_size, size_t b_size) {}
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size) {
    if (a_size < BIGINT_SCHOOLBOOK && b_size < BIGINT_SCHOOLBOOK) return 0; // Doesn't need any
    else if (min(a_size, b_size) * 2 <= max(a_size, b_size)) return 0;
    else if (a_size < BIGINT_KARATSUBA && b_size < BIGINT_KARATSUBA) return __BIGINT_KARATSUBA_WS__(a_size, b_size);
    else if (a_size < BIGINT_TOOM && b_size < BIGINT_TOOM) return __BIGINT_TOOM_WS__(a_size, b_size);
    else return __BIGINT_SSA_WS__(a_size, b_size);
}

/* BIGINT ALGORITHMS */
void __BIGINT_SCHOOLBOOK__(const bigInt *a, const bigInt *b, bigInt *res) {
    memset(res->limbs, 0, (a->n + b->n) * sizeof(uint64_t)); // Set every bytes to 0 in res, basically CALLOC() without the MALLOC()
    // Implementing schoolbook multiplication, treating each limb like a digit
    // -----> Inner loop access each limb of b and multiplying by 1 limb of a before going to the next a's limb
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) {
            uint64_t low, high;
            low = __MUL_UI64__(a->limbs[i], b->limbs[j], &high);
            // Stored and calculated kinda in a staircase pattern seen in the sums of Schoolbook
            uint64_t sum =      res->limbs[i + j] /* Potential data from last iteration */ 
                            +   low /* Lower half (64 bit) */ 
                            +   carry /* Carry from the last iteration */;

            carry =     high /* The remaining half (64 bit) */ 
                    +   (sum < low) /* Overflow from adding the lower half */ 
                    +   (sum < carry) /* Overflow from adding the last iteration's carry */;

            res->limbs[i + j] = sum; // Apply the new sum
        }
        res->limbs[i + b->n] += carry; // Add the remaining carry to the largest, 
                                       // most significant limb of the current sum
    }
    res->n = a->n + b->n;
}
void __BIGINT_KARATSUBA__(const bigInt *x, const bigInt *y, bigInt *res, calc_ctx karat_ctx) {
    if (x->n <= BIGINT_SCHOOLBOOK && y->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(x, y, res); return;
    } //* ---- 1. SETUP ---- *?/
    size_t m = (size_t)(max(x->n, y->n) / 2);
    size_t  x0_range = m,  x1_range = x->n - m;
    size_t  y0_range = m,  y1_range = y->n - m;
    bigInt x0 = {.limbs = x->limbs,             .n = x0_range, .cap = x0_range};
    bigInt x1 = {.limbs = x->limbs + x0_range,  .n = x1_range, .cap = x1_range};
    bigInt y0 = {.limbs = y->limbs,             .n = y0_range, .cap = y0_range};
    bigInt y1 = {.limbs = y->limbs + y0_range,  .n = y1_range, .cap = y1_range};

    size_t karat_mark = scratch_mark(&karat_ctx);
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + 2*m), x0_range + y0_range) + 1;

    limb_t *tmp1_limbs = scratch_alloc(&karat_ctx, (max(x0_range, x1_range) + 1) * BYTES_IN_UINT64_T);
    limb_t *tmp2_limbs = scratch_alloc(&karat_ctx, (max(y0_range, y1_range) + 1) * BYTES_IN_UINT64_T);
    limb_t *z0_limbs = scratch_alloc(&karat_ctx, (x0_range + y0_range) * BYTES_IN_UINT64_T);
    limb_t *z1_limbs = scratch_alloc(&karat_ctx, z1_size * BYTES_IN_UINT64_T);
    limb_t *z2_limbs = scratch_alloc(&karat_ctx, z2_size * BYTES_IN_UINT64_T);

    bigInt tmp1 = {.limbs = tmp1_limbs, .n = 0, .cap = max(x0_range, x1_range) + 1};
    bigInt tmp2 = {.limbs = tmp2_limbs, .n = 0, .cap = max(y0_range, y1_range) + 1};
    bigInt z0 = {.limbs = z0_limbs, .n = 0, .cap = x0_range + y0_range};
    bigInt z1 = {.limbs = z1_limbs, .n = 0, .cap = z1_size};
    bigInt z2 = {.limbs = z2_limbs, .n = 0, .cap = z2_size};

    //* ------- 2. QUADRATIC COMPONENTS CALCULATION -------- *//
    // The procedure basically gpes:
    //  z3 = (x1 + x0)(y1 + y0)
    //  z2 = x1 * y1
    //  z0 = x0 * y0
    //  z1 = z3 - z2 - z1
    __BIGINT_KARATSUBA__(&x0, &y0, &z0, karat_ctx);
    __BIGINT_KARATSUBA__(&x1, &y1, &z2, karat_ctx);
    __BIGINT_ADD_WC__(&tmp1, &x1, &x0); __BIGINT_ADD_WC__(&tmp2, &y1, &y0);
    __BIGINT_KARATSUBA__(&tmp1, &tmp2, &z1, karat_ctx);
    __BIGINT_SUB_WB__(&z1, &z1, &z2); __BIGINT_SUB_WB__(&z1, &z1, &z0);

    //* ------------ 3. FINAL CALCULATION -------------- *//
    __BIGINT_INTERNAL_LLSHIFT__(&z2, 2*m); __BIGINT_INTERNAL_LLSHIFT__(&z1, m);
    __BIGINT_ADD_WC__(&z2, &z2, &z1); __BIGINT_ADD_WC__(&z2, &z2, &z0);
    __BIGINT_INTERNAL_COPY__(res, &z2);
    scratch_reset(&karat_ctx, karat_mark);
}
void __BIGINT_TOOM__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {
    if (m->n <= BIGINT_SCHOOLBOOK && n->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(m, n, res); return;
    } //* -------- 1. SETUP & SPLITTING -------- *//
    size_t k = (size_t)(max(m->n, n->n) / 3) + 1;
    size_t m0_m1_range = k,   /**/   m2_range = m->n - 2*k;
    size_t n0_n1_range = k,   /**/   n2_range = n->n - 2*k;
    bigInt m0 = {.limbs = m->limbs,         .n = m0_m1_range,   .cap = m0_m1_range};
    bigInt m1 = {.limbs = m->limbs + k,     .n = m0_m1_range,   .cap = m0_m1_range};
    bigInt m2 = {.limbs = m->limbs + 2*k,   .n = m2_range,      .cap = m2_range};
    bigInt n0 = {.limbs = n->limbs,         .n = n0_n1_range,   .cap = n0_n1_range};
    bigInt n1 = {.limbs = n->limbs + k,     .n = n0_n1_range,   .cap = n0_n1_range};
    bigInt n2 = {.limbs = n->limbs + 2*k,   .n = n2_range,      .cap = n2_range};

    //* -------- 2. EVALUATION & POINT-WISE MULTIPLICATION -------- *//

}
void __BIGINT_SSA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx) {}
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx) {
    if (a->n < BIGINT_SCHOOLBOOK && b->n < BIGINT_SCHOOLBOOK) __BIGINT_SCHOOLBOOK__(a, b, res);
    else if (min(a->n, b->n) * 2 <= max(a->n, b->n)) __BIGINT_SCHOOLBOOK__(a, b, res);
    else if (a->n < BIGINT_KARATSUBA && b->n < BIGINT_KARATSUBA) __BIGINT_KARATSUBA__(a, b, res, mul_ctx);
    else if (a->n < BIGINT_TOOM && b->n < BIGINT_TOOM) __BIGINT_TOOM__(a, b, res, mul_ctx);
    else __BIGINT_SSA__(a, b, res, mul_ctx);
}