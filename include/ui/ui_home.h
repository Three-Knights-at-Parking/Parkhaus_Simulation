#ifndef HOME_H
#define HOME_H

/**
 * @file home.h
 * @brief Home menu UI (main navigation screen).
 *
 * The Home menu is the central navigation point of the application.
 * It allows switching to:
 * - Simulation menu
 * - Configuration menu
 * - Storage menu
 * - Exit
 */

#include "ui.h"

/**
 * @brief Maximum valid menu number on the Home screen (valid range: 0..HOME_MAX_VALID_NUMBER).
 */
#define HOME_MAX_VALID_NUMBER (3)

/**
 * @brief Prints the Home menu screen.
 */
void print_homescreen(void);

/**
 * @brief Handles user interaction in the Home menu.
 *
 * Reads and validates the user input and returns the next UI state.
 *
 * @return Next UI state depending on user selection.
 */
ui_state home_menu(void);

#endif /* HOME_H */