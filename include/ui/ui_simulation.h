#ifndef SIMULATION_H
#define SIMULATION_H

/**
 * @file simulation.h
 * @brief Simulation menu UI and interface to start the simulation (data layer).
 *
 * This module provides:
 * - Printing the simulation menu
 * - Starting a simulation run via the simulation/data layer
 * - Receiving statistics pointers via "handover" callbacks
 * - Iterating through the tick statistics list and printing them via ui_statistics
 */

#include "ui.h"
#include "../../include/types.h"
#include "config.h"

/* Max valid menu number in simulation menu (valid range: 0..SIMULATION_MAX_VALID_NUMBER). */
#define SIMULATION_MAX_VALID_NUMBER (2)

/* Max valid menu number in post simulation prompt (valid range: 0..SIM_POST_MAX_VALID_NUMBER). */
#define SIM_POST_MAX_VALID_NUMBER (1)

/**
 * @brief Prints the simulation menu including current configuration.
 *
 * @param[in] p_settings Current settings to display.
 */
void print_simulationscreen(const Settings *p_settings);

/**
 * @brief Post-simulation user prompt that offers jumping to the storage folder.
 *
 * @param[in] p_sim_output_path Path to the directory where simulation files were saved.
 */
void post_simulation_prompt(const char *p_sim_output_path);

/**
 * @brief Handles the simulation menu interaction.
 *
 * Starts the simulation and prints tick statistics and final summary depending
 * on the selected output_mode.
 *
 * @param[in] p_settings Current settings used to start the simulation.
 * @return Next UI state depending on user selection.
 */
ui_state simulation_menu(Settings *p_settings);

/**
 * @brief Starts the simulation (implemented by simulation/data layer).
 *
 * The simulation layer must:
 * - Run the simulation according to settings
 * - Provide tick statistics via hand_over_simulationdata(...)
 * - Provide final summary via hand_over_endstatistics(...)
 * - Create a new output directory for this run and return its path
 *
 * @param[in] p_settings Current settings used for the simulation.
 * @return Pointer to a string containing the output directory path (may be NULL on error).
 */
char *start_simulation(Settings *p_settings);

/**
 * @brief Callback: hands over the pointer to the stats list (tick history).
 *
 * This function is called by the simulation/data layer after creating the
 * statistics list container.
 *
 * @param[in] p_stat_list Pointer to the StatList that contains p_tick_head/tail.
 */
void hand_over_simulationdata(struct StatList *p_stat_list);

/**
 * @brief Callback: hands over the pointer to the final summary statistics.
 *
 * This function is called by the simulation/data layer after computing the
 * final StatsSummary.
 *
 * @param[in] p_summary Pointer to the aggregated summary statistics.
 */
void hand_over_endstatistics(struct StatsSummary *p_summary);

#endif /* SIMULATION_H */