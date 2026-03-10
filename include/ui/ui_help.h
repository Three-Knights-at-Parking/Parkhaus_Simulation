//
// Created by Daniel_Work on 10.03.2026.
//
#ifndef UI_HELP_H
#define UI_HELP_H

/**
 * @file ui_help.h
 * @brief Help menu UI for explaining simulation behavior and settings.
 *
 * This module is responsible for:
 * - printing the help menu
 * - explaining the simulation model
 * - explaining the most important configuration parameters
 * - returning to the home menu
 */

#include "ui.h"

enum users_choice_help{
 BACK_HELP = 0,
 SIMULATION_HELP = 1,
 SETTINGS_HELP = 2
};

/* ========================================================================= */
/* Menu limits                                                               */
/* ========================================================================= */

/**
 * @brief Maximum valid menu number in the help menu.
 *
 * Valid input range:
 * 0 .. HELP_MAX_VALID_NUMBER
 */
#define HELP_MAX_VALID_NUMBER (2)

/* ========================================================================= */
/* Public interface                                                          */
/* ========================================================================= */

/**
 * @brief Prints the help menu screen.
 *
 * The help menu allows navigation to:
 * - simulation model explanation
 * - settings explanation
 * - return to home menu
 */
void print_helpscreen(void);

/**
 * @brief Prints the simulation model help page.
 *
 * Explains:
 * - general simulation concept
 * - time model
 * - arrival model
 * - statistics overview
 */
void print_help_simulation(void);

/**
 * @brief Prints the settings explanation help page.
 *
 * Explains:
 * - capacity
 * - floors
 * - gates
 * - gate entry time
 * - tick length
 * - output mode
 * - max ticks
 * - random seed
 */
void print_help_settings(void);

/**
 * @brief Handles user interaction in the help menu.
 *
 * This function:
 * - prints the help menu
 * - reads and validates user input
 * - shows the selected help page
 * - returns to the home menu
 *
 * @return Next UI state depending on user selection.
 */
ui_state help_menu(void);

#endif /* UI_HELP_H */