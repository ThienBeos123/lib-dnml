#ifndef ___DNML_ABI_H___
#define ___DNML_ABI_H___


#include "__arch.h"


// ARM64/AArch64 ABI
#if __ARCH_ARM64__
    #define __ABI_AARCH64__ 1
#else
    #define __ABI_AARCH64__ 0
#endif

// X86_64 ABIs
#if __ARCH_X86_64__
    // Win64 ABI
    #if defined(_WIN64)
        #define MY_ABI_WIN64 1
    // System-V ABI
    #elif defined(__unix__) || defined(__unix) || defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        #define MY_ABI_SYSTEM_V 1
    // Unknown ABI - Fallback to Vanilla C
    #else 
        #define __ABI_X64_WIN64__ 0
        #define __ABI_X64_SYSV__ 0
    #endif
#else
    #define __ABI_X64_WIN64__ 0
    #define __ABI_X64_SYSV__ 0
#endif

// RISC-V64 / RV64 ABI
#if __ARCH_RVI64__
    #define __ABI_LP64__ 1
#else
    #define __ABI_LP64__ 0
#endif





#endif