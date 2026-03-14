/**
 * @file Test_ui_home.c
 * @brief Unit tests for the home menu module.
 *
 * This file contains assert-based unit tests for the functions implemented
 * in ui_home.c. The tests verify correct behaviour of the home menu screen
 * output and the main navigation logic.
 *
 * Tested functionality includes:
 * - home screen printing
 * - menu choice handling
 * - UI state transitions from the home menu
 *
 * All tests simulate stdin and stdout using temporary files in order to
 * provide deterministic input sequences and to capture terminal output.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui.h"
#include "ui/ui_home.h"

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

static void test_print_homescreen(void)
{
    FILE *p_out = NULL;

    int saved_fd = -1;
    char buffer[2000];

    p_out = begin_capture_stdout(&saved_fd);

    print_homescreen();

    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: Title should be printed */
    assert(strstr(buffer, "HOME MENU") != NULL);

    /* Test 2: Menu option should appear */
    assert(strstr(buffer, "1 - Simulation") != NULL);
}

static void test_home_menu(void)
{
    FILE *p_in = NULL;

    /* Test 1: selecting simulation */
    p_in = set_stdin_text("1\n");
    assert(home_menu() == UI_SIMULATION);
    fclose(p_in);

    /* Test 2: selecting exit */
    p_in = set_stdin_text("0\n");
    assert(home_menu() == UI_EXIT);
    fclose(p_in);
}

void test_ui_home(void)
{
    test_print_homescreen();
    test_home_menu();

    printf("All ui_home.c tests passed successfully!\n");
}