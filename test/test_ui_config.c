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
/* Tests for ui_config.c                                                     */
/* Tested functions:                                                         */
/*   - output_mode_to_string()                                               */
/*   - print_configscreen()                                                  */
/*   - config_menu()                                                         */
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

static void test_print_configscreen(void) {
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[5000];
    Settings settings = {0};

    strcpy(settings.name, "TestConfig");
    settings.capacity = 100;
    settings.floors = 3;
    settings.gates = 2;
    settings.gate_entry_inSec = 5;
    settings.tick_inSec = 10;
    settings.min_parking_ticks = 2;
    settings.max_parking_ticks = 20;
    settings.entry_probability_perSec_prec = 12.5f;
    settings.max_ticks = 500;
    settings.rand_seed = 42;
    settings.output_mode = NORMAL;

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

static void test_config_menu(void) {
    FILE *p_in = NULL;
    Settings settings = {0};

    strcpy(settings.name, "InitialName");
    settings.capacity = 100;
    settings.floors = 2;
    settings.gates = 1;
    settings.gate_entry_inSec = 5;
    settings.tick_inSec = 10;
    settings.min_parking_ticks = 2;
    settings.max_parking_ticks = 10;
    settings.entry_probability_perSec_prec = 5.0f;
    settings.max_ticks = 100;
    settings.rand_seed = 1;
    settings.output_mode = NORMAL;
}