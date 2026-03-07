#include <stdio.h>
#include <stdlib.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_config.h"
#include "../include/ui/ui_simulation.h"
#include "../include/ui/ui_statistics.h"

#include "../include/simulation.h"
#include "../include/types.h"

//Function prototypes
static int print_simulation_statistics(const Settings *p_settings);

/* ========================================================================= */
/* Global handover storage (owned by simulation/data layer)                  */
/* ========================================================================= */

static StatList *pStatList = NULL;
static StatsSummary *pStatsSummary = NULL;

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


void post_simulation_prompt(const char *p_sim_output_path) {
    printf("Post Simulation Prompt (Stub)\n");
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state simulation_menu(Settings *p_settings, Simulation *p_simulation) {
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

    int choice = 0;
    validation_flag valid = INVALID;

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, SIMULATION_MAX_VALID_NUMBER);
    }

    if (choice == 1) {
        printf("Starting simulation...\n");

        /* Reset handover pointers before each new run. */
        pStatList = NULL;
        pStatsSummary = NULL;

        if (simulation_start(p_simulation) != OK)
        {
            printf("Simulation execution failed.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return UI_SIMULATION;
        }
    }

    /* Defensive fallback */
    return UI_SIMULATION;
}

char *start_simulation(Settings *p_settings) {
    return NULL;
}

void hand_over_simulationdata(struct StatList *p_stat_list) {
}

void hand_over_endstatistics(struct StatsSummary *p_summary) {
}
