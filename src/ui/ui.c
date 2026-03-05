#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_home.h"
#include "../include/ui/ui_config.h"
#include "../include/ui/ui_simulation.h"
#include "../include/ui/ui_storage.h"

/* ========================================================================= */
/* Helper functions                                                          */
/* ========================================================================= */

static void wait_for_enter(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void clear_terminal(void)
{
    for (int i = 0; i < 40; i++)
    {
        printf("\n");
    }
}

/* ========================================================================= */
/* Shared input helpers                                                      */
/* ========================================================================= */

int user_input(void)
{
    char buffer[64];
    char *endptr = NULL;
    long value = 0;

    printf("Enter the number (int) you want to navigate to: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }

    errno = 0;
    value = strtol(buffer, &endptr, 10);

    /* No digits were found - endptr points to the first element of buffer*/
    if (endptr == buffer)
    {
        return -1;
    }

    /* Skip trailing whitespace */
    while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n')
    {
        endptr++;
    }

    /* Reject trailing garbage, e.g. "12abc" */
    if (*endptr != '\0')
    {
        return -1;
    }

    /* Overflow/underflow */
    if (errno != 0)
    {
        return -1;
    }

    /* Range check before cast */
    if (value < INT_MIN || value > INT_MAX)
    {
        return -1;
    }

    return (int)value;
}

validation_flag validate_user_input(const int user_choice, const int max_valid_number)
{
    if (user_choice < 0)
    {
        printf("Your input is not a valid integer!\n");
        printf("Please press ENTER and try again...\n");
        wait_for_enter();
        return INVALID;
    }

    if (user_choice > max_valid_number)
    {
        printf("The number you entered is invalid!\n");
        printf("Please only choose between the numbers displayed.\n");
        printf("Press ENTER and try again...\n");
        wait_for_enter();
        return INVALID;
    }

    return VALID;
}

/* ========================================================================= */
/* Welcome screen                                                            */
/* ========================================================================= */

ui_state welcome_message(void)
{
    clear_terminal();

    printf("=========================================\n");
    printf("     Parkhaus-Simulation Rauenegg\n");
    printf("=========================================\n\n");

    printf("[Welcome Message with brief description]\n\n");
    printf("Press ENTER to continue...\n");

    wait_for_enter();

    return UI_HOME;
}

/* ========================================================================= */
/* Main UI state machine                                                     */
/* ========================================================================= */

ui_state ui_start(void)
{
    ui_state state = welcome_message();

    while (state != UI_EXIT)
    {
        if (state == UI_HOME)
        {
            state = home_menu();
        }
        else if (state == UI_KONFIG)
        {
            state = config_menu();
        }
        else if (state == UI_SIMULATION)
        {
            state = simulation_menu();
        }
        else if (state == UI_STORAGE)
        {
            state = storage_menu();
        }
        else
        {
            state = UI_HOME;
        }
    }

    return UI_EXIT;
}