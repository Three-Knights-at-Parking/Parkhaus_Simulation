#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATION_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATION_H

#include "types.h"
/**
 * Simulation root object.
 * Owns settings, Parkhaus, queues, and statistics for one simulation run.
 */

/**
 * @brief Initializes simulation state, RNG, queues, and Parkhaus.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object to initialize.
 * @param p_settings Pointer to simulation settings.
 * @param p_StatList Pointer to the statistics list.
 */
int simulation_init(Simulation *p_sim, const Settings *p_settings, StatList *p_StatList);

/**
 * @brief Frees child objects owned by the Simulation.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object.
 */
static void simulation_cleanup_children(Simulation *p_sim);

/**
 * @brief Advances the simulation by one tick.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object.
 */
int simulation_tick(Simulation *p_sim);

/**
 * @brief Starts the simulation and resets runtime tick state.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object.
 */
int simulation_start(Simulation *p_sim);

/**
 * @brief Runs the simulation for the configured duration in ticks.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object.
 */
int simulation_run(Simulation *p_sim);

/**
 * @brief Ends the simulation, persists summary data, and cleans up children.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object.
 */
void simulation_end(Simulation *p_sim);

/**
 * @brief Frees the Simulation object and owned Parkhaus resources.
 * @author: ibach
 * @param p_sim Pointer to the Simulation object.
 */
int free_simulation(Simulation *p_sim);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATION_H
