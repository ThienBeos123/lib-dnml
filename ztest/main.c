#include "atest_include.h"
#include "../calculation_algorithms/header/num_theory.h"

int main(void) {
    fputs("Input to test for primality: ", stdout);
    uint64_t x; scanf("%llu", &x);
    printf("Trial Division verdict:     %s\n", (__BIGINT_TRIAL_DIV__(x)) ? "True" : "False");
    printf("Miller Rabin verdict:       %s\n", (__BIGINT_SMALL_MRABIN__(x)) ? "True" : "False");

    return 0;
}