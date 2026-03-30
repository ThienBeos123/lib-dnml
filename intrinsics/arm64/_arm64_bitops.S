#include "_arm64_conn.h"
#if __ARCH_ARM64__


asm_text_section
.global     _arm64_clz64
.global     _arm64_ctz64
.global     _arm64_bswap64
.type       _arm64_clz64, %function
.type       _arm64_ctz64, %function
.type       _arm64_bswap64, %function
.align      2

/* ------- COUNT LEADING ZERO 64 BIT -------
* uint8_t _arm64_clz64(uint64_t x)
* arg: x0: uint64_t x --> (64 BIT)
* ret: x0: clz(x) ------> (8  BIT)
*/
_arm64_clz64:
    clz x0, x0
    ret


/* ------- COUNT TRAILING ZERO 64 BIT -------
* uint8_t _arm64_ctz64(uint64_t x)
* arg: x0: uint64_t x --> (64 BIT)
* ret: x0: ctz(x) ------> (8  BIT)
*/
_arm64_ctz64:
    ctz x0, x0
    ret


/* ------- BYTE SWAP 64 BIT -------
* uint8_t _arm64_bswap64(uint64_t x)
* arg: x0: uint64_t x ----> (64 BIT)
* ret: x0: bswap(x) ------> (8  BIT)
*/
_arm64_bswap64:
    rev x0, x0
    ret


#endif