//
// Created by ibach on 01.03.2026.
// @author: Ibach
//

#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATS_H

#include "types.h"

/**
 * @file Stats.h
 * @brief API for per-tick statistics and overall aggregation.
 *
 * Tick snapshots are stored in a doubly linked list
 * (p_tick_head ... p_tick_tail) inside `Simulation.StatList`.
 * Aggregation in `StatsSummary` is built on demand at the end of
 * the simulation using this history.
 */

/**
 * Initializes the statistics container for tick history.
 *
 * @param p_sim         Target container
 * @param capacity_total  Total parking garage capacity
 */
int StatsTick_init(Simulation *p_sim, uint16_t capacity_total, uint32_t current_tick);

/**
 * Initializes the statistics list container for a simulation.
 */
StatList *StatList_init(Simulation *p_simulation);

/**
 * Frees the statistics list container itself.
 */
int StatList_free(StatList *p_stats);

/**
 * Frees all elements stored in the tick list.
 */
int StatsTick_free(StatList *p_stats);

/** Sets end-of-tick capacity values. */
int stats_tick_set_capacity(StatList *p_stats, uint16_t taken, uint16_t free);

/** Increments queue rejections for the current tick. */
int stats_tick_add_queue_rejections(StatList *p_stats, uint16_t amount);

/** Increments generated arrivals for the current tick. */
int stats_tick_add_arrivals_generated(StatList *p_stats, uint16_t amount);

/** Sets whether the "full" blocker was active in this tick (0/1). */
int stats_tick_add_blocker_full_active(StatList *p_stats);

/**
 * Records vehicle-based metrics into the current tick.
 *
 * The function derives values from GenericVehicle timestamps relative
 * to `current_tick`:
 * - queue wait + entered counter if `park_house_entered == current_tick`
 * - parking duration + departed counter if `park_house_left == current_tick`
 *
 * For car objects, bad parking is counted when `spaces_needed > minimum_spaces`.
 */
int stats_tick_add_vehicle(StatList *p_stats, const GenericVehicle *p_vehicle, uint32_t current_tick);

/** Returns the most recently committed tick (tail) or NULL. */
const StatsTick *stats_get_latest_tick(const StatList *p_stats);

/**
 * Computes the aggregated summary statistics from all stored ticks.
 * p_summary must be provided by the caller.
 */
int stats_build_summary(const StatList *p_stats, StatsSummary *p_summary);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATS_H
