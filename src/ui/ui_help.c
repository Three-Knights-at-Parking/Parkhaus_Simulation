/**
 * @file ui_help.c
 * @brief Help menu implementation.
 *
 * This module prints explanatory pages for the simulation model,
 * configurable settings, and file handling rules.
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
    printf("====================================\n\n");
    printf("What would you like to read about?\n\n");
    printf("  1 - Simulation Model & Overview\n");
    printf("  2 - Settings Explanation\n");
    printf("  3 - File & Path Handling\n");
    printf("  0 - Back to Home\n\n");
    printf("Choice: ");
}

void print_help_simulation(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("       HELP: SIMULATION MODEL\n");
    printf("====================================\n\n");

    printf("Overview\n");
    printf("------------------------------------\n");
    printf("This program is a discrete simulation of a parking garage.\n");
    printf("Vehicles arrive randomly, wait in queues if necessary, enter\n");
    printf("through gates, park for a randomized duration, and depart.\n\n");

    printf("Time Model (Ticks)\n");
    printf("------------------------------------\n");
    printf("The simulation runs in discrete time steps called 'ticks'.\n");
    printf("Each tick represents a configurable number of real seconds.\n");
    printf("The tick length must be a multiple of the gate entry time\n");
    printf("so that only whole gate operations occur per tick.\n\n");

    printf("Arrival & Parking Model\n");
    printf("------------------------------------\n");
    printf("Users define an expected arrival rate, which is converted\n");
    printf("internally into a per-second entry probability.\n");
    printf("Each vehicle receives a randomized parking duration bounded\n");
    printf("by the configured minimum and maximum parking ticks.\n\n");

    printf("Statistics Tracking\n");
    printf("------------------------------------\n");
    printf("The engine strictly tracks values such as occupancy, queue\n");
    printf("length, arrivals, and wait times. These are evaluated per\n");
    printf("tick and aggregated into a comprehensive final summary.\n\n");

    printf("Press ENTER to return to the Help Menu...\n");
}

void print_help_settings(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("      HELP: SETTINGS EXPLANATION\n");
    printf("====================================\n\n");

    printf("Capacity / Floors / Gates\n");
    printf("    Defines the physical layout and limits of the garage."
           "    Capacity is equal to the capacity per floor and the \n"
           "    total capacity is calculated by capacity*floors.\n\n");

    printf("Gate Entry Time & Tick Length (sec)\n");
    printf("    Controls the flow of time. Tick length must be evenly\n");
    printf("    divisible by the gate entry time.\n\n");

    printf("Min / Max Parking Ticks\n");
    printf("    The random bounds for how long a vehicle remains parked.\n\n");

    printf("Entry Prob / Sec (%%)\n");
    printf("    Internal per-second percentage-based entry value.\n");
    printf("    It is derived from the user-defined arrival rate.\n\n");

    printf("Max Ticks\n");
    printf("    Total number of simulation ticks to execute. Entering a negative \n"
           "    number -n will align the duration to the n-day equivalent of the tick duration you setup.\n\n");

    printf("Random Seed\n");
    printf("    Controls reproducibility. Using the exact same seed\n");
    printf("    and settings produces an identical simulation run. -1 uses the current timestamp\n\n");

    printf("Output Mode\n");
    printf("    Controls the verbosity of printed and saved statistics.\n");
    printf("    Options: NONE, NORMAL, VERBOSE.\n\n");

    printf("Press ENTER to return to the Help Menu...\n");
}

void print_help_file_handling(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("      HELP: FILE & PATH HANDLING\n");
    printf("====================================\n\n");

    printf("Settings Configuration (.json)\n");
    printf("------------------------------------\n");
    printf("Settings are saved and loaded using JSON files. By default,\n");
    printf("the application uses './config.json'. You can specify custom\n");
    printf("relative paths, but absolute paths are generally restricted.\n\n");

    printf("Statistics & Storage (.csv)\n");
    printf("------------------------------------\n");
    printf("Simulation statistics are saved as CSV files. To maintain\n");
    printf("a clean directory structure and ensure security, all stat\n");
    printf("files are strictly sandboxed into the './stats/' folder.\n\n");

    printf("Path Security Restrictions:\n");
    printf("  - Absolute paths (e.g., C:/... or /usr/...) are blocked.\n");
    printf("  - Upward directory traversal (e.g., ../) is blocked.\n\n");

    printf("If an invalid path is entered, the system safely ignores it\n");
    printf("and falls back to using the default: './stats/stats.csv'.\n\n");

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
        else if (choice == HELP_MENU_FILE_HANDLING)
        {
            print_help_file_handling();
            press_enter_to_continue();
        }
        else
        {
            return UI_HOME;
        }
    }
}