#include "atest_include.h"

uint64_t recursive_binary(uint64_t base, uint64_t power) {
    if (power == 0) return 1;
    uint64_t x = recursive_binary(base, power/2);
    if (!(power & 1)) return x*x;
    else return base*x*x;
}

uint64_t binary(uint64_t base, uint64_t power) {
    if (power == 0) return 1;
    uint64_t res = 1;
    while (power > 0) {
        if (power & 1) {
            res *= base;
        }
        base *= base;
        power >>= 1;
    } return res;
}

int main(void) {
    uint64_t base, power;
    fputs("Enter the base:  ", stdout); scanf("%llu", &base);
    fputs("Enter the power: ", stdout); scanf("%llu", &power);
    printf(" ------------ RESULTS - (%llu^%llu) ------------\n", base, power);
    printf("Recursive: %llu\n", recursive_binary(base, power));
    printf("Iterative: %llu\n", binary(base, power));

    return 0;
}