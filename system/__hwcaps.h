#ifndef ___DNML_CAPS_H___
#define ___DNML_CAPS_H___


//* ----------- INCLUDES & MACROS ----------- *//
#include "__arch.h"
#include "__compiler.h"
#include "asm/__sys_conn.h"
#include <stdint.h>
#include <string.h>

#if defined(__linux__) || defined(__linux) 
    #include <sys/syscall.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
    #include <sys/aux.h>
#elif __compiler_msvc || defined(_WIN32)
    #include <intrin.h>
    #include <windows.h>
#endif

// Bit definitions for CPUID
#define bit_SSE42 (1 << 20)
#define bit_ABM (1 << 5)
#define bit_BMI1 (1 << 3)


//* ------- TYPE & OBJECT DECLARATIONS ------- *//
typedef struct {
    // RISC-V EXTENSIONS
    uint8_t rv64_zbb;
    uint8_t rv64_zba;
    uint8_t rv64_zbs;
    // X86_64 EXTENSIONS
    uint8_t x86_abm;
    uint8_t x86_bmi1;
    uint8_t x86_sse4_2;
    // AARCH64 EXTENSIONS
} _dnml_hwcaps;
extern _dnml_hwcaps libdnml_caps;

//* --------- BARE-METAL MANUAL FLAGS --------- *//
// RISC-V 64 bit Flags
#define _DNML_BARE_RISCV_ZBB 0
#define _DNML_BARE_RISCV_ZBA 0
#define _DNML_BARE_RISCV_ZBS 0
// x86_64 Flags
#define _DNML_BARE_X86_ABM 0
#define _DNML_BARE_X86_BMI1 0
#define _DNML_BARE_X86_SSE4_2 0
// ARM64/AARCH64 Flags


//* --------- EXTENSION DETECTION FUNCTIONS --------- *//
// RV64 -- ZBB + ZBA + ZBS detections
static inline void ___rv64_LINUX_detcaps(void) {
#if defined(__linux__) || defined(__linux)
    struct riscv_hwprobe pairs[] = {
        { .key = RISCV_HWPROBE_KEY_IMA_EXIT_0 }
    }; long ret = syscall(__NR_riscv_hwprobe, pairs, 1, 0, NULL, 0);
    if (!ret) {
        uint64_t ext = pairs[0].value;
        libdnml_caps.rv64_zbb = (ext & RISCV_HWPROBE_EXT_ZBB) != 0;
        libdnml_caps.rv64_zba = (ext & RISCV_HWPROBE_EXT_ZBA) != 0;
        return;
    }
#endif
}
static inline void ___rv64_legLINUX_detcaps(void) {
#if defined(__linux__) || defined(__linux)
    FILE *f = fopen("/proc/cpuinfo", "r");
        if (f) {
            char curr_block[256];
            while (fgets(curr_block, sizeof(curr_block), f)) {
                if (strncmp(curr_block, "isa", 3) == 0) {
                    libdnml_caps.rv64_zbb = 
                        strstr(curr_block, "_zbb") != NULL ||
                        strstr(curr_block, "zbb")  != NULL;
                    libdnml_caps.rv64_zba = strstr(curr_block, "_zba") != NULL;
                    break;
                }
            }
            fclose(f);
        }
#endif
}
static inline void ___rv64_BSDs_detcaps(void) {
#if defined(__FreeBSD__) || defined(__OpenBSD__)
    unsigned long hwcap = 0;
    elf_aux_info(AT_HWCAP, &hwcap, sizeof(hwcap));
    libdnml_caps.rv64_zbb = (hwcap & HWCAP_ISA_B) != 0;
#endif
}
static inline void __DNML_DETRV64_HWCAPS(void) {
#if defined(__linux__) || defined(__linux)
    // Modern Linux detection
    #if defined(__NR_riscv_hwprobe)
        ___rv64_LINUX_detcaps(); return;
    #endif // Legacy Linux detection
    ___rv64_legLINUX_detcaps();
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
    ___rv64_BSDs_detcaps();
#else // Bare metal, Embedded systems
    // Users are forced to manually set 
    // the _DNMLEMB_RISCV_ZBB flag
    libdnml_caps.rv64_zbb = _DNML_BARE_RISCV_ZBB;
    libdnml_caps.rv64_zba = _DNML_BARE_RISCV_ZBA;
    libdnml_caps.rv64_zbs = _DNML_BARE_RISCV_ZBS;
#endif
}


// x86_64 -- ABM + BMI1 detections
static inline void ___x86_WIN64_detcaps(void) {
#if __compiler_msvc || defined(_WIN32)
    int cpuInfo[4] = {0};
    // BMI1 Checking
    __cpuid(cpuInfo, 7) // Scan Page 0x00000007
    libdnml_caps.x86_bmi1 = (cpuInfo[1] & (1 << 3)) != 0;
    // ABM Checking
    __cpuid(cpuInfo, 0x80000001);
    libdnml_caps.x86_abm = (cpuInfo[2] & (1 << 5)) != 0;
    libdnml_caps.x86_sse4_2 = IsProcessorFeaturePresent(
        PF_SSE4_2_INSTRUCTIONS_AVAILABLE
    );
#endif
}
static inline void ___x86_Pdetcaps(void) {
#if defined(__linux__) || defined(__linux)
    unsigned int det_registers[4] = {0};
    unsigned int max_instruct = 0;
    // SSE4.2 Detection
    max_instruct = __sysv_cpu_maxl();
    __sysv_get_cpuid(1, 0, det_registers);
    libdnml_caps.x86_sse4_2 = (det_registers[2] & bit_SSE42) != 0;
    // ABM Detection
    if (max_instruct >= 0x80000001) {
        __sysv_get_cpuid(0x80000001, 0, det_registers);
        libdnml_caps.x86_abm = (det_registers[2] & bit_ABM) != 0;
    } else libdnml_caps.x86_abm = 0;
    if (max_instruct >= 7) {
        __sysv_get_cpuid(7, 0, det_registers);
        libdnml_caps.x86_bmi1 = (det_registers[1] & bit_BMI1) != 0;
    } else libdnml_caps.x86_bmi1 = 0;
#endif
}
static inline void __DNML_DETX64_HWCAPS(void) {
// COMPILER INTRINSICS --- GCC / Clang
#if __compiler_clang || __compiler_gcc
    libdnml_caps.x86_sse4_2 = __builtin_cpu_supports("sse4.2");
    libdnml_caps.x86_bmi1 = __builtin_cpu_supports("bmi");
    libdnml_caps.x86_abm = __builtin_cpu_supports("abm");
#elif __compiler_msvc || defined(_WIN32)
    ___x86_WIN64_detcaps();
#elif __ABI_X64_SYSV__ // System-V - Any other OS
    ___x86_Pdetcaps();
#else // BARE METAL - Unknown ABI
    libdnml_caps.x86_abm = _DNML_BARE_X86_ABM;
    libdnml_caps.x86_bmi1 = _DNML_BARE_X86_BMI1;
    libdnml_caps.x86_sse4_2 = _DNML_BARE_X86_SSE4_2;
#endif
}


// ARM64 -- detections
static inline void __DNML_DETARM64_HWCAPS(void) {}

// Detection main + dispatch
static inline void _libdnml_detect_hwcaps(void) {
    #if __ARCH_X86_64__
        __DNML_DETX64_HWCAPS();
    #elif __ARCH_ARM64__
        __DNML_DETARM64_HWCAPS();
    #elif __ARCH_RVI64__
        __DNML_DETRV64_HWCAPS();
    #endif
}





#endif