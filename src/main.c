/**
* @file main.c
 * @brief Program entry point for the parking garage simulation.
 *
 * Allocates and initializes the main Settings and Simulation objects,
 * starts the terminal UI and performs final cleanup before exit.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../include/ui/ui.h"
#include "../include/Settings.h"
#include "../include/Simulation.h"
#include "ui/ui_config.h"
#include "ui/ui_help.h"
#include "ui/ui_home.h"
#include "ui/ui_simulation.h"
#include "ui/ui_storage.h"
#include "utils/SafteyUtils.h"

int main(void)
{
    Simulation *p_simulation = malloc(sizeof(Simulation));
    if (p_simulation == NULL)
    {
        printf("malloc (Simulation) failed\n");
        return ERROR;
    }

    Settings *p_settings = malloc(sizeof(Settings));
    if (p_settings == NULL)
    {
        printf("malloc (Settings) failed\n");
        free(p_simulation);
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
            SETTINGS_DEFAULT_IS_LEAVABLE,
            SETTINGS_DEFAULT_QUEUE_MAX_LENGTH
        ) != OK)
    {
        print_error_s("Failed to init settings object", HIGH);
        free(p_settings);
        free(p_simulation);
        return ERROR;
    }

    if (simulation_init(p_simulation, p_settings, NULL) != OK)
    {
        print_error_s("Failed to init simulation object", HIGH);
        delete_settings(p_settings);
        free(p_simulation);
        return ERROR;
    }
    // We now skip the UI part with the DEBUG flag active so we can debug faster and easier.
    // From a user POV this is quite retarded, because once enabled,
    // they need to manually change the value in the settings if something goes wrong.
    // oh well.
    //
    //
    // UPDATE: I hope this fixes it. Frankly depends on the way UI is implemented.
    if (p_settings->output_mode == DEBUG) {
        simulation_start(p_simulation);
        simulation_end(p_simulation);
        ui_state state = home_menu();

        while (state != UI_EXIT)
        {
            if (state == UI_HOME)
            {
                state = home_menu();
            }
            else if (state == UI_KONFIG)
            {
                state = config_menu(p_settings);
            }
            else if (state == UI_SIMULATION)
            {
                state = simulation_menu(p_settings, p_simulation);
            }
            else if (state == UI_STORAGE)
            {
                state = storage_menu();
            }
            else if (state == UI_HELP)
            {
                state = help_menu();
            }
            else
            {
                state = UI_HOME;
            }
        }
    }
    if (p_settings->output_mode != DEBUG) {
        ui_start(p_settings, p_simulation);
    }
    if (free_simulation(p_simulation) != OK)
    {
        delete_settings(p_settings);
        return ERROR;
    }

    return 0;
}
