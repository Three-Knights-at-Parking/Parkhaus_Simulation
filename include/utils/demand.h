//
// Created by ibach on 01.03.2026.
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H

#include "../types.h"
#include "utils/RNG.h"

/*
 * Demand module
 * - simulates the total demand per tick (global)
 * INPUT: Settings, Current_Tick, random number generator (with seed)
 * OUTPUT: returns total demand as unsigned integer
 */

uint16_t demand_generate_total_perTick(const Settings *p_settings);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H