#include "utils/demand.h"
#include "utils/RNG.h"
#include <stddef.h>

int demand_generate_total_perTick(const Settings *p_settings) {
    if (p_settings == NULL) {
        return ERROR;
    }

    // The expected average arrivals per tick (lambda)
    // Assuming entry_probability is cars per second, and real_equivalent is seconds per tick.
    double lambda = (double)p_settings->entry_probability_perSec_prec * (double)p_settings->real_equivalent;
    if (lambda <= 0.0) {
        return ERROR;
    }

    uint32_t arrivals_this_tick = rng_poisson(lambda);

    return (int)arrivals_this_tick;
}