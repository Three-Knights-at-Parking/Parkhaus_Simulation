#include "types.h"
#include "utils/demand.h"
#include <math.h>
#include "utils/SafteyUtils.h"
#include "utils/RNG.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int demand_generate_total_perTick(const Settings *p_settings)
{
    if (checkNull(p_settings)) {
        print_error_s("Settings pointer is NULL", LOW);
        return ERROR;
    }
    const double lambda = ((double)p_settings->entry_probability_perSec_prec / 100.0) * (double)p_settings->real_equivalent;
    if (lambda <= 0.0)
    {
        return 0;
    }

    // Poisson distribution using Knuth's algorithm.
    // Safeguard for low and moderate lambda values.
    if (lambda <= 30.0)
    {
        return (int)rng_poisson(lambda);
    }
    // Fallback: normal approximation via Box-Muller transformation.
    double u1, u2;
    do {
        u1 = (double)rng_next_u32() / (double)UINT32_MAX;
    } while (u1 <= 0.0);

    u2 = (double)rng_next_u32() / (double)UINT32_MAX;
    double stdNormalverteilung = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

    // Scale to target mean/variance: mu = lambda, sigma = sqrt(lambda).
    // N = mu + sigma * Z.
    double result = lambda + (sqrt(lambda) * stdNormalverteilung);
    // Round to nearest integer.
    int demand = (int)round(result);
    // Demand cannot be negative (possible in rare extreme tails of normal approximation).
    return demand < 0 ? 0 : demand;
}
