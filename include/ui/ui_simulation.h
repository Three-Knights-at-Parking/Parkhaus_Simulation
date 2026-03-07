#ifndef UI_SIMULATION_H
#define UI_SIMULATION_H

/**
 * @file ui_simulation.h
 * @brief Simulation menu UI for starting and displaying simulations.
 *
 * This module is responsible for:
 * - printing the simulation menu and current settings
 * - starting the simulation backend
 * - printing tick statistics and final summary
 * - returning the next UI state
 */

#include "ui.h"
#include "types.h"

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
 * The settings overview is aligned with the config screen so the user sees
 * the same values in a familiar layout before starting the simulation.
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
 * - prints available statistics
 * - returns the next UI state
 *
 * @param[in] p_settings Pointer to the current settings object.
 * @param[in] p_simulation Pointer to the simulation object.
 * @return Next UI state depending on user selection.
 */
ui_state simulation_menu(Settings *p_settings, Simulation *p_simulation);

#endif /* UI_SIMULATION_H */