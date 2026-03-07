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

void print_simulationscreen(const Settings *p_settings) {
    printf("Simulation Screen (Stub)\n");
}

void post_simulation_prompt(const char *p_sim_output_path) {
    printf("Post Simulation Prompt (Stub)\n");
}

ui_state simulation_menu(Settings *p_settings) {
    printf("Simulation Menu (Stub)\n");
    return UI_HOME;
}

char *start_simulation(Settings *p_settings) {
    return NULL;
}

void hand_over_simulationdata(struct StatList *p_stat_list) {
}

void hand_over_endstatistics(struct StatsSummary *p_summary) {
}
