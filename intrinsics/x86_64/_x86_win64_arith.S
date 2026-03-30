#include "_x86_conn.h"
#if __ARCH_X86_64__
#if __ABI_X64_WIN64__


asm_text_section
.intel_syntax   noprefix
.globl	        _x86w64_add64c
.globl          _x86w64_sub64b
.globl          _x86w64_wmul128
.globl          _x86w64_wdiv128

.type	        _x86w64_add64c, %function
.type           _x86w64_sub64b, %function
.type           _x86w64_wmul128, %function
.type           _x86w64_wdiv128, %function
.p2align        4, 0x90



_x86w64_add64c:
    ret


_x86w64_sub64b:
    ret


_x86w64_wmul128:
    ret


_x86w64_wdiv128:
    ret


#endif
#endif
