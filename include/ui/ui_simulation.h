#ifndef UI_SIMULATION_H
#define UI_SIMULATION_H

/**
 * @file ui_simulation.h
 * @brief Simulation menu UI for starting and displaying simulations.
 *
 * This module is responsible for:
 * - printing the simulation menu and current settings
 * - starting the simulation backend
 * - forwarding tick statistics and final summary output
 * - returning the next UI state
 */

#include "ui.h"
#include "types.h"

/* ========================================================================= */
/* Simulation menu selection                                                 */
/* ========================================================================= */

/**
 * @brief Menu entries for the simulation menu.
 */
typedef enum
{
    SIMULATION_MENU_BACK = 0,
    SIMULATION_MENU_START = 1,
    SIMULATION_MENU_CONFIG = 2
} simulation_menu_choice;

/* ========================================================================= */
/* Simulation menu limits                                                    */
/* ========================================================================= */

/**
 * @brief Maximum valid menu number in the simulation menu.
 *
 * Valid range:
 * 0 .. SIMULATION_MAX_VALID_NUMBER
 */
#define SIMULATION_MAX_VALID_NUMBER (2)

/* ========================================================================= */
/* Public interface                                                          */
/* ========================================================================= */

/**
 * @brief Prints the simulation screen including current settings.
 *
 * The settings overview is aligned with the configuration screen so the user
 * sees the same values before starting the simulation.
 *
 * @param[in] p_settings Pointer to the current settings object.
 * @return OK on success, ERROR if p_settings is invalid.
 */
int print_simulationscreen(const Settings *p_settings);

/**
 * @brief Handles user interaction in the simulation menu.
 *
 * This function:
 * - prints the simulation screen
 * - reads and validates the user's menu selection
 * - starts the simulation backend
 * - returns the next UI state
 *
 * @param[in] p_settings Pointer to the current settings object.
 * @param[in] p_simulation Pointer to the simulation object.
 * @return Next UI state depending on user selection.
 */
ui_state simulation_menu(Settings *p_settings, Simulation *p_simulation);

/* ========================================================================= */
/* Backend statistics output interface                                       */
/* ========================================================================= */

/**
 * @brief Forwards one simulation tick from the backend to the UI layer.
 *
 * This function is intended to be called by the simulation backend
 * during runtime to print statistics for a single simulation tick.
 *
 * @param[in] p_current_tick Pointer to the current tick statistics.
 * @param[in] output_mode Selected output mode.
 */
void print_StatsTick_backend(const StatsTick *p_current_tick,
                             enum OutputMode output_mode);

/**
 * @brief Forwards the final simulation summary from the backend to the UI.
 *
 * This function is intended to be called by the simulation backend
 * once the simulation has finished.
 *
 * @param[in] p_stats_summary Pointer to the final summary statistics.
 * @param[in] output_mode Selected output mode.
 */
void print_final_stats_backend(const StatsSummary *p_stats_summary,
                               enum OutputMode output_mode);

#endif /* UI_SIMULATION_H */