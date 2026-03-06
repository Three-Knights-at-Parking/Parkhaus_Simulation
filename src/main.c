#include <stdio.h>
#include <stdlib.h>

#include "../include/ui/ui.h"
#include "../include/Settings.h"

int main() {
    /* Temporary Solution */
    Settings* p_settings = malloc(sizeof(Settings));
    if (p_settings == NULL) {
        printf("malloc failed");
        return 1;
    }

    if (settings_init(p_settings,
                      "config.json",
                      "Rauenegg",
                      100,
                      1,
                      1,
                      60,
                      NORMAL,
                      1440,
                      -1) != OK)
    {
        return 1;
    }

    /* UI-only defaults until dedicated setters exist */
    settings.gate_entry_inSec = 5;
    settings.tick_inSec = 10;
    settings.entry_probability_perSec_prec = 75.0f;
    settings.max_parking_ticks = 10;
    settings.min_parking_ticks = 1;

    ui_start();

    (void)delete_settings(&settings);
    return 0;
}