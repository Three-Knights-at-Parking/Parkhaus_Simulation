#include <stdio.h>
#include <stdlib.h>

#include "../include/ui/ui.h"
#include "../include/Settings.h"
#include "../include/Simulation.h"

int main() {
    /* Temporary Solution */
    Simulation *p_simulation = malloc(sizeof(Simulation));
    if (p_simulation == NULL)
    {
        printf("malloc (Simulation) failed");
        return ERROR;
    }
    Settings *p_settings = malloc(sizeof(Settings));
    if (p_settings == NULL)
    {
        printf("malloc failed");
        return ERROR;
    }

    if (settings_init(
        p_settings,
                      SETTINGS_DEFAULT_PATH,
                      SETTINGS_DEFAULT_NAME,
                      SETTINGS_DEFAULT_CAPACITY,
                      SETTINGS_DEFAULT_FLOORS,
                      SETTINGS_DEFAULT_GATES,
                      SETTINGS_DEFAULT_REAL_EQUIVALENT,
                      SETTINGS_DEFAULT_OUTPUT_MODE,
                      SETTINGS_DEFAULT_MAX_TICKS,
                      SETTINGS_DEFAULT_RAND_SEED,
                      SETTINGS_DEFAULT_GATE_ENTRY_SEC,
                      SETTINGS_DEFAULT_TICK_SEC,
                      SETTINGS_DEFAULT_MAXIMUM_PARKING_TICKS,
                      SETTINGS_DEFAULT_MINIMUM_PARKING_TICKS,
                      SETTINGS_DEFAULT_MODE_SELECT,
                      SETTINGS_DEFAULT_ENTRY_PROB,
                      SETTINGS_DEFAULT_IS_LEAVABLE
        ) != OK)
    {
        return ERROR;
    }
    if (simulation_init(p_simulation, p_settings, NULL) != OK)
    {
        return ERROR;
    }

    ui_start(p_settings);

    if (free_simulation(p_simulation) != OK)
    {
        return ERROR;
    }
    if (delete_settings(p_settings) != OK)
    {
        return ERROR;
    }

    return 0;
}