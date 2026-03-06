#include "../include/ui/ui_simulation.h"
#include <stdio.h>

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
