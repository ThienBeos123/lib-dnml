#ifndef IO_STR_CASE_GEN_H
#define IO_STR_CASE_GEN_H


// Essential Includes
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../../test_ui/_strui.h"
#include "../../../util/util.h"

/* String Random Generation Configuration
*   - Note - Repetition of TEST_CONDUCT.md:
*       +) Key component configuration entirely dictates
*          the numerical amounts configuration, or, more precisely,
*          numerical amounts configuration is automatically not set
*          and MUST NOT be touched if "key components configuration" 
*          of such components are NOT set to TRUE
*       
*       +) Any early implementation may accept the use of simpler &
*          less cryptographically-secured combination of srand(time(NULL))
*          and rand() by ANSI-C <stdlib.h>, though TRNG/HWRNG are specifically
*          preferred for more unbiased source of entropy for XORSHIFT64
*
*       +) Regarding entropy collection via TRNG/HWRNG, it is generally
*          preferred to use lib-dnml's OS-dispatched entropy collector -
*          __get_entropy() - for cross-platform compatibility and
*          stability of usage from the library's standardization. 
*          However, early prototypes may use OS-specific entropy
*          harvest ONLY for prototyping of functionality and reliability,
*          BUT final implementation must use the general function like __get_entropy() 
*          or another specialized entropy-collection function that supports
*          at least the dispatching of MacOS, Linux, AND Windows (64 bit).
*/

typedef struct { 
    float chance; 
    uint8_t low_qbound; uint8_t high_qbound; // Quantitative Bounds
    float low_pbound; float high_pbound; // Probability Bounds
} component_prob_t;
typedef enum { CLEAN_MODE, STANDARD_MODE, FAULTY_MODE } gen_mode;
const component_prob_t prob_matrix[3][7] = {
    [ CLEAN_MODE ] = {
        {5.2f, 1, 1}, {0, 0, 0},
        {50.0f, 1, 1, 0.0f, 0.0f}, // Signs 
        {50.0f, 1, 1, 0.0f, 0.0f}, // Base-prefix
        {0.0f, 0, 0, 0.0f, 0.0f}, // Junk
        {0.0f, 0, 0, 0.0f, 0.0f}, // Invalid Digits
        {0.0f, 0, 0, 0.0f, 0.0f} // Early Null
    },
    [ STANDARD_MODE ] = {
        {30.5f, 2, 4}, {27.6f, 1, 6},
        {75.0f, 1, 1, 0.0f, 0.0f}, // Signs
        {67.67f, 1, 2, 0.4f, 5.7f}, // Base-prefix
        {8.5f, 1, 3, 1.2f, 6.7f}, // Junk
        {3.3f, 1, 3, 0.09f, 1.2f}, // Invalid Digits
        {0.01f, 1, 1, 0.0001f, 0.005f} // Early Null
    },
    [ FAULTY_MODE ] = {
        {50.0f, 2, 10}, {50.0f, 2, 16},
        {95.0f, 1, 2, 12.34f, 45.25f}, // Sign
        {90.0f, 1, 4, 45.25f, 78.6f}, // Base-prefix
        {25.0f, 6, 72, 12.3f, 34.5f}, // Junk
        {6.5f, 1, 6, 1.4f, 5.9f}, // Invalid Digits
        {2.5f, 0, 0, 0.025f, 0.1f} // Early Null
    },
};
const uint8_t inval_drift_vector[3][2] = {
    [ CLEAN_MODE ] = {0, 0},
    [ STANDARD_MODE ] = {3, 6},
    [ FAULTY_MODE ] = {1, 2}
};

typedef struct {
    xoshiro256_state base_seed;
    // Key components configuration
    bool whitespace; bool lzeros;
    bool sign; bool bprefix; bool junk;
    bool inval_digit; bool early_null;
    
    // Numerical amounts configuration
    uint8_t wscount; uint8_t lzcount;
    bool mixed_sign; bool mixed_bp; size_t bprefix_cnt;
    float junk_chance; size_t max_junk_cnt; // Max Junk Count may never be reached
    float init_inval_chance; uint8_t inval_digit_cnt; // Inval Digit Count will always be reached
    uint8_t inval_digit_drift; float enull_chance;

    // Further Configuration Settings
    gen_mode mod_gen_mode;
} str_rand_mod;

// The actual function
static inline uint64_t get_fair_range(uint64_t min, uint64_t max) {
    uint64_t range = max - min + 1, r;
    uint64_t limit = UINT64_MAX - (UINT64_MAX % range);
    // Rejection sampling to eliminate modulo bias
    do { r = ; } while (r >= limit);
    return min + (r % range);
}
static inline float __seed_to_float(uint64_t *state) {
    *state += 0x9E3779B97F4A7C15; // Increment state (Weyl sequence)
    uint64_t z = *state;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9; // Scramble
    z = (z ^ (z >> 27)) * 0x94D049BB133111EB;
    return (float)(z >> 11) * (1.0 / 9007199254740992.0); // Better 53-bit float conversion
}
static inline void _strgen_rseed_include(str_rand_mod* config) {
    gen_mode i = config->mod_gen_mode;
    float curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    // ----- 1. Collateral Component Inclusion Filling -----
    config->whitespace = (curr_roll < prob_matrix[i][0].chance) ? true : false;
    curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    config->lzeros = (curr_roll < prob_matrix[i][1].chance) ? true : false;

    // ----- 2. Small Lexical Component Inclusion Filling -----
    curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    config->sign = (curr_roll < prob_matrix[i][2].chance) ? true : false;
    curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    config->bprefix = (curr_roll < prob_matrix[i][3].chance) ? true : false;
    curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    config->early_null = (curr_roll < prob_matrix[i][5].chance) ? true : false;

    // ----- 3. Main Numerical Component Inclusion Filling -----
    curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    config->junk = (curr_roll < prob_matrix[i][4].chance) ? true : false;
    curr_roll = (float)(fmod(__seed_to_float(&seed), 100));
    config->inval_digit = (curr_roll < prob_matrix[i][5].chance) ? true : false;
}
static inline void _strgen_rseed_quant(str_rand_mod* config) {
    gen_mode i = config->mod_gen_mode;
    component_prob_t comp = prob_matrix[i][0];
    // ----- 1. Whitespace & Leading Zeros -----
}
static inline void strgen(char *buf, size_t len, xoshiro256_state seed, str_rand_mod* config) {}




#endif