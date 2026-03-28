#ifndef ____DNML_AARCH64_CONN
#define ____DNML_AARCH64_CONN


#include "../../system/arch.h"
#include "../../system/compiler.h"

#if __ARCH_ARM64__
// _arm64_arith.S
extern uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _arm64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
// _arm64_modop.S
extern uint64_t _arm64_modinv64(uint64_t x);
extern uint64_t _arm64_modmul64(uint64_t a, uint64_t b, uint64_t mod);
extern uint64_t _arm64_modexp64(uint64_t base, uint64_t exp, uint64_t mod);
// _arm64_utils.S

#endif



#endif