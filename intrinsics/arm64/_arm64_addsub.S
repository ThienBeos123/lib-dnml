#include "_arm64_conn.h"
#if __ARCH_ARM64__

.section    __TEXT,__text,regular,pure_instructions
.global     _arm64_add64c
.type       _arm64_add64cc, @function
.global     _arm64_sub64b
.type       _arm64_sub64b, @function
.align      2

/*
*   +) uint64_t _arm64_add64c(uint64_t a, uint64_t b, uint8_t *carry) 
*   +) Functionality:
*       -----> Sum (mod 2^64): return a + b + *carry
*       -----> Carry: (sum > 2^64 - 1) ? (1) : (0)
*/
_arm64_add64c:
    /*
    *   Function parameter lists:
    *       x0: uint64_t a ----> (64 BIT)
    *       x1: uint64_t b ----> (64 BIT)
    *       x2: uint8_t *carry -> (8 BIT)
    *   Function returns:
    *       x0: RETURNED AS UINT64_T / 64 BIT
    */
    ldr x3, [x2] // Load *carry into register x3
    cmp x3, #1   // Sets the C flag to 1 (NO-BORROW) if carry = 1

    // 1. Add a = a + b + carry
    // -----> x0 = x0 + x1 + C_flag
    // 2. Set carry flag (Automatic by ADCS)
    adcs    x0, x0, x1
    adc x3, xzr, xzr
    str x3, [x2]
    ret

/*
*   +) uint64_t _arm64_sub64b(uint64_t a, uint64_t b, uint8_t *borrow) 
*   +) Functionality:
*       -----> Difference (mod 2^64): return a - b - *borrow
*       -----> Borrow: (diff < 0) ? (1) : (0)
*/
_arm64_sub64b:
    /*
    *   Function parameter lists:
    *       x0: uint64_t a ----> (64 BIT)
    *       x1: uint64_t b ----> (64 BIT)
    *       x2: uint8_t *borrow -> (8 BIT)
    *   Function returns:
    *       x0: RETURNED AS UINT64_T / 64 BIT
    */
    ldr x3, [x2] // Load *carry into register x3
    subs xzr, xzr, x0 // Sets the C flag to 0 (BORROW) if borrow = 1

    // 1. Subtract a = a - b - carry
    // ---------> x0 = x0 - x1 - C_flag
    sbcs    x0, x0, x1
    sbc x3, #1, xzr
    str x3, [x2]
    ret

#endif