#include "utils/RNG.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <utils/SafteyUtils.h>

// Internal helper: draw uniformly in an inclusive unsigned range [min, max].
static uint32_t rng_range_u32(uint32_t min, uint32_t max) {
    // Accept inverted inputs by swapping boundaries.
    if (min > max) {
        const uint32_t temp = min;
        min = max;
        max = temp;
    }

    // Inclusive range size: e.g. [5, 7] -> 3 values.
    const uint32_t range = (uint32_t)max - (uint32_t)min + 1;
    const uint32_t raw = (uint32_t)rng_next_u32();
    // Fold raw RNG value into the target range.
    const uint32_t offset = raw % range;

    return min + (uint32_t)offset;
}

int rng_init(Settings *p_settings)
{
    if (p_settings == NULL)
    {
        printf("No Settings provided\n");
        return ERROR;
    }

    // Default seed: current Unix timestamp (seconds).
    if (p_settings->rand_seed == -1) {
        p_settings->rand_seed = (int32_t)time(NULL);
    }


    // Initialize C's global RNG state used by rand().
    srand((unsigned int)p_settings->rand_seed);
    return OK;
}

uint32_t rng_next_u32(void) {
    // Build a 32-bit value from two 16-bit chunks of rand().
    const uint32_t upper = ((uint32_t)rand() & 0xFFFFU) << 16U;
    const uint32_t lower = (uint32_t)rand() & 0xFFFFU;
    return upper | lower;
}

int32_t rng_range_int(int32_t min, int32_t max) {
    // Accept inverted inputs by swapping boundaries.
    if (min > max) {
        const int32_t temp = min;
        min = max;
        max = temp;
    }

    // Inclusive range size: e.g. [0, 100] -> 101 values.
    const uint32_t range = ((int32_t)max - (int32_t)min) + 1;
    const uint32_t raw = (uint32_t)rng_next_u32();
    // Fold raw RNG value into the target range.
    const uint32_t offset = raw % range;

    return ((int32_t)min + (int32_t)offset);
}

uint8_t rng_percent(void) {
    // Inclusive percentage for simple probability checks.
    return (uint8_t)rng_range_int(0, 100);
}

uint32_t rng_parking_time(uint32_t min_ticks, uint32_t max_ticks) {
    // Uniform parking time in ticks (inclusive).
    return rng_range_u32(min_ticks, max_ticks);
}

uint32_t rng_gate_index(uint32_t num_gates) {
    // Guard for empty gate list.
    if (num_gates == 0U) {
        return 0U;
    }

    // Valid indices are [0, num_gates - 1].
    return rng_range_u32(0U, num_gates - 1);
}
