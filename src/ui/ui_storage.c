#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_storage.h"

#include "../include/io/SaveHandler.h"
#include "../include/types.h"

/* ========================================================================= */
/* Local helper functions                                                    */
/* ========================================================================= */

static void free_loaded_stat_list(StatList *p_stat_list)
{
    StatsTick *p_current_tick = NULL;
    StatsTick *p_next_tick = NULL;

    if (p_stat_list == NULL)
    {
        return;
    }

    p_current_tick = p_stat_list->p_tick_head;

    while (p_current_tick != NULL)
    {
        p_next_tick = p_current_tick->p_next;
        free(p_current_tick);
        p_current_tick = p_next_tick;
    }

    if (p_stat_list->p_summary != NULL)
    {
        free(p_stat_list->p_summary);
        p_stat_list->p_summary = NULL;
    }

    p_stat_list->p_tick_head = NULL;
    p_stat_list->p_tick_tail = NULL;
    p_stat_list->p_current_tick = NULL;
}

static int load_statistics_file_prompt(const Settings *p_settings)
{
    char file_name[128];
    StatList *p_loaded_stats = NULL;

    if (p_settings == NULL)
    {
        return ERROR;
    }

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

    p_loaded_stats = malloc(sizeof(StatList));
    if (p_loaded_stats == NULL)
    {
        printf("Memory allocation for StatList failed.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }

    p_loaded_stats->p_tick_head = NULL;
    p_loaded_stats->p_tick_tail = NULL;
    p_loaded_stats->p_current_tick = NULL;
    p_loaded_stats->p_summary = NULL;

    if (file_name[0] == '\0')
    {
        if (savehandler_load_and_print(NULL, p_loaded_stats) != OK)
        {
            printf("Loading default statistics file failed.\n");
            free(p_loaded_stats);
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return ERROR;
        }
    }
    else
    {
        if (savehandler_load_and_print(file_name, p_loaded_stats) != OK)
        {
            printf("Loading statistics file failed.\n");
            free(p_loaded_stats);
            printf("Press ENTER to continue...\n");
            press_enter_to_continue();
            return ERROR;
        }
    }

    clear_terminal();

    if (print_loaded_statistics(p_settings, p_loaded_stats) != OK)
    {
        printf("Printing loaded statistics failed.\n");
        free_loaded_stat_list(p_loaded_stats);
        free(p_loaded_stats);
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }

    free_loaded_stat_list(p_loaded_stats);
    free(p_loaded_stats);

    printf("\n");
    printf("Finished displaying loaded statistics.\n");
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
    printf("1 Load statistics files\n");
    printf("0 Back to Home\n");
    printf("\n");
}

/* ========================================================================= */
/* Menu logic                                                                */
/* ========================================================================= */

ui_state storage_menu(Settings *p_settings)
{
    int choice = 0;
    validation_flag valid = INVALID;

    if (p_settings == NULL)
    {
        printf("Internal error: Settings not available.\n");
        printf("Press ENTER to return...\n");
        press_enter_to_continue();
        return UI_HOME;
    }

    print_storagescreen();

    while (valid != VALID)
    {
        choice = user_input();
        valid = validate_user_input(choice, STORAGE_MAX_VALID_NUMBER);
    }

    if (choice == 1)
    {
        (void)load_statistics_file_prompt(p_settings);
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