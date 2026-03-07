#include <stdio.h>
#include <stdlib.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_config.h"
#include "../include/ui/ui_simulation.h"
#include "../include/ui/ui_statistics.h"

#include "../include/simulation.h"
#include "../include/types.h"

/* ========================================================================= */
/* Function prototypes                                                       */
/* ========================================================================= */

static int print_simulation_statistics(Settings *p_settings, Simulation *p_simulation);

/* ========================================================================= */
/* Screen printing                                                           */
/* ========================================================================= */

int print_simulationscreen(const Settings *p_settings)
{
    if (p_settings == NULL)
    {
        return ERROR;
    }

    clear_terminal();

    printf("====================================\n");
    printf("          SIMULATION MENU\n");
    printf("====================================\n\n");

    printf("Current Settings\n");
    printf("------------------------------------\n");
    printf("Name                 : %s\n",
           (p_settings->name != NULL) ? p_settings->name : SETTINGS_DEFAULT_NAME);
    printf("Capacity / Floor     : %u\n", (unsigned)p_settings->capacity);
    printf("Floors               : %u\n", (unsigned)p_settings->floors);
    printf("Gates                : %u\n", (unsigned)p_settings->gates);
    printf("Gate Entry Time (sec): %u\n", (unsigned)p_settings->gate_entry_inSec);
    printf("Tick Length (sec)    : %u\n", (unsigned)p_settings->tick_inSec);
    printf("Output Mode          : %s\n", output_mode_to_string(p_settings->output_mode));
    printf("Entry Prob / Sec (%%)  : %.2f\n", p_settings->entry_probability_perSec_prec);
    printf("Max Ticks            : %ld\n", (long)p_settings->max_ticks);
    printf("Random Seed          : %ld\n", (long)p_settings->rand_seed);
    printf("------------------------------------\n\n");

    printf("1 Start Simulation\n");
    printf("2 Go to Configuration\n");
    printf("0 Back to Home\n\n");

    return OK;
}

/* ========================================================================= */
/* Local helper functions                                                    */
/* ========================================================================= */

static int print_simulation_statistics(Settings *p_settings, Simulation *p_simulation)
{
    StatsTick *p_current_tick = NULL;
    StatList *p_stat_list = NULL;

    if (p_settings == NULL || p_simulation == NULL)
    {
        return ERROR;
    }

    p_stat_list = p_simulation->StatList;
    if (p_stat_list == NULL)
    {
        return ERROR;
    }

    /* Implement in ui_statistics.c */
    ui_statistics_print_header(p_settings);

    p_current_tick = p_stat_list->p_tick_head;

    while (p_current_tick != NULL)
    {
        /* Implement in ui_statistics.c */
        ui_statistics_print_tick(p_current_tick, p_settings);

        /* Optional later:
           press_enter_to_continue();
           if you want one tick per ENTER */
        p_current_tick = p_current_tick->p_next;
    }

    /* Requires StatsSummary integration into StatList, e.g. p_summary */
    if (p_stat_list->p_summary != NULL)
    {
        /* Implement in ui_statistics.c */
        ui_statistics_print_final(p_stat_list->p_summary, p_settings);
    }
    else
    {
        printf("Warning: No summary received.\n");
    }

    return OK;
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state simulation_menu(Settings *p_settings, Simulation *p_simulation)
{
    int choice = 0;
    validation_flag valid = INVALID;

    if (p_settings == NULL || p_simulation == NULL)
    {
        printf("Internal error: Settings or Simulation not available.\n");
        printf("Press ENTER to return...\n");
        press_enter_to_continue();
        return UI_HOME;
    }

    if (print_simulationscreen(p_settings) != OK)
    {
        printf("Loading simulation screen failed.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return UI_HOME;
    }

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, SIMULATION_MAX_VALID_NUMBER);
    }

    if (choice == 1)
    {
        printf("Starting simulation...\n");

        if (simulation_start(p_simulation) != OK)
        {
            printf("Simulation execution failed.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return UI_SIMULATION;
        }

        if (p_simulation->StatList == NULL)
        {
            printf("Error: No simulation data received from backend.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return UI_SIMULATION;
        }

        if (print_simulation_statistics(p_settings, p_simulation) != OK)
        {
            printf("Failed to print simulation statistics.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return UI_SIMULATION;
        }

        printf("Simulation finished.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();

        return UI_SIMULATION;
    }
    else if (choice == 2)
    {
        return UI_KONFIG;
    }
    else if (choice == 0)
    {
        return UI_HOME;
    }

    /* Defensive fallback */
    return UI_SIMULATION;
}