#ifndef ____DNML_X86_CONN
#define ____DNML_X86_CONN
#if __ARCH_X86_64__

#include "../../system/compiler.h"
#include "../../system/arch.h"

#if __ABI_X64_SYSV__
// _x86_sysv_arith.S
extern uint64_t _x86sv_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _x86sv_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _x86sv_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
extern uint64_t _x86sv_wdiv128(
    uint64_t lo, uint64_t hi,
    uint64_t div, uint64_t *rem
);
#define _x86_add64c     _x86sv_add64c
#define _x86_sub64b     _x86sv_sub64b
#define _x86_wmul128    _x86sv_wmul128
#define _x86_wdiv128    _x86sv_wdiv128
// _x86_sysv_marith.S
// _x86_sys_valg.S
// _x86_sysv_bitops.S
#elif __ABI_X64_WIN64__
// _x86_win64_arith.S
// _x86_win64_marith.S
// _x86_win64_alg.S
// _x86_win64_bitops.S
#else
#include "../zvanillc/_vanillc_conn.h"
#define _x86_add64c     _cintrin_add64c
#define _x86_sub64b     _cintrin_sub64b
#define _x86_wmul128    _cintrin_wmul128
#define _x86_wmul128    _cintrin_wdiv128
#endif

#endif

#endif