#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_config.h"

#include "../include/Settings.h"
#include "../include/types.h"

/* ========================================================================= */
/* Screen printing                                                           */
/* ========================================================================= */

void print_configscreen(const Settings *p_settings)
{
    clear_terminal();

    printf("====================================\n");
    printf("             CONFIG MENU\n");
    printf("====================================\n\n");

    printf("Current Settings\n");
    printf("------------------------------------\n");
    printf("1  Name                 : %s\n", (p_settings->name != NULL) ? p_settings->name : "Rauenegg");
    printf("2  Capacity / Floor     : %u\n", (unsigned)p_settings->capacity);
    printf("3  Floors               : %u\n", (unsigned)p_settings->floors);
    printf("4  Gates                : %u\n", (unsigned)p_settings->gates);
    printf("5  Gate Entry Time (sec): %u\n", (unsigned)p_settings->gate_entry_inSec);
    printf("6  Tick Length (sec)    : %u\n", (unsigned)p_settings->tick_inSec);
    printf("7  Output Mode          : %s\n", output_mode_to_string(p_settings->output_mode));
    printf("8  Entry Prob / Sec (%%)  : %.2f\n", p_settings->entry_probability_perSec_prec);
    printf("9  Max Ticks            : %ld\n", (long)p_settings->max_ticks);
    printf("10 Random Seed          : %ld\n", (long)p_settings->rand_seed);
    printf("------------------------------------\n");
    printf("0  Back to Home\n\n");
}

/* ========================================================================= */
/* Config menu                                                               */
/* ========================================================================= */

ui_state config_menu(void) {
    //Settings *p_settings = ui_get_settings(); //p_settings will be probably given as a parameter

    if (p_settings == NULL)
    {
        printf("Internal error: Settings not available.\n");
        printf("Press ENTER to return...\n");
        press_enter_to_continue();
        return UI_HOME;
    }

    print_configscreen(p_settings);

    int choice = 0;
    validation_flag valid = INVALID;

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, CONFIG_MAX_VALID_NUMBER);
    }
}