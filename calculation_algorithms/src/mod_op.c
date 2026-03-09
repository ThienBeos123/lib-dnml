#include "../header/mod_op.h"

void __BIGINT_BINARY_MODEXP__(
    bigInt *res, const bigInt *modulus,
    const bigInt *base, const bigInt *power
) {
    //* --- 1. SETUP ---- *//
    mont_ctx modexp_contx = { 
        .n      = modulus,
        .nprime = __MODINV_UI64__(modulus->limbs[0]),
        .k      = modulus->n
    }; bigInt r, r_mod_n, tmp;
    __BIGINT_INTERNAL_LINIT__(&r, modulus->n + 1);
    __BIGINT_INTERNAL_LINIT__(&r_mod_n, modulus->n);
    __BIGINT_INTERNAL_LINIT__(&tmp, modulus->n + 1);
    r.limbs[modulus->n] = 1; __BIGINT_MOD_DISPATCH__(&r, modulus, &r_mod_n, &tmp);
    __BIGINT_INTERNAL_ENSCAP__(&tmp, modulus->n * 2);
    __BIGINT_INTERNAL_ENSCAP__(&r, modulus->n * 2);
    __BIGINT_MUL_DISPATCH__(&tmp, &r_mod_n, &r_mod_n);
    __BIGINT_MOD_DISPATCH__(&tmp, modulus, &r_mod_n, &r);
    modexp_contx.r2 = &r_mod_n;

    //* ----- 2. MAIN LOOP ----- *//
    bigInt tmp_res; __BIGINT_INTERNAL_LINIT__(&tmp_res, modulus->n);
    while (power->n > 0) {
        if (power->limbs[0] & 1) {
            __BIGINT_MONTMUL__(&tmp_res, base, modexp_contx, &tmp);
            __BIGINT_INTERNAL_COPY__(&tmp_res, &tmp);
        }
        __BIGINT_MONTMUL__(&base, &base, modexp_contx, &tmp);
        __BIGINT_INTERNAL_COPY__(&base, &tmp);
        
    }
}


void __BIGINT_MONTMUL__(
    const bigInt *a, const bigInt *b, 
    mont_ctx ctx, bigInt *res
) {
    bigInt t; __BIGINT_INTERNAL_LINIT__(&t, a->n + b->n);
    __BIGINT_MUL_DISPATCH__(&t, a, b);
    __BIGINT_MONT_REDC__(&t, ctx, res);
}


void __BIGINT_CLASSICAL_MODMUL__() {}


