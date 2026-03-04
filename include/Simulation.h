#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATION_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATION_H

#include "types.h"
/**
 * Base Simulation Object acting as an overarching parent for all other child objects. This represents one
 * Simulation that carries its own parking complex, queue and statistics. Future usage might be to parallel
 * / multithread with multiple Simulations.
 */

    /**
     * Initialize a Simulation with Settings.
     * @param p_sim Pointer to the Simulation object to initialize.
     * @param p_settings Pointer to the Settings used to configure the Simulation.
     * @return 0 on success, non-zero on error.
     */
    int simulation_init(Simulation *p_sim, const Settings *p_settings, const Stats* stats); //statistik Hinzufügen zu initialisierung?

    /**
     * Progress the Simulation by one tick.
     * @param p_sim Pointer to the Simulation object to tick.
     * @return Short Status / Success of the tick (0 on success, non-zero on error or stop condition).
     */
    int simulation_tick(Simulation *p_sim);

    /**
     * Start a Simulation with given settings.
     * @param p_sim Pointer to the Simulation object to run.
     * @return 0 on success, non-zero on error.
     */
    int simulation_start(Simulation *p_sim);

    /**
     * End the Simulation and free up its memory. Simulation takes ownership of its children, freeing them too.
     * @param p_sim Pointer to the Simulation object to stop.
     */
    void simulation_end(Simulation *p_sim);

    /**
     * Free this Simulation-Object's memory. Simulation takes ownership of its children, freeing them too. In general,
     * freeing the Simulation-Object without ending it is not useful. Rather call simulation_end.
     * @param p_sim Pointer to the Simulation object to stop.
     * @return 0 on success, non-zero on error.
     */
    int free_simulation(Simulation *p_sim);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATION_H