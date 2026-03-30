#include "_x86_conn.h"
#if __ARCH_X86_64__


.section	    __TEXT,__text,regular,pure_instructions
.intel_syntax   noprefix
.globl	        _x86_add64c
.globl          _x86_sub64b
.globl          _x86_wmul128
.globl          _x86_wdiv128
.p2align	    4, 0x90


/* ------- 64 BIT ADDITION WITH CARRY -------
* uint64_t _x86_add64c(uint64_t a, uint64_t b, uint8_t *carry)
* arg: 
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
*   rdx: uint8_t *carry   (8  BIT)
* ret: 
*   rax: (a + b + *carry) mod(2^64)
*   rdx: boolean - 1/0
*/
_x86_add64c:
    mov rax, qword ptr [rdx] # Load *carry into rax
    add rax, -1 # rax + UINT64_MAX:
    # +) If rax = 1 ---> UINT64_MAX + 1 overflows --> C flag set (carry)
    # +) If rax = 0 ---> UINT64_MAX + 0 is normal --> C flag not set (no carry)
    # Add a = a + b + *carry
    # --> rdi = rdi + rsi + C flag
    adc rdi, rsi

    xor rax, rax # Clearing out rax
    setc    al # Set rax/al based on C flag
    movzx rax, al # Zero-extends al into rcx

    mov [rdx], rax # *carry/*rdx = rax
    mov rax, rdi # return rdi
    ret


/* ------- 64 BIT SUBTRACTION WITH BORROW -------
* uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow)
* arg: 
*   rdi: uint64_t a       (64 BIT)
*   rsi: uint64_t b       (64 BIT)
*   rdx: uint8_t *borrow  (8  BIT)
* ret: 
*   rax: (a - b - *borrow) mod(2^64)
*   rdx: boolean - 1/0
*/
_x86_sub64b:
    mov rax, qword ptr [rdx] # Load *borrow into rax
    bt  rax, 0 # Copies rax_bits[0] into C_flag
    # Subtract a = a - b - *borrow
    # -------> rdi = rdi - rsi - C flag
    sbc rdi, rsi

    xor rax, rax # Clearing out rax
    setc    al # Set rax/al based on C flag
    movzx rax, al # Zero-extends al into rax
    
    mov [rdx], rax # *carry/*rdi = rax
    mov rax, rdi # return rdi
    ret


_x86_wmul128:
    ret


_x86_wdiv128:
    ret


#endif