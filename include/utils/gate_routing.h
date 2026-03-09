#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H

#include "../types.h"

/**
 * Gate routing module.
 * Distributes total demand per tick across all configured gate queues.
 */

/**
 * @brief Distributes total tick demand across all gate queues.
 * @author: ibach
 * @param settings Pointer to simulation settings containing gate count.
 * @param total_demand Total generated demand for the current tick.
 * @param gate_queues Pointer to the array of gate queues.
 * @param current_tick Current simulation tick.
 */
int GateRouting_DistributeTotalDemand(const Settings* settings,
                                      uint16_t total_demand,
                                      Queue** gate_queues,
                                      uint32_t current_tick);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GATE_ROUTING_H
