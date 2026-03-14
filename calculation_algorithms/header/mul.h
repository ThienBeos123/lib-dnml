#ifndef DNML_MUL_H
#define DNML_MUL_H



#ifdef __cplusplus
extern "C" {
#endif


#include "../../system/include.h"
#include "../../system/compiler.h"
#include "../../sconfigs/numeric_config.h"
#include "../atypes/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"


/* BIGINT WORKSPACE SIZE */
size_t __BIGINT_KARATSUBA_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_TOOM_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_SSA_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size);

/* BIGINT ALGORITHMS */
void __BIGINT_SCHOOLBOOK__(const bigInt *a, const bigInt *b, bigInt *res);
void __BIGINT_KARATSUBA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx karat_ctx);
void __BIGINT_TOOM__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_SSA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx);
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx);


#ifdef __cplusplus
}
#endif



#endif
