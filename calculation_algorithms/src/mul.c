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
void __BIGINT_KARATSUBA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx karat_ctx) {}
void __BIGINT_TOOM__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx toom_ctx) {}
void __BIGINT_SSA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx) {}
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx) {
    if (a->n < BIGINT_SCHOOLBOOK && b->n < BIGINT_SCHOOLBOOK) __BIGINT_SCHOOLBOOK__(a, b, res);
    else if (a->n < BIGINT_KARATSUBA && b->n < BIGINT_KARATSUBA) __BIGINT_KARATSUBA__(a, b, res, mul_ctx);
    else if (a->n < BIGINT_TOOM && b->n < BIGINT_TOOM) __BIGINT_TOOM_3__(a, b, res, mul_ctx);
    else __BIGINT_SSA__(a, b, res, mul_ctx);
}