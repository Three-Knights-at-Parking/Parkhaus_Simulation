//
// Created by ibach on 06.03.2026.
//

#include "types.h"
#include "utils/demand.h"
#include <math.h>
#include <tgmath.h>
#include "utils/SafteyUtils.h"
#include "utils/RNG.h"


int demand_generate_total_perTick(const Settings *p_settings)
{
    checkNull(p_settings);

    // Expected value: average vehicles arriving in one tick.
    double lambda = (double)(p_settings->entry_probability_perSec_prec / 100) * p_settings->real_equivalent;

    if (lambda <= 0.0)
    {
        print_error("Percentage of Vehicles arriving too low");
        return ERROR;
    }

    // Poisson distribution using Knuth's algorithm.
    // Safeguard for low and moderate lambda values.
    if (lambda <= 30.0 && lambda >= 1e-12)
    {
        double L = exp(-lambda);
        int k = 0;
        double p = 1.0;

        do {
            k++;
            // Generate a random value in the range [0.0, 1.0].
            double u = (double)rng_range_int(0,100000) / (double)100000;
            p *= u;
        } while (p > L);

        return k - 1;
    }
    // Fallback: normal approximation via Box-Muller transformation.
    else
    {
        double rn1 = (double)rng_range_int(0,100000) / (double)100000;
        double rn2 = (double)rng_range_int(0,100000) / (double)100000;

        // Prevent log(0) if the RNG returns an exact 0 value.
        if (rn1 < 1e-15)
        {
            rn1 = 1e-15;
        }

        // Box-Muller transform for a standard normal variable.
        double stdNormalverteilung = sqrt((-2.0 * log(rn1))) * cos(2.0 * M_PI * rn2);

        // Scale to target mean/variance: mu = lambda, sigma = sqrt(lambda).
        // N = mu + sigma * Z.
        double result = lambda + sqrt(lambda) * stdNormalverteilung;

        // Round to nearest integer.
        uint16_t demand = (int)round(result);

        // Demand cannot be negative (possible in normal approximation).
        return demand < 0 ? 0 : demand;
    }
}
