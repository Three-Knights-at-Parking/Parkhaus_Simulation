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

    printf("====================================\n");
    printf("             HOME MENU\n");
    printf("====================================\n\n");
    printf("What would you like to do?\n\n");
    printf("  1 - Simulation    (Run the parking simulation)\n");
    printf("  2 - Configuration (Edit settings like capacity, ticks, etc.)\n");
    printf("  3 - Storage       (Load and view past simulation statistics)\n");
    printf("  4 - Help          (View instructions and documentation)\n");
    printf("  0 - Quit          (Exit the application)\n\n");
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state home_menu()
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