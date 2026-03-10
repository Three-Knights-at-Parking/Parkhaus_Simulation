//
// Created by Daniel_Work on 10.03.2026.
//
/**
 * @file ui_help.c
 * @brief Help menu implementation.
 */

#include "../include/ui/ui.h"
#include "../include/ui/ui_help.h"

#include <stdio.h>

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
    printf("park for a certain time and leave again afterwards.\n\n");

    printf("Time Model\n");
    printf("------------------------------------\n");
    printf("The simulation runs in discrete time steps called ticks.\n");
    printf("Each tick represents a configurable number of seconds.\n");
    printf("The tick length must be a multiple of the gate entry time\n");
    printf("so that only whole vehicle entries occur per tick.\n\n");

    printf("Arrival Model\n");
    printf("------------------------------------\n");
    printf("Vehicle arrivals are configured as a probability per second.\n");
    printf("The user may enter an arrival rate in vehicles per second,\n");
    printf("minute or hour. The value is converted internally into a\n");
    printf("probability percentage per second.\n\n");

    printf("Statistics\n");
    printf("------------------------------------\n");
    printf("The simulation tracks values such as occupancy, queue length,\n");
    printf("arrivals, departures, waiting times, parking durations and more...\n\n");

    printf("Press ENTER to return to the Help Menu...\n");
}

void print_help_settings(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("      HELP: SETTINGS EXPLANATION\n");
    printf("====================================\n\n");

    printf("Capacity / Floor\n");
    printf("    Number of parking spaces on each floor.\n\n");

    printf("Floors\n");
    printf("    Number of parking levels in the garage.\n\n");

    printf("Gates\n");
    printf("    Number of entry gates. More gates may reduce queues.\n\n");

    printf("Gate Entry Time (sec)\n");
    printf("    Time required for one vehicle to pass a gate.\n\n");

    printf("Tick Length (sec)\n");
    printf("    Duration of one simulation step.\n");
    printf("    Must be divisible by Gate Entry Time.\n\n");

    printf("Max Gate Entries / Tick\n");
    printf("    Maximum number of vehicles that can pass all gates\n");
    printf("    during one tick based on the time model.\n\n");

    printf("Arrival Prob / Sec (%%)\n");
    printf("    Probability of vehicle arrival per second.\n");
    printf("    Derived from the user-defined arrival rate.\n\n");

    printf("Output Mode\n");
    printf("    Controls how much simulation information is printed.\n");
    printf("    NONE, NORMAL, VERBOSE or DEBUG.\n\n");

    printf("Max Ticks\n");
    printf("    Total number of simulation steps.\n\n");

    printf("Random Seed\n");
    printf("    Controls reproducibility of random behaviour.\n");
    printf("    Using the same seed produces the same simulation.\n\n");

    printf("Press ENTER to return to the Help Menu...\n");
}
