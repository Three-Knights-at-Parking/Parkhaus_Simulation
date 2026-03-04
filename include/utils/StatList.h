#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATLISTHANDLER_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATLISTHANDLER_H

#include "../types.h"

    /**
     * @brief Append a StatsTick node at the end of the Simulation's stats list.
     *        Does not allocate p_tick, only links it into the list. Ownership stays
     *        with the caller.
     * @note Does not check if p_tick is empty and will override head.
     * @param p_sim Pointer to Simulation (owns the list).
     * @param p_tick Pointer to StatsTick to append (p_next should be NULL).
     * @return 0 on success, non-zero on error.
     */
    int statlist_append(Simulation *p_sim, StatsTick *p_tick);

    /**
     * @brief Remove and free all StatsTick nodes in the Simulation's stats list.
     *        After this call, p_sim->StatList->p_tick_head and p_sim->StatList->p_tick_tail will be NULL.
     * @note Takes ownership of all StatTick nodes.
     * @param p_sim Pointer to Simulation (owns the list).
     */
    void statlist_clear(Simulation *p_sim);

    /**
     * @brief Compute an aggregated StatsSummary from the Simulation's stats list.
     *        Iterates over all StatsTick entries and fills p_summary accordingly.
     * @param p_sim Pointer to Simulation with p_sim->StatList tick history.
     * @param p_summary Pointer to StatsSummary to fill.
     * @return 0 on success, non-zero on error (e.g. no stats available).
     */
    int statlist_compute_summary(const Simulation *p_sim, StatsSummary *p_summary);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_STATLISTHANDLER_H
