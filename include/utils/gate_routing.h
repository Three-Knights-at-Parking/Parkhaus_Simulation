//
// Created by ibach on 01.03.2026.
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H

#include "../types.h"

/*
 * Gate routing module
 * - distributes global per-tick demand (100%) across existing gate queues
 * - checks at every step whether queue/element exists
 * - writes only per-gate demand values into the queues
 */

uint8_t GateRouting_DistributeTotalDemand(const Settings* settings, uint_16 total_demand,const Queue* gate_queues, rng* rng, uint32_t current_tick);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H