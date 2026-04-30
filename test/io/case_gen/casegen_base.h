#ifndef IO_BASE_CASE_GEN_H
#define IO_BASE_CASE_GEN_H


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


typedef enum {
    INTENSITY_NONE,
    INTENSITY_LOW,
    INTENSITY_HIGH,
    INTENSITY_EXTREME
} weight_level_t;

weight_level_t get_weighted_level(
    uint64_t seed,
    int none, int low, 
    int high, int extreme
) {
    int r = seed % 100;
    if (r < none) return INTENSITY_NONE;
    if (r < (none + low)) return INTENSITY_LOW;
    if (r < (none + low + high)) return INTENSITY_HIGH;
    return INTENSITY_EXTREME;
}




#endif