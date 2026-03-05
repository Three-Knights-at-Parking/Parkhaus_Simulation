/**
* @file ui_home.c
 * @brief Home menu implementation (main navigation).
 *
 * The home menu is responsible for:
 * - printing the home screen
 * - reading and validating the user's menu choice
 * - returning the selected next UI state
 */

#include "../include/ui/ui.h"
#include "../include/ui/ui_home.h"

#include <stdio.h>

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
    printf("0 - Quit\n");
    printf("\n");
}
