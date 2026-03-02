#include "atest_include.h"

static inline uint64_t __MODINV_UI64__(uint64_t x) {
    // The input needs to be sanitized/fact-checked as odd
    uint64_t res = 2 - x;
    for (uint8_t i = 0; i < 5; ++i) res *= 2 - x * res;
    return res;
}

static inline uint64_t __MODINV_UI64_ARM64__(uint64_t x) {
    uint64_t res, tmp;
    uint64_t cnst = 2;
    uint8_t cnt = 5;
    asm (
        "SUB  %[res], %[cnst], %[x] \n\t"  // res = 2 - x
        "1: \n\t"
        "MSUB %[tmp], %[x], %[res], %[cnst] \n\t" // tmp = 2 - (x * res)
        "MUL  %[res], %[res], %[tmp] \n\t"       // res = res * tmp 
        "SUBS %w[cnt], %w[cnt], #1 \n\t" // --i
        "B.NE 1b \n\t"
        : [res] "=&r" (res), [tmp] "=&r" (tmp), [cnt] "+r" (cnt)
        : [x] "r" (x), [cnst] "r" (cnst)
        : "cc"
    ); return res; 
}

static inline uint64_t __MODINV_UI64_ARM64_UNROLLED__(uint64_t x) {
    uint64_t res, tmp;
    uint64_t cnst = 2;
    asm (
        "SUB  %[res], %[cnst], %[x] \n\t"  // res = 2 - x
        // 1st - 5th iteration
        "MSUB %[tmp], %[x], %[res], %[cnst] \n\t" // tmp = 2 - (x * res)
        "MUL  %[res], %[res], %[tmp] \n\t"        // res = res * tmp
        "MSUB %[tmp], %[x], %[res], %[cnst] \n\t"
        "MUL  %[res], %[res], %[tmp] \n\t"
        "MSUB %[tmp], %[x], %[res], %[cnst] \n\t"
        "MUL  %[res], %[res], %[tmp] \n\t"
         "MSUB %[tmp], %[x], %[res], %[cnst] \n\t"
        "MUL  %[res], %[res], %[tmp] \n\t"
         "MSUB %[tmp], %[x], %[res], %[cnst] \n\t"
        "MUL  %[res], %[res], %[tmp] \n\t"
        : [res] "=&r" (res), [tmp] "=&r" (tmp)
        : [x] "r" (x), [cnst] "r" (cnst)
        : "cc"
    ); return res; 
}

int main(void) {
    puts("---------------- Modular Inverse Test ----------------");
    printf("Enter loop iteration count:    ");
    char c[21]; scanf("%20s", c); c[20] = '\0';
    char *p = &c[20];
    uint64_t input = strtoull(c, &p, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Value Overflow - Abort...");
        abort();
    }
    
    srand(time(NULL)); uint64_t initial_seed = rand();
    struct timespec start, end;
    long double vanilla_time = 0, assembly_time = 0, uasm_time = 0;
    uint64_t vanilla_res = 0, assembly_res = 0, unasm_res = 0;

    //* -------- Vanilla C --------*/
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < input; ++i) { 
        initial_seed = initial_seed * 6364136223846793005ULL + 1;
        vanilla_res ^= __MODINV_UI64__(initial_seed);
    } clock_gettime(CLOCK_MONOTONIC, &end);
    vanilla_time = (long double)(end.tv_sec - start.tv_sec) 
                 + (long double)(end.tv_nsec - start.tv_nsec) / 1e9L;


    //* ---- ARM64 Assembly - Looped ---- */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < input; ++i) { 
        initial_seed = initial_seed * 6364136223846793005ULL + 1;
        assembly_res ^= __MODINV_UI64_ARM64__(initial_seed);
    } clock_gettime(CLOCK_MONOTONIC, &end);
    assembly_time = (long double)(end.tv_sec - start.tv_sec) 
                  + (long double)(end.tv_nsec - start.tv_nsec) / 1e9L;


    //* ---- ARM64 Assembly - Unrolled ---- */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < input; ++i) {
        initial_seed = initial_seed * 6364136223846793005ULL + 1;
        unasm_res ^= __MODINV_UI64_ARM64_UNROLLED__(initial_seed);
    } clock_gettime(CLOCK_MONOTONIC, &end);
    uasm_time = (long double)(end.tv_sec - start.tv_sec) 
             + (long double)(end.tv_nsec - start.tv_nsec) / 1e9L;

    puts("-------------------- Result Board --------------------");
    puts("                                            Time");
    printf("o) Vanilla C Performance:               %.9Lf\n", vanilla_time);
    printf("o) ARM64 Assembly Performance:          %.9Lf\n", assembly_time);
    printf("o) ARM64 Assembly Unrolled Performance: %.9Lf\n\n", uasm_time);
    printf("o) Vanilla C's Sink:     %" PRIu64 "\n", vanilla_res);
    printf("o) ARM64 Assembly Sink:  %" PRIu64 "\n", assembly_res);
    printf("o) UARM64 Assembly Sink: %" PRIu64 "\n", unasm_res);
    puts("------------------------------------------------------");
    return 0;
}