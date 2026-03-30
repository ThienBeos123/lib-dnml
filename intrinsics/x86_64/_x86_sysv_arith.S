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
    mov rcx, qword ptr [rdx] # Load *carry into rcx
    add rcx, -1 # Add rcx to UINT64_MAX:
                #   If rax = 1 ---> UINT64_MAX + 1 overflows --> C flag set
                #   If rax = 0 ---> UINT64_MAX + 0 does't overflow --> doesn't set C flag
    # Add a = a + b + *carry
    # --> rdi = rdi + rsi + C flag
    adc rdi, rsi
    
    xor rcx, rcx # Clearing out rcx
    setc    cl # Set rcx/cl based on C flag
    movzx rcx, cl # Zero-extends cl into rcx

    mov [rdx], rcx # *carry/*rdi = rcx
    mov rax, rdi # return rdi
    ret


_x86_sub64b:
    ret


_x86_wmul128:
    ret


_x86_wdiv128:
    ret


#endif