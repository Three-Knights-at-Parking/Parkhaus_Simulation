//
// Created by ibach on 01.03.2026.
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H

#include "../types.h"

/*
 * Gate-Routing-Modul
 * - verteilt den globalen Tick-Demand (100%) auf vorhandene Gate-Queues
 * - prueft bei jedem Schritt, ob Queue/Element vorhanden ist
 * - schreibt nur Demand pro Gate in die Queues
 */

uint8_t GateRouting_DistributeTotalDemand(const Settings* settings, uint_16 total_demand,const Queue* gate_queues, rng* rng, uint32_t current_tick);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H