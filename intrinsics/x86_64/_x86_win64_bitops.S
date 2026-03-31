#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86w64_clz64:
.globl          _x86w64_ctz64
.globl          _x86w64_bswap64

.type	        _x86sv_clz64, %function
.type           _x86w64_ctz64, %function
.type           _x86w64_bswap64, %function
.p2align	    4, 0x90


/* ------- COUNT LEADING ZERO 64 BIT -------
* uint8_t _x86w64_clz64(uint64_t x)
* arg: rcx: uint64_t x --> (64 BIT)
* ret: rax: clz(x) ------> (8  BIT)
*/
_x86w64_clz64:
    lzcnt rax, rcx
    ret


/* ------- COUNT TRAILING ZERO 64 BIT -------
* uint8_t _x86w64_ctz64(uint64_t x)
* arg: rcx: uint64_t x --> (64 BIT)
* ret: rax: ctz(x) ------> (8  BIT)
*/
_x86w64_ctz64:
    tzcnt rax, rcx
    ret


/* ------- BYTE SWAP 64 BIT -------
* uint8_t _x86w64_bswap64(uint64_t x)
* arg: rcx: uint64_t x ----> (64 BIT)
* ret: rax: bswap(x) ------> (8  BIT)
*/
_x86w64_bswap64:
    bswap   rcx
    mov rax, rcx
    ret


#endif
#endif