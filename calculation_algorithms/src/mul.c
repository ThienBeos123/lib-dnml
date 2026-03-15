#include "../header/mul.h"


/* BIGINT WORKSPACE SIZE */
size_t __BIGINT_KARATSUBA_WS__(size_t a_size, size_t b_size) {}
size_t __BIGINT_TOOM_WS__(size_t a_size, size_t b_size) {}
size_t __BIGINT_SSA_WS__(size_t a_size, size_t b_size) {}
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size) {
    if (a_size < BIGINT_SCHOOLBOOK && b_size < BIGINT_SCHOOLBOOK) return 0; // Doesn't need any
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
        __BIGINT_SCHOOLBOOK__(x, y, res);
        return;
    } //* ---- 1. SETUP ---- *?/
    size_t  x0_range = (size_t)(x->n / 2),  x1_range = x->n - x0_range;
    size_t  y0_range = (size_t)(y->n / 2),  y1_range = y->n - y0_range;
    bigInt x0 = {.limbs = x->limbs,             .n = x0_range, .cap = x0_range};
    bigInt x1 = {.limbs = x->limbs + x0_range,  .n = x1_range, .cap = x1_range};
    bigInt y0 = {.limbs = y->limbs,             .n = y0_range, .cap = y0_range};
    bigInt y1 = {.limbs = y->limbs + y0_range,  .n = y1_range, .cap = y1_range};

    size_t karat_mark = scratch_mark(&karat_ctx);
    limb_t *tmp1_limbs = scratch_alloc(&karat_ctx, (max(x0_range, x1_range) + 1) * BYTES_IN_UINT64_T);
    limb_t *tmp2_limbs = scratch_alloc(&karat_ctx, (max(y0_range, y1_range) + 1) * BYTES_IN_UINT64_T);
    limb_t *z0_limbs = scratch_alloc(&karat_ctx, (x0_range + y0_range) * BYTES_IN_UINT64_T);
    limb_t *z2_limbs = scratch_alloc(&karat_ctx, (x1_range + y1_range) * BYTES_IN_UINT64_T);
    limb_t *z1_limbs = scratch_alloc(&karat_ctx, 
        (max(x1_range + y0_range, x0_range + y1_range) + 1) * BYTES_IN_UINT64_T);
    bigInt tmp1 = {.limbs = tmp1_limbs, .n = 0, .cap = max(x0_range, x1_range) + 1};
    bigInt tmp1 = {.limbs = tmp2_limbs, .n = 0, .cap = max(y0_range, y1_range) + 1};
    bigInt z0 = {.limbs = z0_limbs, .n = 0, .cap = x0_range + y0_range};
    bigInt z2 = {.limbs = z2_limbs, .n = 0, .cap = x1_range + y1_range};
    bigInt z1 = {.limbs = z1_limbs, .n = 0, .cap = max(x1_range + y0_range, x0_range + y1_range) + 1};

    //* ------- 2. ALGORITHM CALLS -------- *//
    __BIGINT_KARATSUBA__(&x0, &y0, &z0, karat_ctx);
    __BIGINT_KARATSUBA__(&x1, &y1, &z2, karat_ctx);
}
void __BIGINT_TOOM__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx toom_ctx) {}
void __BIGINT_SSA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx) {}
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx) {
    if (a->n < BIGINT_SCHOOLBOOK && b->n < BIGINT_SCHOOLBOOK) __BIGINT_SCHOOLBOOK__(a, b, res);
    else if (a->n < BIGINT_KARATSUBA && b->n < BIGINT_KARATSUBA) __BIGINT_KARATSUBA__(a, b, res, mul_ctx);
    else if (a->n < BIGINT_TOOM && b->n < BIGINT_TOOM) __BIGINT_TOOM__(a, b, res, mul_ctx);
    else __BIGINT_SSA__(a, b, res, mul_ctx);
}