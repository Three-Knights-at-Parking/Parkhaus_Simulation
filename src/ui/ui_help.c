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