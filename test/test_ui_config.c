/**
 * @file Test_ui_config.c
 * @brief Unit tests for the configuration menu module.
 *
 * This file contains assert-based unit tests for the public functions
 * implemented in ui_config.c. The tests verify correct behaviour of the
 * configuration screen, menu navigation, and modification of the active
 * Settings object.
 *
 * Directly tested functionality includes:
 * - conversion of OutputMode values to readable strings
 * - printing the configuration screen
 * - editing configuration values via the config menu
 * - validation and update of Settings fields
 * - loading settings through the configuration menu
 *
 * Indirectly tested functionality:
 * The module ui_config.c contains a large number of static helper functions
 * used internally for parsing, validation and menu handling. Since static
 * functions are not visible outside their translation unit, they cannot be
 * called directly from this test file. Instead, their behaviour is verified
 * indirectly by executing the corresponding config_menu() code paths that
 * rely on them.
 *
 * Indirectly covered static helper functions include:
 * - parse_long()
 * - parse_float()
 * - read_long_in_range()
 * - read_float_nonnegative()
 * - ui_settings_set_name()
 * - apply_mode_select()
 * - edit_mode_select()
 * - is_time_config_valid()
 * - find_prev_valid_tick()
 * - find_next_valid_tick()
 * - resolve_tick_gate_conflict()
 * - is_parking_time_config_valid()
 * - load_settings_from_path()
 * - load_previous_settings()
 * - load_custom_settings_prompt()
 * - load_settings_menu_prompt()
 *
 * These helpers are exercised through targeted input sequences that trigger
 * the corresponding configuration menu branches.
 *
 * All tests simulate stdin and stdout using temporary files in order to
 * provide deterministic input sequences and to capture terminal output.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui.h"
#include "ui/ui_config.h"
#include "Settings.h"

/* ------------------------------------------------------------------------- */
/* Test helpers (copied from Test_ui.c)                                      */
/* ------------------------------------------------------------------------- */

static FILE *set_stdin_text(const char *p_text)
{
    FILE *p_tmp = tmpfile();
    assert(p_tmp != NULL);

    if (p_text != NULL)
    {
        fputs(p_text, p_tmp);
    }

    rewind(p_tmp);
    assert(dup2(fileno(p_tmp), fileno(stdin)) >= 0);
    clearerr(stdin);
    return p_tmp;
}

static FILE *begin_capture_stdout(int *p_saved_stdout_fd)
{
    FILE *p_tmp = tmpfile();
    assert(p_tmp != NULL);

    fflush(stdout);
    *p_saved_stdout_fd = dup(fileno(stdout));
    assert(*p_saved_stdout_fd >= 0);
    assert(dup2(fileno(p_tmp), fileno(stdout)) >= 0);
    clearerr(stdout);

    return p_tmp;
}

static void end_capture_stdout(FILE *p_tmp, int saved_stdout_fd,
                               char *p_buffer, size_t buffer_len)
{
    size_t read_len = 0U;

    assert(p_tmp != NULL);
    assert(p_buffer != NULL);
    assert(buffer_len > 0U);

    fflush(stdout);
    assert(dup2(saved_stdout_fd, fileno(stdout)) >= 0);
    close(saved_stdout_fd);

    rewind(p_tmp);
    read_len = fread(p_buffer, 1U, buffer_len - 1U, p_tmp);
    p_buffer[read_len] = '\0';

    fclose(p_tmp);
}

/* ------------------------------------------------------------------------- */
/* Helper Function                                                           */
/* ------------------------------------------------------------------------- */

static void init_test_settings(Settings *p_settings)
{
    assert(p_settings != NULL);

    memset(p_settings, 0, sizeof(*p_settings));

    p_settings->src_path = strdup("./config.json");
    p_settings->stats_path = strdup("./stats.txt");

    strcpy(p_settings->name, "InitialName");

    p_settings->capacity = 100U;
    p_settings->floors = 2U;
    p_settings->gates = 1U;
    p_settings->gate_entry_inSec = 5U;
    p_settings->tick_inSec = 10U;
    p_settings->min_parking_ticks = 2U;
    p_settings->max_parking_ticks = 10U;
    p_settings->entry_probability_perSec_prec = 5.0f;
    p_settings->max_ticks = 100;
    p_settings->rand_seed = 1;
    p_settings->output_mode = NORMAL;
}

/* ------------------------------------------------------------------------- */
/* Tests                                                                     */
/* ------------------------------------------------------------------------- */

static void test_output_mode_to_string(void)
{
    /* Test 1: NORMAL should map correctly */
    assert(strcmp(output_mode_to_string(NORMAL), "NORMAL") == 0);

    /* Test 2: VERBOSE should map correctly */
    assert(strcmp(output_mode_to_string(VERBOSE), "VERBOSE") == 0);

    /* Additional checks */
    assert(strcmp(output_mode_to_string(NONE), "NONE") == 0);
    assert(strcmp(output_mode_to_string(DEBUG), "DEBUG") == 0);
}

static void test_print_configscreen(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[5000];
    Settings settings = {0};

    init_test_settings(&settings);

    p_out = begin_capture_stdout(&saved_fd);
    assert(print_configscreen(&settings) == OK);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: title should be printed */
    assert(strstr(buffer, "CONFIG MENU") != NULL);

    /* Test 2: settings name should be printed */
    assert(strstr(buffer, "InitialName") != NULL);

    /* Additional checks */
    assert(strstr(buffer, "Capacity / Floor") != NULL);
    assert(strstr(buffer, "Output Mode") != NULL);
}

static void test_config_menu_back(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* Input sequence:
     * - 0 -> select "Back to Home"
     *
     * Indirectly covers:
     * - no local static helper function directly
     * - verifies basic menu handling in config_menu()
     */
    p_in = set_stdin_text("0\n");
    assert(config_menu(&settings) == UI_HOME);
    fclose(p_in);
}

static void test_config_menu_name_edit(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* Input sequence:
     * - 1       -> select "Name"
     * - NewName -> enter new settings name
     *
     * Indirectly covers:
     * - ui_settings_set_name
     */
    p_in = set_stdin_text("1\nNewName\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(strcmp(settings.name, "NewName") == 0);
}
static void test_config_menu_numeric_edits(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("2\n50\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.capacity == 50U);

    p_in = set_stdin_text("3\n4\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.floors == 4U);

    p_in = set_stdin_text("4\n2\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.gates == 2U);

    p_in = set_stdin_text("11\n200\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.max_ticks == 200);

    p_in = set_stdin_text("12\n123\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.rand_seed == 123);
}

static void test_config_menu_gate_conflict_resolution(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* Input sequence:
     * - 5 -> select "Gate Entry Time"
     * - 6 -> enter new gate entry time
     * - 2 -> choose automatic tick adjustment
     * - 2 -> choose next valid tick
     *
     * Indirectly covers:
     * - read_long_in_range
     * - parse_long
     * - is_time_config_valid
     * - resolve_tick_gate_conflict
     * - find_prev_valid_tick
     * - find_next_valid_tick
     */
    p_in = set_stdin_text("5\n6\n2\n2\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.gate_entry_inSec == 6U);
    assert(settings.tick_inSec == 12U);
}
static void test_config_menu_parking_time_validation(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("8\n20\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.min_parking_ticks == 2U);

    p_in = set_stdin_text("9\n25\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.max_parking_ticks == 25U);
}



static void test_config_menu_entry_probability(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("10\n2\n60\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.entry_probability_perSec_prec > 0.0f);
    assert(settings.entry_probability_perSec_prec == 100.0f);
}


static void test_config_menu_output_mode(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("13\n2\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.output_mode == VERBOSE);
}

static void test_config_menu_load_previous_settings(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("14\n1\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
}

static void test_config_menu_load_custom_settings(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("14\n2\n./config.json\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
}
void test_ui_config(void)
{
    test_output_mode_to_string();
    test_print_configscreen();
    test_config_menu_back();
    test_config_menu_name_edit();
    test_config_menu_numeric_edits();
    test_config_menu_gate_conflict_resolution();
    test_config_menu_parking_time_validation();
    test_config_menu_entry_probability();
    test_config_menu_output_mode();
    test_config_menu_load_previous_settings();
    test_config_menu_load_custom_settings();

    printf("All ui_config.c tests passed successfully!\n");
}