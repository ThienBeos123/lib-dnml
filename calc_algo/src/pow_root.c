#include "../header/pow_root.h"


/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, size_t pow_size) {}
size_t __BIGINT_SLIDIN_WS__(size_t base_size, size_t pow_size) {}
size_t __BIGINT_NEWTSQRT_WS__(size_t a_size, size_t root_size) {}
size_t __BIGINT_NEWT_NROOT_WS__(size_t a_size, size_t root_size) {}



/* ----------------- ALGORITHMS ----------------- */
void __BIGINT_BINARY_EXP__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx binexp_ctx) {
    // SETUP
    size_t binexp_mark = scratch_mark(&binexp_ctx);
    BIGINT_TEMP(tmp_res, base->n * power, binexp_ctx);
    tmp_res.limbs[0] = 1; tmp_res.n = 1;
    BIGINT_TEMP(tmp_base, base->n * power, binexp_ctx);
    memcpy(tmp_base.limbs, base->limbs, base->n * BYTES_IN_UINT64_T);
    tmp_base.n = base->n;

    // MAIN OPERATION
    while (power) { 
        if (power & 1) { 
            __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_base, &tmp_res, binexp_ctx); 
        } __BIGINT_MUL_DISPATCH__(&tmp_base, &tmp_base, &tmp_base, binexp_ctx); 
        power >>= 1;
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); 
    scratch_reset(&binexp_ctx, binexp_mark);
}
void __BIGINT_SLIDING_WINDOW_EXP__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx slide_ctx) {}
void __BIGINT_NEWTON_SQRT__(bigInt *res, const bigInt *a, const bigInt *root) {}
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, const bigInt *root) {}