#include "../include/ui/ui_storage.h"
#include <stdio.h>

void print_storagescreen(void) {
    printf("Storage Screen (Stub)\n");
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
