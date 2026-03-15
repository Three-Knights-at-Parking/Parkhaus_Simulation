//
// Created by ibach on 01.03.2026.
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H

#include "../types.h"

/**
 * @brief Generates total vehicle demand per tick based on Poisson-distributed arrivals.
 * @author: ibach
 * @param p_settings Pointer to simulation settings.
 */
int demand_generate_total_perTick(const Settings *p_settings);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H