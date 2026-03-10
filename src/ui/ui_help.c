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