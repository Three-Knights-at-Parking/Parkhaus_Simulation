/**
* @file ui_config.c
 * @brief Configuration menu implementation for terminal-based settings editing.
 *
 * This module prints the configuration menu, validates user input
 * and applies changes to the active Settings object.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_config.h"

#include "../include/Settings.h"
#include "../include/types.h"

/* ========================================================================= */
/* Local helpers                                                             */
/* ========================================================================= */

// Function Prototypes
static int parse_long(const char *p_text, long *p_out);
static int parse_float(const char *p_text, float *p_out);
static int read_long_in_range(const char *p_prompt, long min_val, long max_val, long *p_out);
static int read_float_nonnegative(const char *p_prompt, float *p_out);
static int ui_settings_set_name(Settings *p_settings, const char *p_name);
static enum OutputMode apply_mode_select(const int mode_select);
static int edit_mode_select(void);
static int is_time_config_valid(uint16_t tick_in_sec, uint16_t gate_entry_in_sec);
static uint16_t find_prev_valid_tick(uint16_t current_tick, uint16_t gate_entry_in_sec);
static uint16_t find_next_valid_tick(uint16_t current_tick, uint16_t gate_entry_in_sec);
static int resolve_tick_gate_conflict(Settings *p_settings, int changed_field);
static int is_parking_time_config_valid(const Settings *p_settings);


static int parse_long(const char *p_text, long *p_out)
{
    char *p_end = NULL;
    long value = 0;

    if (p_text == NULL || p_out == NULL)
    {
        return ERROR;
    }

    errno = 0;
    value = strtol(p_text, &p_end, 10);

    if (errno != 0)
    {
        return ERROR;
    }

    if (p_end == p_text)
    {
        return ERROR;
    }

    while (*p_end == ' ' || *p_end == '\t')
    {
        p_end++;
    }

    if (*p_end != '\0')
    {
        return ERROR;
    }

    *p_out = value;
    return OK;
}

static int parse_float(const char *p_text, float *p_out)
{
    char *p_end = NULL;
    float value = 0.0f;

    if (p_text == NULL || p_out == NULL)
    {
        return ERROR;
    }

    errno = 0;
    value = strtof(p_text, &p_end);

    if (errno != 0)
    {
        return ERROR;
    }

    if (p_end == p_text)
    {
        return ERROR;
    }

    while (*p_end == ' ' || *p_end == '\t')
    {
        p_end++;
    }

    if (*p_end != '\0')
    {
        return ERROR;
    }

    *p_out = value;
    return OK;
}

static int read_long_in_range(const char *p_prompt, const long min_val, const long max_val, long *p_out)
{
    char buffer[64];
    long value = 0;

    if (p_out == NULL)
    {
        return ERROR;
    }

    while (1)
    {
        printf("%s", p_prompt);

        if (read_line(buffer, sizeof(buffer)) != OK)
        {
            printf("Input error.\n");
            continue;
        }

        if (parse_long(buffer, &value) != OK)
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
        return OK;
    }
}

static int read_float_nonnegative(const char *p_prompt, float *p_out)
{
    char buffer[64];

    if (p_out == NULL)
    {
        return ERROR;
    }

    while (1)
    {
        float value = 0.0f;

        printf("%s", p_prompt);

        if (read_line(buffer, sizeof(buffer)) != OK)
        {
            printf("Input error.\n");
            continue;
        }

        if (parse_float(buffer, &value) != OK)
        {
            printf("Your input is not a valid number!\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        if (value <= 0.0f)
        {
            printf("Value must be greater than 0.0.\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            continue;
        }

        *p_out = value;
        return OK;
    }
}

static int ui_settings_set_name(Settings *p_settings, const char *p_name)
{
    if (p_settings == NULL || p_name == NULL)
    {
        return ERROR;
    }

    if (p_name[0] == '\0')
    {
        return ERROR;
    }

    strncpy(p_settings->name, p_name, SETTINGS_NAME_MAX_LENGTH);
    p_settings->name[SETTINGS_NAME_MAX_LENGTH] = '\0';

    return OK;
}

/**
 * @brief Converts OutputMode enum to a readable string.
 *
 * @param[in] mode Output mode enum value.
 * @return Constant string representation.
 */
const char *output_mode_to_string(const enum OutputMode mode)
{
    if (mode == NONE) return "NONE";
    if (mode == NORMAL) return "NORMAL";
    if (mode == VERBOSE) return "VERBOSE";
    if (mode == DEBUG) return "DEBUG";
    return "NORMAL";
}

/**
 * @brief Maps numeric mode selection to OutputMode enum.
 *
 * @param[in] mode_select Number in range [0..3].
 * @return Corresponding OutputMode value.
 */
static enum OutputMode apply_mode_select(const int mode_select)
{
    if (mode_select == 0) return NONE;
    if (mode_select == 1) return NORMAL;
    if (mode_select == 2) return VERBOSE;
    if (mode_select == 3) return DEBUG;
    return NORMAL;
}


/**
 * @brief Shows output mode selection screen and returns selection in [0..3].
 *
 * @return Mode selection number (0..3).
 */
static int edit_mode_select(void)
{
    clear_terminal();

    printf("Select Output Mode:\n");
    printf("------------------------------------\n");
    printf("0 = NONE\n");
    printf("1 = NORMAL\n");
    printf("2 = VERBOSE\n");
    printf("3 = DEBUG\n");
    printf("------------------------------------\n");

    int choice = -1;
    validation_flag valid = INVALID;

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, 3);
    }
    return (int)choice;
}

static int is_time_config_valid(const uint16_t tick_in_sec, const uint16_t gate_entry_in_sec)
{
    if (gate_entry_in_sec == 0U)
    {
        return ERROR;
    }

    return ((tick_in_sec % gate_entry_in_sec) == 0U) ? OK : ERROR;
}

static uint16_t find_prev_valid_tick(const uint16_t current_tick, const uint16_t gate_entry_in_sec)
{
    if (gate_entry_in_sec == 0U)
    {
        return current_tick;
    }

    const uint16_t remainder = (uint16_t)(current_tick % gate_entry_in_sec);

    if (remainder == 0U)
    {
        return current_tick;
    }

    return (uint16_t)(current_tick - remainder);
}

static uint16_t find_next_valid_tick(const uint16_t current_tick, const uint16_t gate_entry_in_sec)
{
    if (gate_entry_in_sec == 0U)
    {
        return current_tick;
    }

    const uint16_t remainder = (uint16_t)(current_tick % gate_entry_in_sec);

    if (remainder == 0U)
    {
        return current_tick;
    }

    return (uint16_t)(current_tick + (gate_entry_in_sec - remainder));
}

static int resolve_tick_gate_conflict(Settings *p_settings,
                                       const int changed_field)
{
    int choice = -1;
    validation_flag valid = INVALID;

    uint16_t prev_valid = 0U;
    uint16_t next_valid = 0U;

    if (p_settings == NULL)
    {
        return ERROR;
    }

    if (is_time_config_valid(p_settings->tick_inSec,
                             p_settings->gate_entry_inSec) == OK)
    {
        return OK;
    }

    clear_terminal();

    printf("====================================\n");
    printf("        INVALID TIME SETTINGS\n");
    printf("====================================\n\n");

    printf("Current configuration is invalid:\n");
    printf("Tick Length      : %u sec\n", (unsigned)p_settings->tick_inSec);
    printf("Gate Entry Time  : %u sec\n\n", (unsigned)p_settings->gate_entry_inSec);

    printf("Reason:\n");
    printf("tick_inSec must be a multiple of gate_entry_inSec.\n");
    printf("Otherwise fractional entries per tick would occur.\n\n");

    if (changed_field == 6)
    {
        prev_valid = find_prev_valid_tick(p_settings->tick_inSec, p_settings->gate_entry_inSec);
        next_valid = find_next_valid_tick(p_settings->tick_inSec, p_settings->gate_entry_inSec);

        printf("Choose a valid Tick Length:\n");
        printf("1 - Use previous valid value: %u sec\n", (unsigned)prev_valid);
        printf("2 - Use next valid value    : %u sec\n", (unsigned)next_valid);
        printf("0 - Cancel and keep old configuration\n\n");

        while (valid != VALID)
        {
            choice = user_input();
            valid = validate_user_input(choice, 2);
        }

        if (choice == 1)
        {
            p_settings->tick_inSec = prev_valid;
            return OK;
        }

        if (choice == 2)
        {
            p_settings->tick_inSec = next_valid;
            return OK;
        }

        return ERROR;
    }

    if (changed_field == 5)
    {
        printf("The new gate entry time does not divide the current tick length.\n");
        printf("Please choose one of the following options:\n");
        printf("1 - Re-enter gate entry time\n");
        printf("2 - Adjust tick length automatically\n");
        printf("0 - Cancel\n\n");

        while (valid != VALID)
        {
            choice = user_input();
            valid = validate_user_input(choice, 2);
        }

        if (choice == 1)
        {
            return ERROR;
        }

        if (choice == 2)
        {
            prev_valid = find_prev_valid_tick(p_settings->tick_inSec, p_settings->gate_entry_inSec);
            next_valid = find_next_valid_tick(p_settings->tick_inSec, p_settings->gate_entry_inSec);

            clear_terminal();

            printf("Possible valid tick lengths for gate entry time %u sec:\n\n",(unsigned)p_settings->gate_entry_inSec);

            printf("1 - %u sec\n", (unsigned)prev_valid);
            printf("2 - %u sec\n", (unsigned)next_valid);
            printf("0 - Cancel\n\n");

            valid = INVALID;

            while (valid != VALID)
            {
                choice = user_input();
                valid = validate_user_input(choice, 2);
            }

            if (choice == 1)
            {
                p_settings->tick_inSec = prev_valid;
                return OK;
            }

            if (choice == 2)
            {
                p_settings->tick_inSec = next_valid;
                return OK;
            }
        }

        return ERROR;
    }

    return ERROR;
}

static float convert_rate_to_prob_perc(const float vehicles, const rate_input_mode mode)
{
    float cars_per_second = 0.0f;

    if (mode == RATE_PER_SECOND)
    {
        cars_per_second = vehicles;
    }
    else if (mode == RATE_PER_MINUTE)
    {
        cars_per_second = vehicles / 60.0f;
    }
    else if (mode == RATE_PER_HOUR)
    {
        cars_per_second = vehicles / 3600.0f;
    }

    return cars_per_second * 100.0f;
}

static int edit_arrival_rate_mode(void)
{
    int choice = -1;

    clear_terminal();

    printf("Arrival Rate Input Mode\n");
    printf("-----------------------\n");
    printf("1 - Vehicles per second\n");
    printf("2 - Vehicles per minute\n");
    printf("3 - Vehicles per hour\n\n");

    while (1)
    {
        choice = user_input();

        if (choice >= 1 && choice <= 3)
        {
            return choice;
        }

        printf("Please choose 1, 2 or 3.\n");
        printf("Press ENTER and try again...\n");
        press_enter_to_continue();
        clear_terminal();

        printf("Arrival Rate Input Mode\n");
        printf("-----------------------\n");
        printf("1 - Vehicles per second\n");
        printf("2 - Vehicles per minute\n");
        printf("3 - Vehicles per hour\n\n");
    }
}

static int is_parking_time_config_valid(const Settings *p_settings)
{
    if (p_settings == NULL)
    {
        return ERROR;
    }

    if (p_settings->min_parking_ticks < 1U)
    {
        return ERROR;
    }

    if (p_settings->max_parking_ticks < p_settings->min_parking_ticks)
    {
        return ERROR;
    }

    return OK;
}

/* ========================================================================= */
/* Screen printing                                                           */
/* ========================================================================= */

int print_configscreen(const Settings *p_settings)
{
    if (p_settings == NULL)
    {
        return ERROR;
    }

    clear_terminal();

    printf("====================================\n");
    printf("             CONFIG MENU\n");
    printf("====================================\n\n");

    printf("Current Settings\n");
    printf("------------------------------------\n");
    printf("1  Name                   : %s\n", p_settings->name);
    printf("2  Capacity / Floor       : %u\n", (unsigned)p_settings->capacity);
    printf("3  Floors                 : %u\n", (unsigned)p_settings->floors);
    printf("4  Gates                  : %u\n", (unsigned)p_settings->gates);
    printf("5  Gate Entry Time (sec)  : %u\n", (unsigned)p_settings->gate_entry_inSec);
    printf("6  Tick Length (sec)      : %u\n", (unsigned)p_settings->tick_inSec);
    printf("7  Min Parking Ticks      : %lu\n", (unsigned long)p_settings->min_parking_ticks);
    printf("8  Max Parking Ticks      : %lu\n", (unsigned long)p_settings->max_parking_ticks);
    printf("9  Entry Prob / Sec (%%)    : %.2f\n", p_settings->entry_probability_perSec_prec);
    printf("10 Max Ticks              : %ld\n", (long)p_settings->max_ticks);
    printf("11 Random Seed            : %ld\n", (long)p_settings->rand_seed);
    printf("12 Output Mode            : %s\n", output_mode_to_string(p_settings->output_mode));
    printf("------------------------------------\n");
    printf("0  Back to Home\n\n");

    return OK;
}

/* ========================================================================= */
/* Config menu                                                               */
/* ========================================================================= */

ui_state config_menu(Settings *p_settings)
{
    int choice = 0;
    validation_flag valid = INVALID;

    if (p_settings == NULL)
    {
        printf("Internal error: Settings not available.\n");
        printf("Press ENTER to return...\n");
        press_enter_to_continue();
        return UI_HOME;
    }

    if (print_configscreen(p_settings) != OK)
    {
        printf("Loading config screen failed. Return to Home.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return UI_HOME;
    }

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, CONFIG_MAX_VALID_NUMBER);
    }

    if (choice == CONFIG_MENU_BACK)
    {
        return UI_HOME;
    }
    else if (choice == CONFIG_MENU_NAME)
    {
        char name_buf[SETTINGS_NAME_MAX_LENGTH + 1];

        printf("Enter name (max %d chars): ", SETTINGS_NAME_MAX_LENGTH);

        if (read_line(name_buf, sizeof(name_buf)) != OK || name_buf[0] == '\0')
        {
            printf("Invalid name.\n");
            printf("Press ENTER and try again...\n");
            press_enter_to_continue();
            return UI_KONFIG;
        }

        if (ui_settings_set_name(p_settings, name_buf) != OK)
        {
            printf("Failed to set name.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_CAPACITY)
    {
        long value = 0;

        (void)read_long_in_range("Enter capacity per floor: ",
                                 SETTINGS_MINIMUM_CAPACITY,
                                 SETTINGS_MAXIMUM_CAPACITY,
                                 &value);

        if (settings_set_size(p_settings, (uint16_t)value) != OK)
        {
            printf("Failed to set capacity.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_FLOORS)
    {
        long value = 0;

        (void)read_long_in_range("Enter number of floors: ",
                                 SETTINGS_MINIMUM_FLOORS,
                                 SETTINGS_MAXIMUM_FLOORS,
                                 &value);

        if (settings_set_floors(p_settings, (uint8_t)value) != OK)
        {
            printf("Failed to set floors.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_GATES)
    {
        long value = 0;

        (void)read_long_in_range("Enter number of gates: ",
                                 SETTINGS_MINIMUM_GATES,
                                 SETTINGS_MAXIMUM_GATES,
                                 &value);

        if (settings_set_gates(p_settings, (uint8_t)value) != OK)
        {
            printf("Failed to set gates.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_GATE_ENTRY_TIME)
    {
        const uint16_t old_gate_entry = p_settings->gate_entry_inSec;
        long value = 0;

        (void)read_long_in_range("Enter gate entry time in seconds: ",
                                 SETTINGS_MINIMUM_GATE_ENTRY_SEC,
                                 SETTINGS_MAXIMUM_GATE_ENTRY_SEC,
                                 &value);

        p_settings->gate_entry_inSec = (uint16_t)value;

        if (resolve_tick_gate_conflict(p_settings, CONFIG_MENU_GATE_ENTRY_TIME) != OK)
        {
            p_settings->gate_entry_inSec = old_gate_entry;

            printf("Change cancelled. Previous gate entry time restored.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_TICK_LENGTH)
    {
        const uint16_t old_tick = p_settings->tick_inSec;
        long value = 0;

        (void)read_long_in_range("Enter tick length in seconds: ",
                                 MIN_TICK_SEC,
                                 MAX_TICK_SEC,
                                 &value);

        p_settings->tick_inSec = (uint16_t)value;

        if (resolve_tick_gate_conflict(p_settings, CONFIG_MENU_TICK_LENGTH) != OK)
        {
            p_settings->tick_inSec = old_tick;

            printf("Change cancelled. Previous tick length restored.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_MIN_PARKING_TICKS)
    {
        const uint32_t old_min = p_settings->min_parking_ticks;
        long value = 0;

        (void)read_long_in_range("Enter minimum parking ticks: ",
                                  SETTINGS_MINIMUM_PARKING_TICKS,
                                 SETTINGS_MAXIMUM_PARKING_TICKS,
                                 &value);

        p_settings->min_parking_ticks = (uint32_t)value;

        if (is_parking_time_config_valid(p_settings) != OK)
        {
            p_settings->min_parking_ticks = old_min;

            printf("Invalid configuration.\n");
            printf("Minimum parking ticks must be <= maximum parking ticks.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_MAX_PARKING_TICKS)
    {
        const uint32_t old_max = p_settings->max_parking_ticks;
        long value = 0;

        (void)read_long_in_range("Enter maximum parking ticks: ",
                                  SETTINGS_MINIMUM_PARKING_TICKS,
                                 SETTINGS_MAXIMUM_PARKING_TICKS,
                                 &value);

        p_settings->max_parking_ticks = (uint32_t)value;

        if (is_parking_time_config_valid(p_settings) != OK)
        {
            p_settings->max_parking_ticks = old_max;

            printf("Invalid configuration.\n");
            printf("Maximum parking ticks must be >= minimum parking ticks.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_ENTRY_PROBABILITY)
    {
        float vehicles = 0.0f;
        const rate_input_mode mode = (rate_input_mode)edit_arrival_rate_mode();

        (void)read_float_nonnegative("Enter average arriving vehicles: ",
                                     &vehicles);

        p_settings->entry_probability_perSec_prec = convert_rate_to_prob_perc(vehicles, mode);

        printf("Converted probability per second: %.2f %%\n",
               p_settings->entry_probability_perSec_prec);
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_MAX_TICKS)
    {
        long value = 0;

        (void)read_long_in_range("Enter max ticks (-1/-2/... for day equivalents): ",
                                 SETTINGS_MAXIMUM_DAY_TICKS,
                                 SETTINGS_MAXIMUM_TICKS,
                                 &value);

        if (settings_set_max_ticks(p_settings, (int32_t)value) != OK)
        {
            printf("Failed to set max ticks.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_RANDOM_SEED)
    {
        long value = 0;

        (void)read_long_in_range("Enter random seed (or -1 for default/time): ",
                                 SETTINGS_DEFAULT_RAND_SEED,
                                 MAX_SEED,
                                 &value);

        if (settings_set_rand_seed(p_settings, (int32_t)value) != OK)
        {
            printf("Failed to set random seed.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }
    else if (choice == CONFIG_MENU_OUTPUT_MODE)
    {
        const int mode_select = edit_mode_select();
        const enum OutputMode mode = apply_mode_select(mode_select);

        if (settings_set_output_mode(p_settings, mode) != OK)
        {
            printf("Failed to set output mode.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
        }

        return UI_KONFIG;
    }

    return UI_KONFIG;
}