#ifndef ____DNML_AARCH64_CONN
#define ____DNML_AARCH64_CONN


#include "../../system/arch.h"
#include "../../system/compiler.h"

#if __ARCH_ARM64__
extern uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
#endif



#endif