/**
* @file ui_home.c
 * @brief Home menu implementation (main navigation).
 *
 * The home menu is responsible for:
 * - printing the home screen
 * - reading and validating the user's menu choice
 * - returning the selected next UI state
 */

#include <stdio.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_home.h"

/* ========================================================================= */
/* Screen printing                                                           */
/* ========================================================================= */

void print_homescreen(void)
{
    clear_terminal();

    printf("==============================\n");
    printf("         Home-Menu\n");
    printf("==============================\n");
    printf("1 - Simulation\n");
    printf("2 - Configuration\n");
    printf("3 - Storage\n");
    printf("4 - Help\n");
    printf("0 - Quit\n");
    printf("\n");
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state home_menu(Settings* p_settings)
{
    int choice = 0;
    validation_flag valid = INVALID;

    print_homescreen();

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, HOME_MAX_VALID_NUMBER);
    }

    /* Map numeric menu choice to UI state.
       The state machine in ui_start() will call the appropriate menu handler. */
    if (choice == HOME_MENU_SIMULATION)
    {
        return UI_SIMULATION;
    }
    else if (choice == HOME_MENU_CONFIG)
    {
        return UI_KONFIG;
    }
    else if (choice == HOME_MENU_STORAGE)
    {
        return UI_STORAGE;
    }
    else if (choice == HOME_MENU_HELP)
    {
        return UI_HELP;
    }
    else
    {
        return UI_EXIT;
    }
}