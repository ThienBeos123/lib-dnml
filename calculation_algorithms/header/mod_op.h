#ifndef DNML_MOD_OP_H
#define DNML_MOD_OP_H




#include "../../system/include.h"
#include "../../system/compiler.h"
#include "../../sconfigs/arena.h"
#include "../../sconfigs/numeric_config.h"
#include "../atypes/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

#include "div.h"
#include "mul.h"
#include "mod.h"

void __BIGINT_MONTMUL__(
    const bigInt *a, const bigInt *b, 
    mont_ctx ctx, bigInt *res
);
void __BIGINT_CLASSICAL_MODMUL__(
    const bigInt *x, const bigInt *b, 
    const bigInt *modulus, bigInt *res
);
void __BIGINT_BINARY_MODEXP__(
    bigInt *res, const bigInt *modulus, 
    const bigInt *base, const bigInt *power
);






#endif