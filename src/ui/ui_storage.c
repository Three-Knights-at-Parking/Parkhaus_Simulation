#include <stdio.h>
#include <string.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_storage.h"

#include "../include/io/SaveHandler.h"
#include "../include/types.h"

/* ========================================================================= */
/* Local helper functions                                                    */
/* ========================================================================= */

static int load_statistics_file_prompt(void)
{
    char file_name[128];

    clear_terminal();

    printf("====================================\n");
    printf("         LOAD STATISTICS FILE\n");
    printf("====================================\n\n");
    printf("Enter a file name from ../stats/\n");
    printf("Example: stats.csv\n");
    printf("Leave empty for default file.\n\n");
    printf("File name: ");

    if (read_line(file_name, sizeof(file_name)) != OK)
    {
        printf("Input error.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }

    if (file_name[0] == '\0')
    {
        if (savehandler_load_and_print(NULL) != OK)
        {
            printf("Loading default statistics file failed.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return ERROR;
        }
    }
    else
    {
        if (savehandler_load_and_print(file_name) != OK)
        {
            printf("Loading statistics file failed.\n");
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return ERROR;
        }
    }

    printf("\n");
    printf("Press ENTER to continue...\n");
    press_enter_to_continue();

    return OK;
}

/* ========================================================================= */
/* Main storage menu                                                         */
/* ========================================================================= */

void print_storagescreen(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("            STORAGE MENU\n");
    printf("====================================\n");
    printf("\n");
    printf("1 Browse Stats Directory\n");
    printf("0 Back to Home\n");
    printf("\n");
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state storage_menu(void)
{
    int choice = 0;
    validation_flag valid = INVALID;

    print_storagescreen();

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, STORAGE_MAX_VALID_NUMBER);
    }

    if (choice == 1)
    {
        (void)load_statistics_file_prompt();
        return UI_STORAGE;
    }
    else if (choice == 0)
    {
        return UI_HOME;
    }

    return UI_STORAGE;
}



/* This is a FEATURE, not part of the requirements
 * Actually not part of the UI
 * Adding a file browser should perhaps be checked to see if it falls out-of-scope.
void browse_directory(const char *p_current_path) {
}

void directory_options(const char *p_dir_path) {
}

void file_options(const char *p_file_path) {
}

void deleting_verification(const char *p_object_path, const char *p_object_type) {
}

void print_file_to_terminal(const char *p_path) {
}

void delete_directory(const char *p_path) {
}

void delete_file(const char *p_path) {
}*/