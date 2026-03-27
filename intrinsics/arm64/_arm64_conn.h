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

// _arm64_utils.S

#endif



#endif