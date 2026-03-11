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
uint8_t __BIGINT_MILLER_RABIN__(const bigInt *n, const bigInt* base) {
    if (n->sign == -1) return 0; uint8_t prim_status = 0; uint64_t a[1] = {1};
    bigInt n_minus_one; __BIGINT_INTERNAL_LINIT__(&n_minus_one, n->n);
    bigInt constant_one = { .limbs = a, .n = 1, .cap = 1, .sign = 1 };
    __BIGINT_INTERNAL_COPY__(&n_minus_one, n);
    __BIGINT_INTERNAL_SUB__(&n_minus_one, &constant_one);
    size_t s = __BIGINT_CTZ__(&n_minus_one);
    bigInt d; __BIGINT_INTERNAL_LINIT__(&d, n_minus_one.n);
    __BIGINT_INTERNAL_COPY__(&d, &n_minus_one);
    __BIGINT_INTERNAL_RLSHIFT__(&d, (uint64_t)(s / BITS_IN_UINT64_T));
    __BIGINT_INTERNAL_RSHIFT__(&d, (uint64_t)(s % BITS_IN_UINT64_T));
    s = (uint64_t)s;
    
    // 1st test: a^d mod(n)
    bigInt x; __BIGINT_INTERNAL_LINIT__(&x, n->n);
    __BIGINT_BINARY_MODEXP__(&x, n, base, &d);
    if (x.n == 1 && x.limbs[0] == 1) prim_status = 1; // a^d mod(n) = 1
    else if (!__BIGINT_INTERNAL_COMP__(&x, &n_minus_one)) prim_status = 1; // a^d mod(n) = n - 1

    // 2nd test: a^(2^r * d) mod(n)
    if (s <= 5) {
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_CLASSICAL_MODMUL__(&x, &x, n, &x);
            if (x.n == 1 && x.limbs[0] == 1) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_minus_one)) { prim_status = 1; break; }
        }
    } else {
        mont_ctx mrabin_ctx = {
            .n = n,     /**/    .nprime = __MODINV_UI64__(n->limbs[0]),
            .k = n->n,  /**/
        }; bigInt r, r_mod_n, tmp;
        __BIGINT_INTERNAL_LINIT__(&r, n->n + 1);
        __BIGINT_INTERNAL_LINIT__(&r_mod_n, n->n);
        __BIGINT_INTERNAL_LINIT__(&tmp, n->n + 1);
        r.limbs[n->n] = 1; __BIGINT_MOD_DISPATCH__(&r, n, &r_mod_n, &tmp);
        __BIGINT_INTERNAL_ENSCAP__(&tmp, n->n * 2);
        __BIGINT_INTERNAL_ENSCAP__(&r, n->n * 2);
        __BIGINT_MUL_DISPATCH__(&tmp, &r_mod_n, &r_mod_n);
        __BIGINT_MOD_DISPATCH__(&tmp, n, &r_mod_n, &r);
        mrabin_ctx.r2 = &r_mod_n;
        // Conversions
        constant_one.limbs = NULL; __BIGINT_INTERNAL_LINIT__(&constant_one, n->n); 
        constant_one.limbs[0] = 1; constant_one.n = 1; constant_one.sign = 1;
        __BIGINT_MONTMUL__(&x, mrabin_ctx.r2, mrabin_ctx, &x);
        __BIGINT_MONTMUL__(&n_minus_one, mrabin_ctx.r2, mrabin_ctx, &n_minus_one);
        __BIGINT_MONTMUL__(&constant_one, mrabin_ctx.r2, mrabin_ctx, &constant_one);
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_MONTMUL__(&x, &x, mrabin_ctx, &x);
            if (!__BIGINT_INTERNAL_COMP__(&x, &constant_one)) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_minus_one)) { prim_status = 1; break; }
        } __BIGINT_INTERNAL_FREE__(&r); __BIGINT_INTERNAL_FREE__(&r_mod_n); __BIGINT_INTERNAL_FREE__(&tmp);
    } __BIGINT_INTERNAL_FREE__(&n_minus_one); __BIGINT_INTERNAL_FREE__(&constant_one);
    __BIGINT_INTERNAL_FREE__(&s); __BIGINT_INTERNAL_FREE__(&d); __BIGINT_INTERNAL_FREE__(&x);
    return prim_status;
}
uint8_t __BIGINT_BPSW__(const bigInt *x) {}
uint8_t __BIGINT_ECPP__(const bigInt *x) {}
uint8_t __BIGINT_PTEST_DISPATCH__(const bigInt *x) {}
