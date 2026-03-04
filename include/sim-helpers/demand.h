//
// Created by ibach on 01.03.2026.
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H

#include "../types.h"

/*
 * Demand module
 * - simulates the total demand per tick (global)
 * INPUT: Settings, Current_Tick, random number generator (with seed)
 * OUTPUT: returns total demand as unsigned integer
 */

uint16_t Demand_GenerateTotalPerTick(const Settings *p_settings, uint32_t current_tick, const rng* rng);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H