#ifndef DNML_POW_ROOT_H
#define DNML_POW_ROOT_H


#ifdef __cplusplus
extern "C" {
#endif



#include "../../system/include.h"
#include "../../system/compiler.h"
#include "../../sconfigs/arena.h"
#include "../../sconfigs/numeric_config.h"
#include "../../sconfigs/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

#include "add_sub.h"
#include "div.h"
#include "mul.h"


/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, size_t pow);
size_t __BIGINT_2K_ARY_WS__(size_t base_size, size_t pow);
size_t __BIGINT_SLIDIN_WS__(size_t base_size, size_t pow);
size_t __BIGINT_NEWTSQRT_WS__(size_t a_size, size_t root_size);
size_t __BIGINT_NEWT_NROOT_WS__(size_t a_size, size_t root_size);


/* ----------------- ALGORITHMS ----------------- */
void __BIGINT_BINARY_EXP__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx binexp_ctx);
void __BIGINT_2K_ARY__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx binexp_ctx);
void __BIGINT_SLIDING__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx slide_ctx);
void __BIGINT_MONT_LADDER__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx ladder_ctx);
void __BIGINT_NEWTON_SQRT__(bigInt *res, const bigInt *a, const bigInt *root);
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, const bigInt *root);



#ifdef __cplusplus
}
#endif


#endif
