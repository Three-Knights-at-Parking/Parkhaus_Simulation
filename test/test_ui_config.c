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

    strcpy(p_settings->name, "InitialName");
    strcpy(p_settings->src_path, "./config.json");
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
/*-------------------------------------------------------------------------- */

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

static void test_print_configscreen(void) {
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[5000];
    Settings settings = {0};

    init_test_settings(&settings);
    strcpy(settings.name, "InitialName");

    p_out = begin_capture_stdout(&saved_fd);

    assert(print_configscreen(&settings) == OK);

    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: title should be printed */
    assert(strstr(buffer, "CONFIG MENU") != NULL);

    /* Test 2: settings name should be printed */
    assert(strstr(buffer, "TestConfig") != NULL);

    /* Additional checks */
    assert(strstr(buffer, "Capacity / Floor") != NULL);
    assert(strstr(buffer, "Output Mode") != NULL);
}

static void test_config_menu_back(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("0\n");
    assert(config_menu(&settings) == UI_HOME);
    fclose(p_in);
}

static void test_config_menu_name_edit(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

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

    /* Test 1: capacity edit */
    p_in = set_stdin_text("2\n50\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.capacity == 50U);

    /* Test 2: floors edit */
    p_in = set_stdin_text("3\n4\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.floors == 4U);

    /* Test 3: gates edit */
    p_in = set_stdin_text("4\n2\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.gates == 2U);

    /* Test 4: max ticks edit */
    p_in = set_stdin_text("10\n200\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.max_ticks == 200);

    /* Test 5: random seed edit */
    p_in = set_stdin_text("11\n123\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
    assert(settings.rand_seed == 123);
}

static void test_config_menu_gate_conflict_resolution(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* Start state: tick_length = 10, gate_entry_time = 5 (valid)
     * Change gate entry time to 6 -> invalid with tick 10
     * Then choose automatic tick adjustment and select next valid tick 12.
     *
     * Covers:
     * - parse_long()
     * - read_long_in_range()
     * - is_time_config_valid()
     * - resolve_tick_gate_conflict()
     * - find_prev_valid_tick()
     * - find_next_valid_tick()
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

    /* Set minimum parking ticks to a value greater than current max (=10).
     * This triggers is_parking_time_config_valid() and restores the old value.
     * An extra ENTER is needed for press_enter_to_continue().
     */
    p_in = set_stdin_text("7\n20\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.min_parking_ticks == 2U);

    /* Set maximum parking ticks to a valid new value. */
    p_in = set_stdin_text("8\n25\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.max_parking_ticks == 25U);
}

static void test_config_menu_entry_probability(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* Input sequence:
     * - 9  → select "Entry Probability"
     * - 2  → choose "Rate per minute"
     * - 60 → enter 60 vehicles/min
     * - \n → acknowledge conversion message
     *
     * Indirectly covers:
     * - edit_arrival_mode
     * - read_float_nonnegative
     * - parse_float
     * - convert_rate_to_prob_perc
     */
    p_in = set_stdin_text("9\n2\n60\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    /* Expected result:
     * - probability > 0
     * - 60/min → 100% per second
     */
    assert(settings.entry_probability_perSec_prec > 0.0f);
    assert(settings.entry_probability_perSec_prec == 100.0f);
}

static void test_config_menu_output_mode(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    p_in = set_stdin_text("12\n2\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);

    assert(settings.output_mode == VERBOSE);
}

static void test_config_menu_load_previous_settings(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* 13 -> load settings menu
     * 1  -> load previous settings
     * \n -> acknowledge success/failure message in load_settings_from_path()
     */
    p_in = set_stdin_text("13\n1\n\n");
    assert(config_menu(&settings) == UI_KONFIG);
    fclose(p_in);
}

static void test_config_menu_load_custom_settings(void)
{
    FILE *p_in = NULL;
    Settings settings;

    init_test_settings(&settings);

    /* 13 -> load settings menu
     * 2  -> custom path prompt
     * ./config.json -> non-empty path, so load_settings_from_path() is reached
     * \n -> acknowledge success/failure message in load_settings_from_path()
     */
    p_in = set_stdin_text("13\n2\n./config.json\n\n");
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