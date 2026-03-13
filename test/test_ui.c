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
