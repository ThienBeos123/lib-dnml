#ifndef ____DNML_AARCH64_CONN
#define ____DNML_AARCH64_CONN
#if __ARCH_ARM64__


#include "../__asm_macros.h"
#include "../../system/__arch.h"
#include "../../system/__compiler.h"

#if __ARCH_ARM64__
// _arm64_arith.S
extern uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _arm64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
// _arm64_marith.S
extern uint64_t _arm64_modinv64(uint64_t x);
// _arm64_alg.S
// _arm64_bitops.S
extern uint64_t _arm64_clz64(uint64_t x);
extern uint64_t _arm64_ctz64(uint64_t x);
extern uint64_t _arm64_bswap64(uint64_t x);
#endif


#endif
#endif