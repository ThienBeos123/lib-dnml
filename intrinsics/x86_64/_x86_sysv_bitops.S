#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


.section	    __TEXT,__text,regular,pure_instructions
.intel_syntax   noprefix
.globl	        _x86sv_clz64
.globl          _x86sv_ctz64
.globl          _x86sv_bswap64
.p2align	    4, 0x90


/* ------- COUNT LEADING ZERO 64 BIT -------
* uint8_t _x86sv_clz64(uint64_t x)
* arg: rax: uint64_t x --> (64 BIT)
* ret: rax: clz(x) ------> (8  BIT)
*/
_x86sv_clz64:
    lzcnt rax, rax
    ret


/* ------- COUNT TRAILING ZERO 64 BIT -------
* uint8_t _x86sv_ctz64(uint64_t x)
* arg: rax: uint64_t x --> (64 BIT)
* ret: rax: ctz(x) ------> (8  BIT)
*/
_x86sv_ctz64:
    tzcnt rax, rax
    ret


/* ------- BYTE SWAP 64 BIT -------
* uint8_t _x86sv_bswap64(uint64_t x)
* arg: rax: uint64_t x ----> (64 BIT)
* ret: rax: bswap(x) ------> (8  BIT)
*/
_x86sv_bswap64:
    bswap rax
    ret


#endif
#endif