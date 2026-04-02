#ifndef ___DNML_CAPS_H___
#define ___DNML_CAPS_H___



#include <__arch.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    // RISC-V EXTENSIONS
    uint8_t rv64_zbb;
    uint8_t rv64_zba;
    uint8_t rv64_zbs;
    // X86_64 EXTENSIONS
    uint8_t x86_abm;
    uint8_t x86_bmi1;
    // AARCH64 EXTENSIONS
} _dnml_hwcaps;

extern _dnml_hwcaps libdnml_caps;
#define _DNMLEMB_RISCV_ZBB 0

// ZBB + ZBA + ZBS detections
void __DNML_DETRV64_HWCAPS(void) {
// Linux detection
#if defined(__linux__)
    #include <sys/syscall.h>
    // Modern Linux detection
    #if defined(__NR_riscv_hwprobe)
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
    // Legacy Linux fallback
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
// BSD OSes detection
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
    #include <sys/aux.h>
    unsigned long hwcap = 0;
    elf_aux_info(AT_HWCAP, &hwcap, sizeof(hwcap));
    libdnml_caps.rv64_zbb = (hwcap & HWCAP_ISA_B) != 0;
#else // Bare metal, Embedded systems
    // Users are forced to manually set 
    // the _DNMLEMB_RISCV_ZBB flag
    libdnml_caps.rv64_zbb = _DNMLEMB_RISCV_ZBB;
#endif
}

// ABM + BMI1 detections
void __DNML_DETX64_HWCAPS(void) {
    return;
}

// __AARCH64__ detections
void __DNML_DETARM64_HWCAPS(void) {
    return;
}

// Detection main + dispatch
void _libdnml_detect_hwcaps(void) {
    #if __ARCH_X86_64__
        __DNML_DETX64_HWCAPS();
    #elif __ARCH_ARM64__
        __DNML_DETARM64_HWCAPS();
    #elif __ARCH_RVI64__
        __DNML_DETRV64_HWCAPS();
    #endif
}





#endif