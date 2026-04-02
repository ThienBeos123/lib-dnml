#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_SYSV__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86sv_clz64e
.globl          _x86sv_clz64s
.globl          _x86sv_ctz64e
.globl          _x86sv_ctz64s
.globl          _x86sv_bswap64

.type	        _x86sv_clz64e, %function
.type           _x86sv_clz64s, %function
.type           _x86sv_ctz64e, %function
.type           _x86sv_ctz64s, %function
.type           _x86sv_bswap64, %function
.p2align	    4, 0x90


/* ------- COUNT LEADING ZERO 64 BIT - ABM-EXTENDED -------
* uint8_t _x86sv_clz64e(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: lzcnt(x) ------> (8  BIT)
*/
_x86sv_clz64e:
    lzcnt rax, rdi
    ret
/* ------- COUNT LEADING ZERO 64 BIT - 80386-STANDARD -------
* uint8_t _x86sv_clz64s(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: bsr(x) ------> (8  BIT)
*/
_x86sv_clz64s:
    bsr rax, rdi
    ret


/* ------- COUNT TRAILING ZERO 64 BIT - BMI1-EXTENDED -------
* uint8_t _x86sv_ctz64e(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: tzcnt(x) ------> (8  BIT)
*/
_x86sv_ctz64e:
    tzcnt rax, rdi
    ret
/* ------- COUNT TRAILING ZERO 64 BIT - 80386-STANDARD -------
* uint8_t _x86sv_ctz64s(uint64_t x)
* arg: rdi: uint64_t x --> (64 BIT)
* ret: rax: bsf(x) ------> (8  BIT)
*/
_x86sv_ctz64s:
    bsf rax, rdi
    ret

/* ------- BYTE SWAP 64 BIT -------
* uint8_t _x86sv_bswap64(uint64_t x)
* arg: rdi: uint64_t x ----> (64 BIT)
* ret: rax: bswap(x) ------> (8  BIT)
*/
_x86sv_bswap64:
    bswap   rdi
    mov rax, rdi
    ret


#endif
#endif