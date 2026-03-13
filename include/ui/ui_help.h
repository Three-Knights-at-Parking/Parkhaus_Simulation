#ifndef UI_HELP_H
#define UI_HELP_H

/**
 * @file ui_help.h
 * @brief Help menu UI for explaining simulation behavior and settings.
 *
 * This module is responsible for:
 * - printing the help menu
 * - explaining the simulation model
 * - explaining the configurable settings
 * - returning to the home menu
 */

#include "ui.h"

/**
 * @brief Menu entries for the help menu.
 */
typedef enum
{
    HELP_MENU_BACK = 0,
    HELP_MENU_SIMULATION = 1,
    HELP_MENU_SETTINGS = 2,
    HELP_MENU_FILE_HANDLING = 3
} help_menu_choice;

/* ========================================================================= */
/* Menu limits                                                               */
/* ========================================================================= */

/**
 * @brief Maximum valid menu number in the help menu.
 *
 * Valid input range:
 * 0 .. HELP_MAX_VALID_NUMBER
 */
#define HELP_MAX_VALID_NUMBER (3)

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
 * - parking duration model
 * - statistics overview
 */
void print_help_simulation(void);

/**
 * @brief Prints the settings explanation help page.
 *
 * Explains:
 * - name
 * - capacity
 * - floors
 * - gates
 * - gate entry time
 * - tick length
 * - minimum parking ticks
 * - maximum parking ticks
 * - entry probability / rate conversion
 * - max ticks
 * - random seed
 * - output mode
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