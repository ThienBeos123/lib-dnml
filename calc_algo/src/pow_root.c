#include "../header/pow_root.h"





/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, size_t pow) {
    size_t raw = base_size * pow;
    size_t fcall_size = __BIGINT_MUL_WS__((raw - base_size), (raw - base_size));
    return raw + fcall_size;
}
size_t __BIGINT_2K_ARY_WS__(size_t base_size, size_t pow, uint8_t ksize) {
    // RAW OBJECTS
    size_t table_size = 1 << (ksize - 1);
    size_t raw_outside = (base_size << 1) + (base_size * pow);
    size_t table_pows = (base_size * table_size) * (table_size + 1);
    // FUNCTION CALL WORKSPACE
    size_t outside_fcalls = __BIGINT_MUL_WS__(base_size, base_size);
    size_t table_loop_fcalls = __BIGINT_MUL_WS__((base_size * (((table_size - 1) << 1) + 1)), (base_size << 1));
    size_t main_loop_even = __BIGINT_MUL_WS__((base_size * pow - base_size), (base_size * pow - base_size));
    size_t main_loop_odd = __BIGINT_MUL_WS__(
        (base_size * pow - base_size), 
        (base_size * ((table_size << 1) + 1))
    ); size_t max_fcall = max(outside_fcalls,
                              max(table_loop_fcalls,
                                  max(main_loop_even, main_loop_odd)));
    return raw_outside + table_pows + max_fcall;
}
size_t __BIGINT_SLIDIN_WS__(size_t base_size, size_t pow, uint8_t ksize) {
    // RAW OBJECTS
    size_t table_size = 1 << (ksize - 1);
    size_t raw_outside = (base_size << 1) + (base_size * pow);
    size_t table_pows = (base_size * table_size) * (table_size + 1);
    // FUNCTION CALL WORKSPACE
    size_t outside_fcall = __BIGINT_MUL_WS__(base_size, base_size);
    size_t table_loop_fcalls = __BIGINT_MUL_WS__((base_size * (((table_size - 1) << 1) + 1)), (base_size << 1));
    size_t main_loop_even = __BIGINT_MUL_WS__((base_size * pow - base_size), (base_size * pow - base_size));
    size_t main_loop_odd = __BIGINT_MUL_WS__(
        (base_size * pow - base_size),
        (base_size * ((table_size << 1) + 1))
    ); size_t max_fcall = max(outside_fcall,
                              max(table_loop_fcalls,
                                  max(main_loop_even, main_loop_odd)));
    return raw_outside + table_pows + max_fcall;
}
size_t __BIGINT_HERON_WS__(size_t a_size) {
    size_t raw_size = a_size << 2;
    size_t fcall = __BIGINT_DIVMOD_WS__(a_size, a_size);
    return raw_size + fcall;
}
size_t __BIGINT_NEWTON_NROOT_WS__(size_t a_size, size_t root_size) {}



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
void __BIGINT_2K_ARY__(bigInt *res, const bigInt *base, uint64_t power, uint8_t k, calc_ctx fixed_ctx) {
    //* --- 1. SETUP ---- *//
    size_t table_size = 1 << (k - 1), _2kary_mark = scratch_mark(&fixed_ctx);
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
    uint64_t mask = (1ULL << k) - 1; uint8_t curr_shift = 0;
    for (uint8_t i = chunk_count - 1; i >= 0; --i) {
        curr_shift = BITS_IN_UINT64_T - k * (chunk_count - i - 1);
        uint64_t curr_chunk = power & (mask << curr_shift);
        curr_chunk = curr_chunk >>= curr_shift; curr_chunk = (uint8_t)(curr_chunk);
        uint8_t s = __CTZ_UI64__(curr_chunk); curr_chunk >>= s;

        for (uint8_t j = 1; j <= chunk_count - s; ++j) {
            __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, fixed_ctx);
        } __BIGINT_MUL_DISPATCH__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, fixed_ctx);
        for (uint8_t j = 1; j <= s; ++j) __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, fixed_ctx);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); scratch_reset(&fixed_ctx, _2kary_mark);
}
void __BIGINT_SLIDING__(bigInt *res, const bigInt *base, uint64_t power, uint8_t k, calc_ctx slide_ctx) {
    //* --- 1. SETUP ---- *//
    size_t table_size = 1 << (k - 1), slidin_mark = scratch_mark(&slide_ctx);
    bigInt table[table_size]; table[0] = *base;
    BIGINT_TEMP(x2, (base->n << 1), slide_ctx);
    __BIGINT_MUL_DISPATCH__(base, base, &x2, slide_ctx);

    //* ---- 2. TABLE PRECOMPUTATION ----- *//
    for (size_t i = 1; i < table_size; ++i) {
        table[i].limbs = scratch_alloc(&slide_ctx, base->n * ((i << 1) + 1) * BYTES_IN_UINT64_T);
        table[i].cap   = base->n * ((i << 1) + 1); 
        table[i].sign  = 1;
        __BIGINT_MUL_DISPATCH__(&table[i-1], &x2, &table[i], slide_ctx);
    }

    //* ------------ 3. MAIN LOOP ------------ *//
    BIGINT_TEMP(tmp_res, base->n * power, slide_ctx);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; tmp_res.sign = 1;
    uint8_t curr_pos = BITS_IN_UINT64_T - (__CLZ_UI64__(power)) - 1;
    uint64_t mask = (1ULL << k) - 1;
    while (curr_pos >= 0) {
        uint8_t curr_bit = power & (1ULL << curr_pos);
        if (!curr_bit) { __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, slide_ctx); --curr_pos; } 
        else {
            uint8_t s = max(curr_pos - k + 1, 0);
            uint64_t curr_chunk = power & (mask << curr_pos - (k - 1));
            curr_chunk >>= curr_pos - (k - 1); curr_chunk = (uint8_t)(curr_chunk);
            uint8_t tz = __CTZ_UI64__(curr_chunk); s -= tz; curr_chunk >>= tz;
            for (uint8_t i = 0; i <= curr_bit - s; ++i) {
                __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, slide_ctx);
            } __BIGINT_MUL_DISPATCH__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, slide_ctx);
            curr_pos = s - 1;
        }
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); 
    scratch_reset(&slide_ctx, slidin_mark);

}
void __BIGINT_HERON__(bigInt *res, const bigInt *a, calc_ctx heron_ctx) {
    uint64_t guess_bits =  (__BIGINT_COUNTDB__(a, 2) + 1) >> 1;
    size_t heron_mark = scratch_mark(&heron_ctx);
    BIGINT_TEMP(guess, a->n, heron_ctx); BIGINT_TEMP(a_guess, a->n, heron_ctx);
    guess.limbs[0] = guess_bits; guess.n = 1; guess.sign = 1;
    BIGINT_TEMP(tmp, a->n, heron_ctx); BIGINT_TEMP(next, a->n, heron_ctx);
    while (true) {
        __BIGINT_DIVMOD_DISPATCH__(a, &guess, &a_guess, &tmp, heron_ctx);
        __BIGINT_ADD_WC__(&next, &guess, &a_guess);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break;
        __BIGINT_INTERNAL_COPY__(&guess, &next);
    } __BIGINT_INTERNAL_COPY__(res, &guess); scratch_reset(&heron_ctx, heron_mark);
}
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, const bigInt *root) {}