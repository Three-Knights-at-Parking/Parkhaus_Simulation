/**
 * @file Test_ui_help.c
 * @brief Unit tests for the help menu module.
 *
 * This file contains assert-based unit tests for the functions implemented
 * in ui_help.c. The tests verify correct behaviour of the help menu screen,
 * the help page output functions and the help menu navigation logic.
 *
 * Tested functionality includes:
 * - help menu screen printing
 * - simulation help page printing
 * - settings help page printing
 * - file handling help page printing
 * - returning from the help menu to the home menu
 *
 * All tests simulate stdin and stdout using temporary files in order to
 * provide deterministic input sequences and to capture terminal output.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui.h"
#include "ui/ui_help.h"

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
/* Tests                                                                     */
/* ------------------------------------------------------------------------- */

static void test_print_helpscreen(void)
{
    FILE *p_out = NULL;
    int saved_stdout_fd = -1;
    char buffer[3000];

    p_out = begin_capture_stdout(&saved_stdout_fd);

    print_helpscreen();

    end_capture_stdout(p_out, saved_stdout_fd, buffer, sizeof(buffer));

    /* Test 1: title should be printed */
    assert(strstr(buffer, "HELP MENU") != NULL);

    /* Test 2: menu option should appear */
    assert(strstr(buffer, "1 - Simulation Model & Overview") != NULL);
}

static void test_print_help_simulation(void)
{
    FILE *p_out = NULL;
    int saved_stdout_fd = -1;
    char buffer[3000];

    p_out = begin_capture_stdout(&saved_stdout_fd);

    print_help_simulation();

    end_capture_stdout(p_out,saved_stdout_fd, buffer, sizeof(buffer));

    /* Test 1: section title should be printed */
    assert(strstr(buffer, "HELP: SIMULATION MODEL") != NULL);

    /* Test 2: return hint should be printed */
    assert(strstr(buffer, "Press ENTER to return to the Help Menu...") != NULL);
}

static void test_print_help_settings(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[4000];

    p_out = begin_capture_stdout(&saved_fd);

    print_help_settings();

    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: section title should be printed */
    assert(strstr(buffer, "HELP: SETTINGS EXPLANATION") != NULL);

    /* Test 2: output mode text should appear */
    assert(strstr(buffer, "Output Mode") != NULL);
}

static void test_print_help_file_handling(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[3000];

    p_out = begin_capture_stdout(&saved_fd);

    print_help_file_handling();

    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: section title should be printed */
    assert(strstr(buffer, "HELP: FILE & PATH HANDLING") != NULL);

    /* Test 2: path restriction text should appear */
    assert(strstr(buffer, "Absolute paths") != NULL);
}

static void test_help_menu(void)
{
    FILE *p_in = NULL;

    /* Test 1: directly go back to home */
    p_in = set_stdin_text("0\n");
    assert(help_menu() == UI_HOME);
    fclose(p_in);

    /* Test 2: open simulation help, return, then go back to home */
    p_in = set_stdin_text("1\n\n0\n");
    assert(help_menu() == UI_HOME);
    fclose(p_in);
}

void test_ui_help(void)
{
    test_print_helpscreen();
    test_print_help_simulation();
    test_print_help_settings();
    test_print_help_file_handling();
    test_help_menu();

    printf("All ui_help.c tests passed successfully!\n");
}