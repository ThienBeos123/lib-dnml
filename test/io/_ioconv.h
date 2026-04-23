#ifndef _IOCONV_H
#define _IOCONV_H



#include "../../test_ui/_strui.h"
// STDLIB utilities
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define stinl static inline
#define cvoid const void
#define csres const str_res

#define scase _libdnml_scase
#define result str_res
#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t


stinl void _print_base(FILE* f, u8 base, bool endl) {
    switch (base) {
        case 1:     fputs("1 - (Unary)", f); break;
        case 2:     fputs("2 - (Binary)", f); break;
        case 3:     fputs("3 - (Ternary)", f); break;
        case 8:     fputs("8 - (Octal)", f); break;
        case 10:    fputs("10 - (Decimal)", f); break;
        case 16:    fputs("16 - (Hexadecimal)", f); break;
        default:    fprintf(f, "%" PRIu8 "", base); break;
    } if (endl) fputc('\n', f);
}






#endif