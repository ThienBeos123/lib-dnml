#ifndef __DNML_CTX__
#define __DNML_CTX__

#include <stdalign.h>
#include "../system/__include.h"
#include "../../adynamol/big_numbers/bigNums.h"

typedef struct {
    const bigInt* n;
    limb_t nprime;
    const bigInt* r2;
    size_t k;
} mont_ctx;

typedef struct {
    void* (*alloc)(void *state, size_t size);
    size_t (*mark)(void *state);
    void (*reset)(void *state, size_t mark);
    void *state;
} calc_ctx;
static inline void* scratch_alloc(calc_ctx *ctx, size_t n) { return ctx->alloc(ctx->state, n); }
static inline size_t scratch_mark(calc_ctx *ctx) { return ctx->mark(ctx->state); }
static inline void scratch_reset(calc_ctx *ctx, size_t mark) { ctx->reset(ctx->state, mark); }


#endif