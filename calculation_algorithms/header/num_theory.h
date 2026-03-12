#ifndef DNML_NUM_THEORY_H
#define DNML_NUM_THEORY_H



#ifdef __cplusplus
extern "C" {
#endif

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
#include "mod_op.h"

//todo ============================== GREATEST COMMON DIVISORS ============================== todo//
uint64_t __BIGINT_EUCLID__(uint64_t u, uint64_t v);
void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v);
void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v);
void __BIGINT_HALF__(bigInt *res, const bigInt *u, const bigInt *v);
void __BIGINT_GCD_DISPATCH__(bigInt *res, const bigInt *u, const bigInt *v);




//todo ================================== PRIMALITY TESTING ================================== todo//
uint8_t __BIGINT_TRIAL_DIV__(uint64_t x);
uint8_t __BIGINT_SMALL_MRABIN__(uint64_t x);
uint8_t __BIGINT_MILLER_RABIN__(const bigInt *x, const bigInt* base);
uint8_t __BIGINT_BPSW__(const bigInt *x);
uint8_t __BIGINT_ECPP__(const bigInt *x);
uint8_t __BIGINT_PTEST_DISPATCH__(const bigInt *x);


#ifdef __cplusplus
}
#endif




#endif