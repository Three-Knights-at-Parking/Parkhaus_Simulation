/**
 * @file test_ui_statistics.c
 * @brief Unit tests for the statistics output module.
 *
 * This file contains assert-based unit tests for the public functions
 * implemented in ui_statistics.c.
 *
 * Directly tested functions:
 * - ui_statistics_print_header()
 * - ui_statistics_print_tick()
 * - ui_statistics_print_final()
 *
 * Indirectly tested behaviour:
 * The module contains several static helper functions for formatting,
 * clamping, occupancy bar rendering, status derivation and statistics
 * calculation. These functions are declared static and are therefore
 * private to ui_statistics.c. Because of this, they cannot be accessed
 * directly from this test translation unit.
 *
 * Instead, their behaviour is verified indirectly through the public
 * printing functions. The tests execute normal and verbose output paths
 * in order to trigger the internal helper functions during regular
 * statistics formatting.
 *
 * All tests capture stdout using temporary files in order to verify
 * deterministic terminal output.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui_statistics.h"
#include "types.h"

/* ------------------------------------------------------------------------- */
/* Test helpers (copied from test_ui.c)                                      */
/* ------------------------------------------------------------------------- */

/* begin_capture_stdout and end_capture_stdout simulate stdout via a temporary file. */
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
/* Tests for ui_statistics.c                                                 */
/* Tested functions:                                                         */
/*   - ui_statistics_print_header()                                          */
/*   - ui_statistics_print_tick()                                            */
/*   - ui_statistics_print_final()                                           */
/* ------------------------------------------------------------------------- */

static void test_ui_statistics_print_header(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[3000];

    /* Test 1: NORMAL header
     *
     * Indirectly covers:
     * - ui_statistics_print_header_normal()
     */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_header(NORMAL);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "PARKHAUS - TICK STATISTICS (NORMAL)") != NULL);
    assert(strstr(buffer, "Legend:") != NULL);

    /* Test 2: VERBOSE header
     *
     * Indirectly covers:
     * - ui_statistics_print_header_verbose()
     */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_header(VERBOSE);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "PARKHAUS - TICK STATISTICS (VERBOSE)") != NULL);
    assert(strstr(buffer, "All available raw metrics per tick are printed.") != NULL);
}

static void test_ui_statistics_print_tick(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[5000];
    StatsTick tick = {0};

    tick.current_tick = 5U;
    tick.capacity_total = 100U;
    tick.capacity_taken = 25U;
    tick.capacity_free = 75U;
    tick.queue_length_end = 3U;
    tick.arrivals_generated = 10U;
    tick.enqueued = 4U;
    tick.entered = 6U;
    tick.departed = 2U;
    tick.queue_rejections = 1U;
    tick.queue_wait_entered_sum_ticks = 12U;
    tick.queue_wait_entered_count = 6U;
    tick.parking_duration_departed_sum_ticks = 30U;
    tick.parking_duration_departed_count = 2U;
    tick.blocker_full_active = 0U;
    tick.bad_parking_cases = 0U;

    /* Test 1: NORMAL tick output
     *
     * Indirectly covers:
     * - ui_statistics_print_tick_normal()
     * - derive_status_text()
     * - calc_util_percent()
     * - calc_avg_queue_wait_entered()
     * - build_occupancy_bar()
     * - clamp_int()
     * - repeat_char()
     * - format_float_1()
     */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_tick(&tick, NORMAL);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "Tick: 5") != NULL);
    assert(strstr(buffer, "Status: OK") != NULL);
    assert(strstr(buffer, "25.0%") != NULL);
    assert(strstr(buffer, "Avg Queue Wait (entered): 2.00 ticks") != NULL);

    /* Test 2: VERBOSE tick output
     *
     * Indirectly covers:
     * - ui_statistics_print_tick_verbose()
     * - derive_status_text()
     * - calc_util_percent()
     * - calc_avg_queue_wait_entered()
     * - format_float_2()
     */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_tick(&tick, VERBOSE);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "Tick = 5 | Status = OK") != NULL);
    assert(strstr(buffer, "Capacity: total=100 | taken=25 | free=75 | util%=25.00") != NULL);
    assert(strstr(buffer, "avgWaitEntered=2.00") != NULL);
}

static void test_ui_statistics_print_final(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[5000];
    StatsSummary summary = {0};

    summary.total_ticks = 100U;
    summary.capacity_taken_percent_avg = 45.678f;
    summary.capacity_taken_percent_peak = 88.888f;
    summary.capacity_taken_peak_tick = 42U;
    summary.first_full_tick = 12;
    summary.full_ticks = 7U;
    summary.arrivals_total = 200U;
    summary.entered_total = 180U;
    summary.departed_total = 170U;
    summary.queue_length_avg = 3.333f;
    summary.queue_length_peak = 9U;
    summary.queue_length_peak_tick = 55U;
    summary.queue_rejections_total = 4U;
    summary.queue_wait_avg_ticks = 2U;
    summary.queue_wait_max_ticks = 8U;
    summary.parking_duration_avg_ticks = 15U;
    summary.blocker_full_ratio_percent = 11.119f;
    summary.bad_parking_cases_total = 2U;
    summary.bad_parking_share_percent = 1.555f;

    /* Test 1: valid summary should be printed
     *
     * Indirectly covers:
     * - format_float_2()
     */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_final(&summary, NORMAL);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "SIMULATION SUMMARY") != NULL);
    assert(strstr(buffer, "Total ticks            : 100") != NULL);
    assert(strstr(buffer, "Avg utilization (%)    : 45.68") != NULL);
    assert(strstr(buffer, "Bad parking share (%)  : 1.56") != NULL);

    /* Test 2: NULL summary should print fallback message
     *
     * Indirectly covers:
     * - no local static helper function directly
     * - verifies NULL handling in ui_statistics_print_final()
     */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_final(NULL, NORMAL);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "No summary statistics available.") != NULL);
    assert(strstr(buffer, "SIMULATION SUMMARY") == NULL);
}

void test_ui_statistics(void)
{
    test_ui_statistics_print_header();
    test_ui_statistics_print_tick();
    test_ui_statistics_print_final();

    printf("All ui_statistics.c tests passed successfully!\n");
}