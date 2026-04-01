#ifndef ____DNML_RVI64_CONN
#define ____DNML_RVI64_CONN

#include "../__asm_macros.h"
#include "../../system/compiler.h"
#include "../../system/arch.h"

#if __ARCH_RVI64__
// _rvi64_arith.S
extern uint64_t _rv64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _rv64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _rv64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
// _rvi64_marith.S
// _rvi64_alg.S
// _rvi64_bitops.S
#endif



#endif