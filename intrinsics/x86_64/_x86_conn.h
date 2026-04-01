#ifndef ____DNML_X86_CONN
#define ____DNML_X86_CONN
#if __ARCH_X86_64__


#include "../__asm_macros.h"
#include "../../system/compiler.h"
#include "../../system/arch.h"

//* -------- SYSTEM-V ABI ---------- *//
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
extern uint64_t _x86sv_modinv64(uint64_t x);
#define _x86_modinv64   _x86sv_modinv64
// _x86_sysv_alg.S
// _x86_sysv_bitops.S
extern uint64_t _x86sv_clz64(uint64_t x);
extern uint64_t _x86sv_ctz64(uint64_t x);
extern uint64_t _x86sv_bswap64(uint64_t x);
#define _x86_clz64      _x86sv_clz64
#define _x86_ctz64      _x86sv_ctz64
#define _x86_bswap64    _x86sv_bswap64


//* -------- WINDOW-64 ABI ---------- *//
#elif __ABI_X64_WIN64__
// _x86_win64_arith.S
extern uint64_t _x86w64_add64c(uint64_t a, uint64_t b, uint8_t *carry);
extern uint64_t _x86w64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
extern uint64_t _x86w64_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
extern uint64_t _x86w64_wdiv128(
    uint64_t lo, uint64_t hi,
    uint64_t div, uint64_t *rem
);
#define _x86_add64c     _x86w64_add64c
#define _x86_sub64b     _x86w64_sub64b
#define _x86_wmul128    _x86w64_wmul128
#define _x86_wdiv128    _x86w64_wdiv128
// _x86_win64_marith.S
extern uint64_t _x86w64_modinv64(uint64_t x);
#define _x86_modinv64   _x86w64_modinv64
// _x86_win64_alg.S
// _x86_win64_bitops.S
extern uint64_t _x86w64_clz64(uint64_t x);
extern uint64_t _x86w64_ctz64(uint64_t x);
extern uint64_t _x86w64_bswap64(uint64_t x);
#define _x86_clz64      _x86w64_clz64
#define _x86_ctz64      _x86w64_ctz64
#define _x86_bswap64    _x86w64_bswap64


//* -------- UNKNOWN ABI ---------- *//
#else
#include "../zvanillc/_vanillc_conn.h"
#define _x86_add64c     _cintrin_add64c
#define _x86_sub64b     _cintrin_sub64b
#define _x86_wmul128    _cintrin_wmul128
#define _x86_wmul128    _cintrin_wdiv128
#define _x86_modinv64   _cintrin_modinv64
#define _x86_clz64      _cintrin_clz64
#define _x86_ctz64      _cintrin_ctz64
#define _x86_bswap64    _cintrin_bswap64
#endif

#endif

#endif