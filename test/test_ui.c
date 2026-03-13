#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ui/ui.h"


/* ------------------------------------------------------------------------- */
/* Small test helpers                                                        */
/* ------------------------------------------------------------------------- */

/*Helper function generated with ChatGPT
 *Simulating read stdin, stdout with temporary files
 */
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

static void end_capture_stdout(FILE *p_tmp, int saved_stdout_fd, char *p_buffer, size_t buffer_len)
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

static void test_press_enter_to_continue()
{
    // Test 1: Only ENTER
    FILE *p_in = set_stdin_text("\n");
    press_enter_to_continue();
    fclose(p_in);
    assert(1);

    // Test 2: Characters before ENTER
    p_in = set_stdin_text("01234ABCD*#'+ \n");
    press_enter_to_continue();
    fclose(p_in);
    assert(1);
}

static void test_clear_terminal(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[256];

    p_out = begin_capture_stdout(&saved_fd);
    clear_terminal();
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    // Test 1: Count the number of newline characters
    int count = 0;
    for (size_t i = 0; buffer[i] != '\0'; i++)
        if (buffer[i] == '\n')
            count++;

    assert(count == 40);

    // Test 2: Ensure no other characters are present
    for (size_t i = 0; buffer[i] != '\0'; i++)
        assert(buffer[i] == '\n');

}

static void test_user_input(void)
{
    // Test 1: Valid integer input should be accepted
    FILE *p_in = set_stdin_text("77\n");
    assert(user_input() == 77);
    fclose(p_in);

    // Test 2: Garbage input should be rejected and return -1
    p_in = set_stdin_text("123ABC\n");
    assert(user_input() == ERROR);
    fclose(p_in);

    // Test 3: Input with surrounding whitespace and a negative number should be accepted
    p_in = set_stdin_text("   -7  \n");
    assert(user_input() == -7);
    fclose(p_in);
}

static void test_validate_user_input(void)
{
    // Test 1: A valid number within the allowed range should be accepted
    FILE *p_in = set_stdin_text("\n");   // Prepare stdin for potential press_enter_to_continue()
    assert(validate_user_input(1, 4) == VALID);
    fclose(p_in);

    // Test 2: A negative number should be rejected and return INVALID
    p_in = set_stdin_text("\n");         // Needed because INVALID triggers press_enter_to_continue()
    assert(validate_user_input(-1, 4) == INVALID);
    fclose(p_in);
}

static void test_trim_newline(void)
{
    char text_a[] = "Hello\n";
    char text_b[] = "World";

    // Test 1: A string ending with a newline should have the newline removed and return OK
    assert(trim_newline(text_a) == OK);

    // Test 2: A string without a trailing newline should remain unchanged and return OK
    assert(trim_newline(text_b) == OK);

    // Test 3: Passing NULL should be rejected and return ERROR
    assert(trim_newline(NULL) == ERROR);

    // Test 4: An empty string should be rejected and return ERROR
    char empty[] = "";
    assert(trim_newline(empty) == ERROR);
}
