#ifndef DNML_MONT_CTX
#define DNML_MONT_CTX

#include "../../system/include.h"
#include "../../system/compiler.h"
#include "../../sconfigs/arena.h"
#include "../../sconfigs/numeric_config.h"

#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

typedef struct {
    const bigInt* n;
    limb_t nprime;
    const bigInt* r2;
    size_t k;
} mont_ctx;



#endif