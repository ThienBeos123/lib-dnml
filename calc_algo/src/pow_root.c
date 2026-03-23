#include "../header/pow_root.h"





/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, size_t pow) {
    size_t raw = base_size * pow;
    size_t fcall_size = __BIGINT_MUL_WS__((raw - base_size), (raw - base_size));
    return raw + fcall_size + (2 * alignof(max_align_t));
}
size_t __BIGINT_2K_ARY_WS__(size_t base_size, size_t pow) {
    // RAW OBJECTS
    size_t table_size = 1 << (4 - 1);
    size_t raw_outside = (base_size << 1) + (base_size * pow);
    size_t table_pows = (base_size * table_size) * (table_size + 1);
    // FUNCTION CALL WORKSPACE
    size_t outside_fcalls = __BIGINT_MUL_WS__(base_size, base_size);
    size_t first_loop_fcalls = __BIGINT_MUL_WS__((base_size * (((table_size - 1) << 1) + 1)), (base_size << 1));
    size_t main_loop_even = __BIGINT_MUL_WS__((base_size * pow - base_size), (base_size * pow - base_size));
    size_t main_loop_odd = __BIGINT_MUL_WS__(
        (base_size * pow - base_size), 
        (base_size * ((table_size << 1) + 1))
    ); size_t max_fcall = max(outside_fcalls,
                              max(first_loop_fcalls,
                                  max(main_loop_even, main_loop_odd)));
    return raw_outside + table_pows + max_fcall;
}
size_t __BIGINT_SLIDIN_WS__(size_t base_size, size_t pow) {}
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
void __BIGINT_2K_ARY__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx fixed_ctx) {
    //* --- 1. SETUP ---- *//
    size_t table_size = 1 << (4 - 1), _2kary_mark = scratch_mark(&fixed_ctx);
    bigInt table[table_size]; table[0] = *base;
    BIGINT_TEMP(x2, (base->n << 1), fixed_ctx);
    __BIGINT_MUL_DISPATCH__(base, base, &x2, fixed_ctx);

    //* ---- 2. TABLE PRECOMPUTATION ----- *//
    for (size_t i = 1; i < table_size; ++i) {
        table[i].limbs = scratch_alloc(&fixed_ctx, base->n * ((i << 1) + 1) * BYTES_IN_UINT64_T);
        table[i].cap   = base->n * ((i << 1) + 1); 
        table[i].sign  = 1;
        __BIGINT_MUL_DISPATCH__(&table[i-1], &x2, &table[i], fixed_ctx);
    }

    //* ----- 3. MAIN LOOP ------ *//
    uint8_t chunk_count = (uint8_t)((BITS_IN_UINT64_T - __CLZ_UI64__(power)) >> 2) + 1;
    BIGINT_TEMP(tmp_res, base->n * power, fixed_ctx);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; tmp_res.sign = 1;
    for (uint8_t i = chunk_count - 1; i >= 0; --i) {
        uint64_t curr_chunk = power & (((1ULL << 4) - 1) << (BITS_IN_UINT64_T - 4));
        curr_chunk = (uint8_t)(curr_chunk >> (BITS_IN_SIZE_T - 4));
        uint8_t s = __CTZ_UI64__(curr_chunk); curr_chunk >>= s;

        for (uint8_t j = 1; j <= chunk_count - s; ++j) {
            __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, fixed_ctx);
        } __BIGINT_MUL_DISPATCH__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, fixed_ctx);
        for (uint8_t j = 1; j <= s; ++j) __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, fixed_ctx);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); scratch_reset(&fixed_ctx, _2kary_mark);
}
void __BIGINT_SLIDING__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx slide_ctx) {}
void __BIGINT_MONT_LADDER__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx ladder_ctx) {}
void __BIGINT_NEWTON_SQRT__(bigInt *res, const bigInt *a, const bigInt *root) {}
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, const bigInt *root) {}