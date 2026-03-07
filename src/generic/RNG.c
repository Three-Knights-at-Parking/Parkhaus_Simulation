#include "utils/RNG.h"
#include <stdlib.h>
#include <time.h>


static uint32_t rng_range_u32(uint32_t min, uint32_t max) {
    if (min > max) {
        const uint32_t temp = min;
        min = max;
        max = temp;
    }
    const uint32_t range = (uint32_t)max - (uint32_t)min + 1;
    const uint32_t raw = (uint32_t)rng_next_u32();
    const uint32_t offset = raw % range;

    return min + (uint32_t)offset;
}

int rng_init(const Settings *p_settings) {
    uint32_t seed = 0;
    if (p_settings == NULL || p_settings->rand_seed == -1) {
        seed = (uint32_t)time(NULL);
    } else {
        seed = (uint32_t)p_settings->rand_seed;
    }
    srand((unsigned int)seed);
    return 0;
}

uint32_t rng_next_u32(void) {
    const uint32_t upper = ((uint32_t)rand() & 0xFFFFU) << 16U;
    const uint32_t lower = (uint32_t)rand() & 0xFFFFU;
    return upper | lower;
}

int32_t rng_range_int(int32_t min, int32_t max) {
    if (min > max) {
        const int32_t temp = min;
        min = max;
        max = temp;
    }
    const uint32_t range = ((int32_t)max - (int32_t)min) + 1;
    const uint32_t raw = (uint32_t)rng_next_u32();
    const uint32_t offset = raw % range;

    return ((int32_t)min + (int32_t)offset);
}

uint8_t rng_percent(void) {
    return (uint8_t)rng_range_int(0, 100);
}

uint32_t rng_parking_time(uint32_t min_ticks, uint32_t max_ticks) {
    return rng_range_u32(min_ticks, max_ticks);
}

uint32_t rng_gate_index(uint32_t num_gates) {
    if (num_gates == 0U) {
        return 0U;
    }

    // Valid indices are [0, num_gates - 1].
    return rng_range_u32(0U, num_gates - 1);
}
