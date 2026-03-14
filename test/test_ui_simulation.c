#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui.h"
#include "ui/ui_config.h"
#include "ui/ui_simulation.h"
#include "ui/ui_statistics.h"
#include "Settings.h"
#include "Simulation.h"

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