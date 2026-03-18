#ifndef DNML_CONV_MACROS
#define DNML_CONV_MACROS




#include "../system/compiler.h"
#include "../big_numbers/bigNums.h"
#include "../sconfigs/_ctx.h"
#include "../sconfigs/arena.h"
#include "../sconfigs/numeric_config.h"


#define BIGINT_TEMP(name, limb_count, ctx) \
    limb_t *name##_limbs = scratch_alloc(&(ctx), (limb_count) * BYTES_IN_UINT64_T); \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};




#endif