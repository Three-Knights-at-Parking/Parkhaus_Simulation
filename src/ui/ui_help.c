/**
 * @file ui_help.c
 * @brief Help menu implementation.
 *
 * This module prints explanatory pages for the simulation model
 * and the configurable settings, and handles help-menu navigation.
 */
#include <stdio.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_help.h"

/* ========================================================================= */
/* Screen printing                                                           */
/* ========================================================================= */

void print_helpscreen(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("              HELP MENU\n");
    printf("====================================\n");
    printf("1 - Simulation Model\n");
    printf("2 - Settings Explanation\n");
    printf("0 - Back to Home\n");
    printf("\n");
}

void print_help_simulation(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("         HELP: SIMULATION MODEL\n");
    printf("====================================\n\n");

    printf("Overview\n");
    printf("------------------------------------\n");
    printf("This program simulates a parking garage.\n");
    printf("Vehicles arrive, may wait in queues, enter through gates,\n");
    printf("park for a configurable duration and leave afterwards.\n\n");

    printf("Time Model\n");
    printf("------------------------------------\n");
    printf("The simulation runs in discrete time steps called ticks.\n");
    printf("Each tick represents a configurable number of seconds.\n");
    printf("The tick length must be a multiple of the gate entry time\n");
    printf("so that only whole gate operations occur per tick.\n\n");

    printf("Arrival Model\n");
    printf("------------------------------------\n");
    printf("The user enters an expected arrival rate in vehicles per\n");
    printf("second, minute or hour.\n");
    printf("This value is converted internally into a per-second\n");
    printf("percentage-based entry value used by the simulation.\n\n");

    printf("Parking Duration\n");
    printf("------------------------------------\n");
    printf("Each vehicle receives a parking duration between the\n");
    printf("configured minimum and maximum parking ticks.\n");
    printf("Real parking time depends on both parking ticks and the\n");
    printf("configured tick length.\n\n");

    printf("Statistics\n");
    printf("------------------------------------\n");
    printf("The simulation tracks values such as occupancy, queue length,\n");
    printf("arrivals, entries, departures, waiting times and parking\n");
    printf("durations for each tick and for the final summary.\n\n");

    printf("Press ENTER to return to the Help Menu...\n");
}

void print_help_settings(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("      HELP: SETTINGS EXPLANATION\n");
    printf("====================================\n\n");

    printf("Name\n");
    printf("    Name of the parking garage / simulation setup.\n\n");

    printf("Capacity / Floor\n");
    printf("    Number of parking spaces available on each floor.\n\n");

    printf("Floors\n");
    printf("    Number of parking levels in the garage.\n\n");

    printf("Gates\n");
    printf("    Number of entry gates used by the simulation.\n\n");

    printf("Gate Entry Time (sec)\n");
    printf("    Time required for one vehicle to pass a gate.\n\n");

    printf("Tick Length (sec)\n");
    printf("    Duration of one simulation tick in seconds.\n");
    printf("    Must be divisible by Gate Entry Time.\n\n");

    printf("Min Parking Ticks\n");
    printf("    Minimum number of ticks a vehicle will remain parked.\n\n");

    printf("Max Parking Ticks\n");
    printf("    Maximum number of ticks a vehicle may remain parked.\n\n");

    printf("Entry Prob / Sec (%%)\n");
    printf("    Internal per-second percentage-based entry value.\n");
    printf("    It is derived from the user-defined arrival rate.\n\n");

    printf("Max Ticks\n");
    printf("    Total number of simulation ticks to execute.\n\n");

    printf("Random Seed\n");
    printf("    Controls reproducibility of random behaviour.\n");
    printf("    Using the same seed produces the same simulation.\n\n");

    printf("Output Mode\n");
    printf("    Controls how much simulation information is printed.\n");
    printf("    NONE, NORMAL, VERBOSE or DEBUG.\n\n");

    printf("Press ENTER to return to the Help Menu...\n");
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state help_menu(void)
{
    int choice = 0;
    validation_flag valid = INVALID;

    while (1)
    {
        print_helpscreen();

        valid = INVALID;

        while (valid != VALID)
        {
            choice = user_input();
            valid = validate_user_input(choice, HELP_MAX_VALID_NUMBER);
        }

        if (choice == HELP_MENU_SIMULATION)
        {
            print_help_simulation();
            press_enter_to_continue();
        }
        else if (choice == HELP_MENU_SETTINGS)
        {
            print_help_settings();
            press_enter_to_continue();
        }
        else
        {
            return UI_HOME;
        }
    }
}