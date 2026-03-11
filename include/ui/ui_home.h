#ifndef UI_HOME_H
#define UI_HOME_H

/**
 * @file ui_home.h
 * @brief Home menu UI (main navigation screen).
 *
 * The Home menu is the central navigation point of the application.
 * It allows switching to:
 * - Simulation menu
 * - Configuration menu
 * - Storage menu
 * - Exit the application
 */

#include "ui.h"

typedef enum
{
    HOME_MENU_EXIT = 0,
    HOME_MENU_SIMULATION = 1,
    HOME_MENU_CONFIG = 2,
    HOME_MENU_STORAGE = 3,
    HOME_MENU_HELP = 4
}home_menu_choice;

/**
 * @brief Maximum valid menu number on the Home screen.
 *
 * Valid input range:
 * 0 .. HOME_MAX_VALID_NUMBER
 */
#define HOME_MAX_VALID_NUMBER (4)

/**
 * @brief Prints the Home menu screen.
 *
 * Clears the terminal and prints the available navigation options.
 */
void print_homescreen(void);

/**
 * @brief Handles user interaction in the Home menu.
 *
 * This function:
 * - prints the home screen
 * - reads the user input
 * - validates the selection
 * - returns the next UI state
 *
 * @return Next UI state depending on user selection.
 */
ui_state home_menu(void);

#endif /* UI_HOME_H */