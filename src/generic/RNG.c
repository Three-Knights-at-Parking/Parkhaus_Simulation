#include "utils/RNG.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "utils/SafteyUtils.h"

// Internal custom RNG state. We don't rely on the compiler's rand() anymore. Funny number as placholder höhö
static uint32_t rng_state = 42067420;

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

    // Initialize our internal state
    rng_state = (uint32_t)p_settings->rand_seed;

    // Xorshift32 cannot have a state of 0. If it's 0, push it to 1.
    if (rng_state == 0) {
        rng_state = 1;
    }

    return OK;
}

uint32_t rng_next_u32(void) {
    // Xorshift32 algorithm
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

// Internal helper: draw uniformly in an inclusive unsigned range [min, max].
static uint32_t rng_range_u32(uint32_t min, uint32_t max) {
    if (min > max) {
        const uint32_t temp = min;
        min = max;
        max = temp;
    }

    const uint32_t range = max - min + 1;

    // If range is exactly 1 (min == max), no need to calculate anything
    if (range == 1) {
        return min;
    }

    // Rejection Sampling
    // Calculate the threshold where the modulo results become uneven.
    const uint32_t threshold = -range % range; // Equivalent to (2^32 - range) % range

    uint32_t raw;
    do {
        raw = rng_next_u32();
    } while (raw < threshold); // Reject numbers that cause modulo bias

    return min + (raw % range);
}

int32_t rng_range_int(int32_t min, int32_t max) {
    // Accept inverted inputs by swapping boundaries.
    if (min > max) {
        const int32_t temp = min;
        min = max;
        max = temp;
    }

    // We calculate the range as uint32_t to safely handle ranges
    // that span across negative and positive bounds without overflow.
    const uint32_t range = (uint32_t)max - (uint32_t)min + 1;

    if (range == 1) {
        return min;
    }

    const uint32_t threshold = -range % range;

    uint32_t raw;
    do {
        raw = rng_next_u32();
    } while (raw < threshold);

    return (int32_t)((uint32_t)min + (raw % range));
}

uint8_t rng_percent(void) {
    return (uint8_t)rng_range_u32(0, 100);
}
static double rng_normal(void) {
    double u1, u2;
    do {
        u1 = (double)rng_next_u32() / (double)UINT32_MAX;
    } while (u1 <= 0.0);

    u2 = (double)rng_next_u32() / (double)UINT32_MAX;

    // Box-Muller transform math
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * 3.14159265358979323846 * u2);
    return z0;
}

uint32_t rng_parking_time(uint32_t min_ticks, uint32_t max_ticks) {
    double mean = min_ticks + (max_ticks - min_ticks) / 2.0;

    // Set standard deviation so that ~99.7% of cars fall within the min/max bounds
    // (In a normal distribution, 6 standard deviations span the vast majority of the curve)
    double std_dev = (max_ticks - min_ticks) / 6.0;

    double val = (rng_normal() * std_dev) + mean;
    if (val < min_ticks) val = min_ticks;
    if (val > max_ticks) val = max_ticks;

    return (uint32_t)val;
}

uint32_t rng_gate_index(uint32_t num_gates) {
    if (num_gates == 0U) {
        return 0U;
    }
    return rng_range_u32(0U, num_gates - 1);
}

// Generates a Poisson-distributed random number based on a mean (lambda)
uint32_t rng_poisson(double lambda) {
    // Knuth's algorithm: L = e^(-lambda)
    double L = exp(-lambda);
    double p = 1.0;
    uint32_t k = 0;

    do {
        k++;
        double u;
        do {
            u = (double)rng_next_u32() / (double)UINT32_MAX;
        } while (u <= 0.0);

        p *= u;
    } while (p > L);

    return k - 1;
}