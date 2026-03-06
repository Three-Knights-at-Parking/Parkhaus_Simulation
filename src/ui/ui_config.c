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
/* Local helpers                                                             */
/* ========================================================================= */

static void trim_newline(char *p_text)
{
    if (p_text == NULL)
    {
        return;
    }

    const size_t len = strlen(p_text);

    if (len == 0U)
    {
        return;
    }

    if (p_text[len - 1U] == '\n')
    {
        p_text[len - 1U] = '\0';
    }
}

static int read_line(char *p_buffer, const size_t buffer_len)
{
    if (p_buffer == NULL || buffer_len == 0U)
    {
        return -1;
    }

    if (fgets(p_buffer, buffer_len, stdin) == NULL)
    {
        return -1;
    }

    if (strchr(p_buffer, '\n') == NULL)
    {
        int c;

        while ((c = getchar()) != '\n' && c != EOF)
        {
            /* discard */
        }
    }

    trim_newline(p_buffer);
    return 0;
}

static int parse_long(const char *p_text, long *p_out)
{
    char *p_end = NULL;
    long value = 0;

    if (p_text == NULL || p_out == NULL)
    {
        return -1;
    }

    errno = 0;
    value = strtol(p_text, &p_end, 10);

    if (errno != 0)
    {
        return -1;
    }

    if (p_end == p_text)
    {
        return -1;
    }

    while (*p_end == ' ' || *p_end == '\t')
    {
        p_end++;
    }

    if (*p_end != '\0')
    {
        return -1;
    }

    *p_out = value;
    return 0;
}

static int parse_float(const char *p_text, float *p_out)    //Maybe possible to synthesize with other parsing functions
{
    char *p_end = NULL;
    float value = 0.0f;

    if (p_text == NULL || p_out == NULL)
    {
        return -1;
    }

    errno = 0;
    value = strtof(p_text, &p_end);

    if (errno != 0)
    {
        return -1;
    }

    if (p_end == p_text)
    {
        return -1;
    }

    while (*p_end == ' ' || *p_end == '\t')
    {
        p_end++;
    }

    if (*p_end != '\0')
    {
        return -1;
    }

    *p_out = value;
    return 0;
}

/* ------------------------------------------------------------------------- */
/* Generic integer input helper                                              */
/* ------------------------------------------------------------------------- */

static int read_long_in_range(const char *p_prompt, long min_val, long max_val, long *p_out)
{
    char buffer[64];
    long value = 0;

    if (p_out == NULL)
    {
        return -1;
    }

    while (1)
    {
        printf("%s", p_prompt);

        if (read_line(buffer, sizeof(buffer)) != 0)
        {
            printf("Input error.\n");
            continue;
        }

        if (parse_long_value(buffer, &value) != 0)
        {
            printf("Your input is not a valid integer!\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        if (value < min_val || value > max_val)
        {
            printf("Value must be between %ld and %ld.\n", min_val, max_val);
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        *p_out = value;
        return 0;
    }
}

static int read_int32_in_range(const char *p_prompt, const int32_t min_val, const int32_t max_val, const int allow_negative, int32_t *p_out)
{
    char buffer[64];

    if (p_out == NULL)
    {
        return -1;
    }

    while (1)
    {
        printf("%s", p_prompt);

        if (read_line(buffer, sizeof(buffer)) != 0)
        {
            printf("Input error.\n");
            continue;
        }

        int32_t value = 0;
        if (parse_int32(buffer, &value) != 0)
        {
            printf("Your input is not a valid integer!\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        if (allow_negative == 0 && value < 0)
        {
            printf("Negative values are not allowed!\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        if (value < min_val || value > max_val)
        {
            printf("Value must be between %ld and %ld.\n", (long)min_val, (long)max_val);
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        *p_out = value;
        return 0;
    }
}

static int read_float_percent(const char *p_prompt, float *p_out)       //Maybe possible to synthesize function with read_int32_in_range()
{
    char buffer[64];

    if (p_out == NULL)
    {
        return -1;
    }

    while (1)
    {
        printf("%s", p_prompt);

        if (read_line(buffer, sizeof(buffer)) != 0)
        {
            printf("Input error.\n");
            continue;
        }

        float value = 0.0f;
        if (parse_float(buffer, &value) != 0)       //Implementing parse_float() in next step
        {
            printf("Your input is not a valid number!\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        if (value < MIN_PROB_PERCENT || value > MAX_PROB_PERCENT)
        {
            printf("Probability must be between %.1f and %.1f percent.\n", MIN_PROB_PERCENT, MAX_PROB_PERCENT);
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        *p_out = value;
        return 0;
    }
}

static int ui_settings_set_name(Settings *p_settings, const char *p_name)
{
    /* Settings.c has a private static settings_set_name().
       Until a public setter exists, we replicate the logic here. */

    if (p_settings == NULL || p_name == NULL)
    {
        return -1;
    }

    if (p_name[0] == '\0')
    {
        return -1;
    }

    size_t len = strlen(p_name);
    if (len > NAME_MAX_LEN)
    {
        len = NAME_MAX_LEN;
    }

    char *p_buf = (char *)malloc(len + 1U);
    if (p_buf == NULL)
    {
        return -1;
    }

    memcpy(p_buf, p_name, len);
    p_buf[len] = '\0';

    free(p_settings->name);
    p_settings->name = p_buf;
    return 0;
}

static const char *output_mode_to_string(const enum OutputMode mode)
{
    if (mode == NONE) return "NONE";
    if (mode == NORMAL) return "NORMAL";
    if (mode == VERBOSE) return "VERBOSE";
    if (mode == DEBUG) return "DEBUG";
    return "NORMAL";
}

static enum OutputMode apply_mode_select(const int mode_select)
{
    if (mode_select == 0) return NONE;
    if (mode_select == 1) return NORMAL;
    if (mode_select == 2) return VERBOSE;
    if (mode_select == 3) return DEBUG;
    return NORMAL;
}

static int edit_mode_select(void)
{
    int32_t choice = 0;

    clear_terminal();
    printf("Select Output Mode:\n");
    printf("------------------------------------\n");
    printf("0 = NONE\n");
    printf("1 = NORMAL\n");
    printf("2 = VERBOSE\n");
    printf("3 = DEBUG\n");
    printf("------------------------------------\n");

    (void)read_int32_in_range("Enter your choice (0 - 3): ", 0, 3, 0, &choice);
    return (int)choice;
}

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

ui_state config_menu(Settings *p_settings) {
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

    if (choice == 0)
    {
        return UI_HOME;
    }
    else if (choice == 1)
    {
        char name_buf[128];

        printf("Enter name (max %d chars): ", NAME_MAX_LEN);
        if (read_line(name_buf, sizeof(name_buf)) != 0 || name_buf[0] == '\0')
        {
            printf("Invalid name.\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            return UI_KONFIG;
        }

        if (ui_settings_set_name(p_settings, name_buf) != 0)
        {
            printf("Failed to set name (out of memory?).\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == 2)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter capacity per floor: ", MIN_CAPACITY, MAX_CAPACITY, 0, &value);
        (void)settings_set_size(p_settings, (uint16_t)value);
        return UI_KONFIG;
    }
    else if (choice == 3)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter number of floors: ", MIN_FLOORS, MAX_FLOORS, 0, &value);
        (void)settings_set_floors(p_settings, (uint8_t)value);
        return UI_KONFIG;
    }
    else if (choice == 4)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter number of gates: ", MIN_GATES, MAX_GATES, 0, &value);
        (void)settings_set_gates(p_settings, (uint8_t)value);
        return UI_KONFIG;
    }
    else if (choice == 5)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter gate entry time in seconds: ", MIN_GATE_ENTRY_SEC, MAX_GATE_ENTRY_SEC, 0, &value);

        /* TODO: replace with settings_set_gate_entry_inSec(p_settings, ...) when available */
        p_settings->gate_entry_inSec = (uint16_t)value;

        return UI_KONFIG;
    }
    else if (choice == 6)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter tick length in seconds: ", MIN_TICK_SEC, MAX_TICK_SEC, 0, &value);

        /* TODO: replace with settings_set_tick_inSec(p_settings, ...) when available */
        p_settings->tick_inSec = (uint16_t)value;

        return UI_KONFIG;
    }
    else if (choice == 7) {
        const int mode_select = edit_mode_select();
        const enum OutputMode mode = apply_mode_select(mode_select);
        (void)settings_set_output_mode(p_settings, mode);
        return UI_KONFIG;
    }
    else if (choice == 8) {
        float prob = 0.0f;
        (void)read_float_percent("Enter entry probability per second (0 - 100 %): ", &prob);

        /* TODO: replace with settings_set_entry_probability_perSec_prec(p_settings, ...) when available */
        p_settings->entry_probability_perSec_prec = prob;

        return UI_KONFIG;
    }
    else if (choice == 9)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter max ticks (-1/-2/... for day equivalents): ",
                                  MIN_MAX_TICKS, MAX_MAX_TICKS, 1, &value);
        (void)settings_set_max_ticks(p_settings, value);
        return UI_KONFIG;
    }
    else if (choice == 10)
    {
        int32_t value = 0;
        (void)read_int32_in_range("Enter random seed (or -1 for default/time): ",
                                  MIN_SEED, MAX_SEED, 1, &value);
        (void)settings_set_rand_seed(p_settings, value);
        return UI_KONFIG;
    }
}