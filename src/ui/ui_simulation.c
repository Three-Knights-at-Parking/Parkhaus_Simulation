/**
* @file ui_simulation.c
 * @brief Simulation menu implementation and backend statistics forwarding.
 *
 * This module prints the simulation menu, starts the simulation
 * and forwards backend-triggered statistics output to ui_statistics.
 */

#include <stdio.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_config.h"
#include "../include/ui/ui_simulation.h"
#include "../include/ui/ui_statistics.h"

#include "../include/Simulation.h"
#include "../include/types.h"

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
    printf("Name                   : %s\n", p_settings->name);
    printf("Capacity / Floor       : %u\n", (unsigned)p_settings->capacity);
    printf("Floors                 : %u\n", (unsigned)p_settings->floors);
    printf("Gates                  : %u\n", (unsigned)p_settings->gates);
    printf("Gate Entry Time (sec)  : %u\n", (unsigned)p_settings->gate_entry_inSec);
    printf("Tick Length (sec)      : %u\n", (unsigned)p_settings->tick_inSec);
    printf("Min Parking Ticks      : %lu\n", (unsigned long)p_settings->min_parking_ticks);
    printf("Max Parking Ticks      : %lu\n", (unsigned long)p_settings->max_parking_ticks);
    printf("Entry Prob / Sec (%%)   : %.2f\n", p_settings->entry_probability_perSec_prec);
    printf("Max Ticks              : %ld\n", (long)p_settings->max_ticks);
    printf("Random Seed            : %ld\n", (long)p_settings->rand_seed);
    printf("Output Mode            : %s\n", output_mode_to_string(p_settings->output_mode));
    printf("------------------------------------\n");
    printf("1  Start Simulation\n");
    printf("2  Go to Configuration\n");
    printf("0  Back to Home\n\n");

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

    if (choice == SIMULATION_MENU_START)
    {
        printf("Starting simulation...\n");
        settings_save_to_file(p_settings, p_settings->src_path);
        ui_statistics_print_header(p_settings->output_mode);

        if (simulation_start(p_simulation) != OK)
        {
            printf("Simulation execution failed.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return UI_SIMULATION;
        }
        simulation_end(p_simulation);
        printf("Simulation finished.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();

        return UI_SIMULATION;
    }
    if (choice == SIMULATION_MENU_CONFIG)
    {
        return UI_KONFIG;
    }
    if (choice == SIMULATION_MENU_BACK)
    {
        return UI_HOME;
    }

    /* Defensive fallback */
    return UI_SIMULATION;
}
/* ========================================================================= */
/* Statistics-print functions - used by Backend                                */
/* ========================================================================= */
void print_StatsTick_backend(const StatsTick *p_current_tick, enum OutputMode output_mode)
{
    ui_statistics_print_tick(p_current_tick, output_mode);
}

void print_final_stats_backend(const StatsSummary *p_stats_summary, enum OutputMode output_mode)
{
    ui_statistics_print_final(p_stats_summary, output_mode);
}