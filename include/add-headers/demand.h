//
// Created by ibach on 01.03.2026.
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H

#include "../types.h"

/*
 * Demand-Modul
 * - simuliert den gesamten Demand pro Tick (global)
 * INPUT: Settings, Current_Tick, Random Number Generator(mit seed)
 * OUTPUT: RETURN Total Demand als GZ
 */

uint16_t Demand_GenerateTotalPerTick(const Settings *p_settings, uint32_t current_tick, const rng* rng);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_DEMAND_H