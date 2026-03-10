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
    const uint16_t max_entries_per_tick = calc_max_possible_entries_per_tick(p_settings);

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
    printf("Name                 : %s\n", p_settings->name);
    printf("Capacity / Floor     : %u\n", (unsigned)p_settings->capacity);
    printf("Floors               : %u\n", (unsigned)p_settings->floors);
    printf("Gates                : %u\n", (unsigned)p_settings->gates);
    printf("Gate Entry Time (sec): %u\n", (unsigned)p_settings->gate_entry_inSec);
    printf("Tick Length (sec)    : %u\n", (unsigned)p_settings->tick_inSec);
    if (max_entries_per_tick > 0U)
    {
        printf("Max Gate Entries/Tick: %u\n", (unsigned)max_entries_per_tick);
    }
    else
    {
        printf("Max Gate Entries/Tick: INVALID CONFIG\n");
    }
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

        ui_statistics_print_header(p_settings);

        if (simulation_start(p_simulation) != OK)
        {
            printf("Simulation execution failed.\n");
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

/* ========================================================================= */
/* Statistics-print functions - used by Backend                                */
/* ========================================================================= */
void print_StatsTick_backend(const StatsTick *p_current_tick, const Settings *p_settings)
{
    ui_statistics_print_tick(p_current_tick, p_settings);
}

void print_final_stats_backend(const StatsSummary *p_stats_summary, const Settings *p_settings)
{
    ui_statistics_print_final(p_stats_summary, p_settings);
}