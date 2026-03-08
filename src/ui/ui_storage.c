#include <stdio.h>
#include <string.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_storage.h"

#include "../include/storage.h"
#include "../include/types.h"

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
        browse_directory(NULL);
        return UI_STORAGE;
    }
    else if (choice == 0)
    {
        return UI_HOME;
    }

    return UI_STORAGE;
}

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
}
