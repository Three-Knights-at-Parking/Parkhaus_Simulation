#include <stdio.h>
#include <string.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_storage.h"

#include "../include/storage.h"
#include "../include/types.h"

/* ========================================================================= */
/* Main storage menu                                                         */
/* ========================================================================= */

int print_storagescreen(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("            STORAGE MENU\n");
    printf("====================================\n");
    printf("\n");
    printf("1 Browse Stats Directory\n");
    printf("0 Back to Home\n");
    printf("\n");

    return OK;
}

ui_state storage_menu() {
    printf("Storage Menu (Stub)\n");
    return UI_HOME;
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
