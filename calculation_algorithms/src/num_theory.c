#include "../header/num_theory.h"

static const uint32_t dmr_bases[7] = {
    2, 325, 9375, 28178, 
    450775, 9780504, 
    1794265022
};

/* GCD - GREATEST COMMON DIVISOR */
uint64_t __BIGINT_EUCLID__(uint64_t u, uint64_t v) {
    uint64_t remainder = (u < v) ? u : v;
    uint64_t dividend = (u >= v) ? u : v;
    uint64_t old_remainder;
    while (remainder) {
        old_remainder = remainder;
        remainder = dividend % remainder;
        dividend = old_remainder;
    }
    return dividend;
}
void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v) {
    // Base case - Identity #1 - gcd(u, 0) = u
    if (u->n == 0) { __BIGINT_INTERNAL_COPY__(res, v); return; }
    else if (v->n == 0) { __BIGINT_INTERNAL_COPY__(res, u); return; }

    // Setup - Identity #2 - gcd(2u, 2v) = gcd(u, v)
    bigInt u_copy; __BIGINT_INTERNAL_LINIT__(&u_copy, u->n);
    bigInt v_copy; __BIGINT_INTERNAL_LINIT__(&v_copy, v->n);
    memcpy(u_copy.limbs, u->limbs, u->n * BYTES_IN_UINT64_T); u_copy.n = u->n;
    memcpy(v_copy.limbs, v->limbs, v->n * BYTES_IN_UINT64_T); v_copy.n = v->n;
    size_t i = __BIGINT_CTZ__(u); __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
    size_t j = __BIGINT_CTZ__(v); __BIGINT_INTERNAL_RSHIFT__(&v_copy, j);
    size_t k = min(i, j);

    // Procedure
    int8_t comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    while (comp_res) {
        if (comp_res == -1) __BIGINT_INTERNAL_SWAP__(&u_copy, &v_copy);
        // Identity #4: gcd(u, v) = gcd(u, v - u)
        //  WHEN:
        //      +) u & v is ODD
        //      +) u <= v
        __BIGINT_INTERNAL_SUB__(&u_copy, &v_copy);
        // Identity #3 - gcd(u, 2v) = gcd(u, v)
        i = __BIGINT_CTZ__(&u_copy);
        __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
        comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    }
    __BIGINT_INTERNAL_LSHIFT__(&u_copy, k);
    __BIGINT_INTERNAL_COPY__(res, &u_copy);
}
void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v) {}
void __BIGINT_HALF__(bigInt *res, const bigInt *u, const bigInt *v) {}
void __BIGINT_GCD_DISPATCH__(bigInt *res, const bigInt *u, const bigInt *v) {
    size_t op_size = min(u->n, v->n);
    __BIGINT_INTERNAL_ENSCAP__(res, op_size);
    if (u->n == 1 && v->n == 1) {
        res->limbs[0] = __BIGINT_EUCLID__(u->limbs[0], v->limbs[0]);
        res->n        = 1;
    } else if (op_size <= BIGINT_STEIN) __BIGINT_STEIN__(res, u, v);
    else if (op_size <= BIGINT_LEHMER)  __BIGINT_LEHMER__(res, u, v);
    else                                __BIGINT_HALF__(res, u, v);
}

/* Primality Testing */
uint8_t __BIGINT_TRIAL_DIV__(uint64_t x) {
    if (x <= 1) return 0;
    else if (x == 2 || x == 3 || x == 5) return 1;
    else if (!(x & 1) || !(x % 3) || !(x % 5)) return 0;
    uint8_t steps[8] = {6, 4, 2, 4, 2, 4, 6, 2};
    uint8_t steps_i = 1;
    for (uint64_t i = 7; 
        i <= (uint64_t)(sqrt(x)) + 1; 
        i += steps[steps_i]
    ) { 
        if (x % i == 0) return 0;
        steps_i = (steps_i < 7) ? steps_i + 1 : 0;
    } return 1;
}
uint8_t __BIGINT_SMALL_MRABIN__(uint64_t n) {
    uint64_t s = 0, d = n - 1, x;
    uint8_t composite = 1;
    while (!(d & 1)) { ++s; d >>= 1; }
    for (uint8_t i = 0; i < 7; ++i) {
        uint32_t curr_base = dmr_bases[i];
        x = __MODEXP_UI64__(curr_base, d, n);
        // 1. Check a^d mod(n) = 1
        if (x == 1 || x == n - 1) continue;
        // 2. Check for a^(2^r * d) mod(n) = n - 1
        composite = 1;
        for (uint64_t r = 1; r < s; ++r) {
            x = __MODMUL_UI64__(x, x, n);
            if (x == n - 1) { composite = 0; break; }
        } if (composite) return 0;
    } return 1;
}
uint8_t __BIGINT_MILLER_RABIN__(const bigInt *x, uint64_t base) {}
uint8_t __BIGINT_BPSW__(const bigInt *x) {}
uint8_t __BIGINT_ECPP__(const bigInt *x) {}
uint8_t __BIGINT_PTEST_DISPATCH__(const bigInt *x) {}
