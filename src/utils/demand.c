//
// Created by ibach on 06.03.2026.
//

#include "types.h"
#include "utils/demand.h"
#include <math.h>
#include <tgmath.h>
#include "utils/SafteyUtils.h"

//Berrechnung nach poisson Verteilung
uint16_t demand_generate_total_perTick(const Settings *p_settings)
{
    checkNull(p_settings);

    //Erwartungswert -> how many Vehicles arrive in one Tick on average
    double lambda = (double)(p_settings->entry_probability_perSec_prec / 100) * p_settings->real_equivalent;

    if (lambda <= 0.0)
    {
        print_error("Percentage of Vehicles arriving too low");
        return ERROR;
    }

    //poisson Verteilung nach Kurth's Algorithmus
    //Absicherung bei Extremwerten
    if (lambda <= 30.0 && lambda >= 1e-12)
    {
        double L = exp(-lambda);
        int k = 0;
        double p = 1.0;

        do {
            k++;
            // Erzeuge Zufallszahl zwischen 0.0 und 1.0
            double u = (double)rng_range_int(0,100000) / (double)100000;
            p *= u;
        } while (p > L);

        return k - 1;
    }
    //Backup: Normalverteilungs-Approximation nach Box-Muller Transformation
    else
    {
        double rn1 = (double)rng_range_int(0,100000) / (double)100000;
        double rn2 = (double)rng_range_int(0,100000) / (double)100000;

        // Verhindere log(0) falls random_uniform exakt 0 zurückgibt
        if (rn1 < 1e-15)
        {
            rn1 = 1e-15;
        }

        // Box-Muller Transformation für Standardnormalverteilung
        double stdNormalverteilung = sqrt( (-2.0 * log(rn1)) ) * cos(2.0 * M_PI * rn2);

        // Skalierung auf unsere Erwartungswerte: mu = lambda, sigma = sqrt(lambda)
        // N = mu + sigma * Z
        double result = lambda + sqrt(lambda) * stdNormalverteilung;

        //Runden auf die nächste ganze Zahl -> besser mehr Fahrzeuge als zu wenig
        uint16_t demand = (int)round(result);

        // Negative Autos gibt es nicht (theoretisch bei Normalverteilung möglich)
        return demand < 0 ? 0 : demand;
    }
};






