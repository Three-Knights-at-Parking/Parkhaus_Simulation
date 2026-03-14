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
/* Tests                                                                     */
/* ------------------------------------------------------------------------- */

static void test_ui_statistics_print_header(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[3000];

    /* Test 1: NORMAL header */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_header(NORMAL);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "PARKHAUS - TICK STATISTICS (NORMAL)") != NULL);
    assert(strstr(buffer, "Legend:") != NULL);

    /* Test 2: VERBOSE header */
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

    /* Test 1: NORMAL tick output */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_tick(&tick, NORMAL);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "Tick: 5") != NULL);
    assert(strstr(buffer, "Status: OK") != NULL);
    assert(strstr(buffer, "25.0%") != NULL);
    assert(strstr(buffer, "Avg Queue Wait (entered): 2.00 ticks") != NULL);

    /* Test 2: VERBOSE tick output */
    p_out = begin_capture_stdout(&saved_fd);
    ui_statistics_print_tick(&tick, VERBOSE);
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "Tick = 5 | Status = OK") != NULL);
    assert(strstr(buffer, "Capacity: total=100 | taken=25 | free=75 | util%=25.00") != NULL);
    assert(strstr(buffer, "avgWaitEntered=2.00") != NULL);
}