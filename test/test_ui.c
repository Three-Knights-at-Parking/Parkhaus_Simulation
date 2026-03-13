#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui.h"

/* ------------------------------------------------------------------------- */
/* Small test helpers                                                        */
/* ------------------------------------------------------------------------- */

/* Simulates stdin via a temporary file. */
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

    fflush(stdout);
    assert(dup2(saved_stdout_fd, fileno(stdout)) >= 0);
    close(saved_stdout_fd);

    rewind(p_tmp);
    read_len = fread(p_buffer, 1U, buffer_len - 1U, p_tmp);
    p_buffer[read_len] = '\0';
    fclose(p_tmp);
}

/* ------------------------------------------------------------------------- */
/* Tests for ui.c                                                            */
/* Tested functions:                                                         */
/*   - press_enter_to_continue()                                             */
/*   - clear_terminal()                                                      */
/*   - user_input()                                                          */
/*   - validate_user_input()                                                 */
/*   - trim_newline()                                                        */
/*   - read_line()                                                           */
/*   - welcome_message()                                                     */
/*   - ui_start()                                                            */
/* ------------------------------------------------------------------------- */

static void test_press_enter_to_continue(void)
{
    FILE *p_in = NULL;

    /* Test 1: only ENTER */
    p_in = set_stdin_text("\n");
    press_enter_to_continue();
    assert(1);
    fclose(p_in);

    /* Test 2: characters before ENTER */
    p_in = set_stdin_text("01234ABCD*#'+ \n");
    press_enter_to_continue();
    assert(1);
    fclose(p_in);
}

static void test_clear_terminal(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[256];
    size_t count = 0U;

    p_out = begin_capture_stdout(&saved_fd);
    clear_terminal();
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: exactly 40 newline characters */
    for (size_t i = 0U; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '\n')
        {
            count++;
        }
    }
    assert(count == 40U);

    /* Test 2: no other characters are printed */
    for (size_t i = 0U; buffer[i] != '\0'; i++)
    {
        assert(buffer[i] == '\n');
    }
}

static void test_user_input(void)
{
    FILE *p_in = NULL;

    /* Test 1: valid integer */
    p_in = set_stdin_text("77\n");
    assert(user_input() == 77);
    fclose(p_in);

    /* Test 2: trailing garbage is rejected */
    p_in = set_stdin_text("123ABC\n");
    assert(user_input() == ERROR);
    fclose(p_in);

    /* Test 3: whitespace and negative number are accepted */
    p_in = set_stdin_text("   -7  \n");
    assert(user_input() == -7);
    fclose(p_in);
}

static void test_validate_user_input(void)
{
    FILE *p_in = NULL;

    /* Test 1: valid number inside range */
    assert(validate_user_input(1, 4) == VALID);

    /* Test 2: negative number is invalid */
    p_in = set_stdin_text("\n");
    assert(validate_user_input(-1, 4) == INVALID);
    fclose(p_in);

    /* Test 3: number above maximum is invalid */
    p_in = set_stdin_text("\n");
    assert(validate_user_input(5, 4) == INVALID);
    fclose(p_in);
}

static void test_trim_newline(void)
{
    char text_a[] = "Hello\n";
    char text_b[] = "World";
    char empty[] = "";

    /* Test 1: trailing newline is removed */
    assert(trim_newline(text_a) == OK);
    assert(strcmp(text_a, "Hello") == 0);

    /* Test 2: string without newline remains unchanged */
    assert(trim_newline(text_b) == OK);
    assert(strcmp(text_b, "World") == 0);

    /* Test 3: NULL is rejected */
    assert(trim_newline(NULL) == ERROR);

    /* Test 4: empty string is rejected */
    assert(trim_newline(empty) == ERROR);
}

static void test_read_line(void)
{
    char buffer[8];
    FILE *p_in = set_stdin_text("Test\n");

    // Test 1: read_line should successfully read a valid line into the buffer
    assert(read_line(buffer, sizeof(buffer)) == OK);

    // Test 2: The buffer should contain the trimmed input without the newline
    assert(strcmp(buffer, "Test") == 0);

    fclose(p_in);
}

static void test_welcome_message(void)
{
    // Test 1: The function should return UI_HOME after completing the welcome screen
    FILE *p_in = set_stdin_text("\n");  //Simulate ENTER input for press_enter_to_continue()
    assert(welcome_message() == UI_HOME);
    fclose(p_in);

    // Test 2: The function should print the expected welcome text to stdout
    int saved_fd;
    char buffer[2000];

    FILE *p_out = begin_capture_stdout(&saved_fd);

    p_in = set_stdin_text("\n");    // Again simulate ENTER input for press_enter_to_continue()
    welcome_message();
    fclose(p_in);

    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    assert(strstr(buffer, "Welcome!") != NULL);
    assert(strstr(buffer, "Parkhaus-Simulation") != NULL);
}

static void test_ui_start(void)
{
    Settings settings = {0};
    Simulation simulation = {0};

    // Test 1: User immediately exits the UI
    // Simulated input sequence:
    //   "\n" → press_enter_to_continue() in welcome screen
    //   "0\n" → choose "Exit" in the main menu
    FILE *p_in = set_stdin_text("\n0\n");
    assert(ui_start(&settings, &simulation) == UI_EXIT);
    fclose(p_in);

    // Test 2: User navigates through multiple menu options before exiting
    // Simulated input sequence:
    //   "\n" → press_enter_to_continue() in welcome screen
    //   "1\n0\n" → open simulation_menu, then return
    //   "2\n0\n" → open configuration_menu, then return
    //   "3\n0\n" → open storage_menu, then return
    //   "4\n0\n" → open help_menu, then return
    //   "0\n"    → finally choose "Exit"
    p_in = set_stdin_text("\n1\n0\n2\n0\n3\n0\n4\n0\n0\n");
    assert(ui_start(&settings, &simulation) == UI_EXIT);
    fclose(p_in);
}
