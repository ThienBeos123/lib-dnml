#ifndef ____DNML_SYS_CONN
#define ____DNML_SYS_CONN

#include "__asm_macros.h"

extern void __sys_get_cpuid(
    unsigned int leaf,
    unsigned int subleaf,
    unsigned int registers[4]
);
extern unsigned int __sys_cpu_maxl(void);




#endif