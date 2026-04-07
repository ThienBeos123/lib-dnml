#ifndef DNML_DIV_H
#define DNML_DIV_H



#ifdef __cplusplus
extern "C" {
#endif

#include "../../system/__include.h"
#include "../../system/__compiler.h"
#include "../../sconfigs/arena.h"
#include "../../sconfigs/numeric_config.h"
#include "../../sconfigs/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../dutil/util.h"
#include "../../adynamol/big_numbers/bigNums.h"

#include "add_sub.h"
#include "mul.h"



size_t __BIGINT_SHORTDIV_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_KNUTH_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_NEWTON_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_DIVMOD_WS__(size_t a_size, size_t b_size);


void __BIGINT_SHORT_DIVISION__(const bigInt *a, uint64_t b, bigInt *quot, bigInt *rem);
void __BIGINT_KNUTH_D__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx knuth_ctx);
void __BIGINT_BURNIKEL__(
    const bigInt *AH, const bigInt *AL, 
    const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx burk_ctx
);
void __BIGINT_NEWTON__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx newton_ctx);
void __BIGINT_DIVMOD_DISPATCH__(
    const bigInt *a, const bigInt *b, 
    bigInt *quot, bigInt *rem, calc_ctx div_ctx
);




#ifdef __cplusplus
}
#endif


#endif
