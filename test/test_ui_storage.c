/**
* @file Test_ui_storage.c
 * @brief Unit tests for the storage menu module.
 *
 * This file contains assert-based unit tests for the public functions
 * implemented in ui_storage.c.
 *
 * Directly tested functions:
 * - print_storagescreen()
 * - storage_menu()
 *
 * Indirectly covered static helper functions:
 * - load_default_statistics_file()
 * - load_custom_statistics_file_prompt()
 * - load_statistics_from_path()
 *
 * Static helper functions that are not guaranteed to execute in these tests:
 * - print_loaded_statistics()
 * - ask_tick_output_mode()
 * - free_loaded_stat_list()
 *
 * Reason:
 * These functions are only executed when a statistics file is successfully
 * loaded by savehandler_load_and_print(). This behaviour depends on the
 * external SaveHandler module and on the availability of a valid statistics
 * file in the runtime environment.
 *
 * The unit tests intentionally do not rely on external files or the behaviour
 * of external modules. Therefore the successful loading path cannot be forced
 * deterministically during testing.
 *
 * The tests verify all deterministic behaviour inside ui_storage.c:
 * - correct screen output
 * - correct menu navigation
 * - correct branching into load paths
 * - correct UI state transitions
 *
 * Full execution of the loading success path would require either:
 * - replacing savehandler_load_and_print() with a test stub, or
 * - providing a guaranteed valid statistics file in the test environment.
 *
 * Since neither is part of this test setup, the tests focus on the deterministic
 * control flow of the module.
 *
 * All tests simulate stdin and stdout using temporary files in order to
 * provide deterministic input sequences and to capture terminal output.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ui/ui.h"
#include "ui/ui_storage.h"

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

static void end_capture_stdout(FILE *p_tmp,
                               int saved_stdout_fd,
                               char *p_buffer,
                               size_t buffer_len)
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
/* Tests for ui_storage.c                                                    */
/* Tested functions:                                                         */
/*   - print_storagescreen()                                                 */
/*   - storage_menu()                                                        */
/* ------------------------------------------------------------------------- */

static void test_print_storagescreen(void)
{
    FILE *p_out = NULL;
    int saved_fd = -1;
    char buffer[3000];

    p_out = begin_capture_stdout(&saved_fd);
    print_storagescreen();
    end_capture_stdout(p_out, saved_fd, buffer, sizeof(buffer));

    /* Test 1: title should be printed */
    assert(strstr(buffer, "STORAGE MENU") != NULL);

    /* Test 2: menu option should be printed */
    assert(strstr(buffer, "1 Load default statistics file") != NULL);

    /* Additional check */
    assert(strstr(buffer, "0 Back to Home") != NULL);
}

static void test_storage_menu(void)
{
    FILE *p_in = NULL;

    /* Test 1:
     * user selects "Back to Home"
     *
     * Covers:
     * - basic menu handling
     * - return state UI_HOME
     */
    p_in = set_stdin_text("0\n");

    assert(storage_menu() == UI_HOME);

    fclose(p_in);

    /* Test 2: user selects default statistics loading
     *
     * Input sequence:
     * - 1  -> select "Load default statistics file"
     * - \n -> acknowledge possible message (in this case: Error message)
     *
     * Indirectly covers:
     * - load_default_statistics_file()
     * - load_statistics_from_path()
     *
     * Verifies:
     * - correct execution of the default-load branch
     * - correct return to UI_STORAGE after the operation
     */

    p_in = set_stdin_text("1\n\n");

    assert(storage_menu() == UI_STORAGE);

    fclose(p_in);

    /* Test 3: user selects custom statistics loading
     *
     * Input sequence:
     * - 2        -> select "Load statistics file from custom path"
     * - test.csv -> enter custom file path
     * - \n       -> acknowledge possible message (in this case: Error message)
     *
     * Indirectly covers:
     * - load_custom_statistics_file_prompt()
     * - load_statistics_from_path()
     *
     * Verifies:
     * - correct execution of the custom-load branch
     * - correct return to UI_STORAGE after the operation
     */
    p_in = set_stdin_text("2\ntest.csv\n\n");

    assert(storage_menu() == UI_STORAGE);

    fclose(p_in);
}

void test_ui_storage(void)
{
    test_print_storagescreen();
    test_storage_menu();

    printf("All ui_storage.c tests passed successfully!");
}