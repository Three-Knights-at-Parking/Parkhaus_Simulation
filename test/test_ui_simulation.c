/**
 * @file Test_ui_simulation.c
 * @brief Unit tests for the simulation menu module.
 *
 * This file contains assert-based unit tests for the functions implemented
 * in ui_simulation.c. The tests verify correct behaviour of the simulation
 * menu screen, basic menu navigation, and backend statistics forwarding.
 *
 * Tested functionality includes:
 * - printing the simulation menu screen
 * - handling simulation menu navigation
 * - forwarding backend statistics output to ui_statistics
 *
 * The backend statistics functions in this module only forward data to the
 * ui_statistics module. Therefore the tests verify that these forwarding
 * functions execute correctly for different output modes without errors.
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
#include "ui/ui_simulation.h"
#include "Settings.h"

/* ------------------------------------------------------------------------- */
/* Test helpers (copied from test_ui.c)                                      */
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

    strcpy(p_settings->name, "SimTest");
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

static void test_print_simulationscreen(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[5000];
    Settings settings = {0};

    init_test_settings(&settings);

    p_out = begin_capture_stdout(&saved_fd);

    assert(print_simulationscreen(&settings) == OK);

    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: title should be printed */
    assert(strstr(buffer, "SIMULATION MENU") != NULL);

    /* Test 2: settings name should be printed */
    assert(strstr(buffer, "SimTest") != NULL);

    /* Additional checks */
    assert(strstr(buffer, "Start Simulation") != NULL);
    assert(strstr(buffer, "Go to Configuration") != NULL);
}

static void test_simulation_menu(void)
{
    FILE *p_in = NULL;
    Settings settings = {0};
    Simulation simulation = {0};

    init_test_settings(&settings);

    /* Input sequence:
     * - 0 -> select "Back to Home"
     */
    p_in = set_stdin_text("0\n");
    assert(simulation_menu(&settings, &simulation) == UI_HOME);
    fclose(p_in);

    /* Input sequence:
     * - 2 -> select "Go to Configuration"
     */
    p_in = set_stdin_text("2\n");
    assert(simulation_menu(&settings, &simulation) == UI_KONFIG);
    fclose(p_in);
}

static void test_print_StatsTick_backend(void)
{
    StatsTick current_tick = {0};

    /* Test 1: call with NORMAL output mode */
    print_StatsTick_backend(&current_tick, NORMAL);
    assert(1);

    /* Test 2: call with VERBOSE output mode */
    print_StatsTick_backend(&current_tick, VERBOSE);
    assert(1);
}

static void test_print_final_stats_backend(void)
{
    StatsSummary stats_summary = {0};

    /* Test 1: call with NORMAL output mode */
    print_final_stats_backend(&stats_summary, NORMAL);
    assert(1);

    /* Test 2: call with VERBOSE output mode */
    print_final_stats_backend(&stats_summary, VERBOSE);
    assert(1);
}

void test_ui_simulation(void)
{
    test_print_simulationscreen();
    test_simulation_menu();
    test_print_StatsTick_backend();
    test_print_final_stats_backend();

    printf("All ui_simulation.c tests passed successfully!\n");
}