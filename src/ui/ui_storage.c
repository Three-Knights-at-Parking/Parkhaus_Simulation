/**
 * @file ui_storage.c
 * @brief Storage menu implementation for loading saved statistics files.
 *
 * This module handles loading persisted statistics data and
 * displaying it via the ui_statistics module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ui/ui.h"
#include "../include/ui/ui_statistics.h"
#include "../include/ui/ui_storage.h"

#include "../include/io/SaveHandler.h"
#include "../include/types.h"

/* ========================================================================= */
/* Local helper function prototypes                                          */
/* ========================================================================= */

static int ask_tick_output_mode(int *p_print_all);

static int print_loaded_statistics(enum OutputMode output_mode,
                                   const StatList *p_stat_list);

static void free_loaded_stat_list(StatList *p_stat_list);

static int load_statistics_from_path(const char *p_path);

static int load_default_statistics_file(void);

static int load_custom_statistics_file_prompt();

/* ========================================================================= */
/* Local helper functions                                                    */
/* ========================================================================= */

static int ask_tick_output_mode(int *p_print_all)
{
    char buffer[16];

    if (p_print_all == NULL)
    {
        return ERROR;
    }

    while (1)
    {
        printf("Press ENTER for next tick or enter 0 to print all remaining ticks: ");

        if (read_line(buffer, sizeof(buffer)) != OK)
        {
            printf("Input error.\n");
            continue;
        }

        if (buffer[0] == '\0')
        {
            *p_print_all = 0;
            return OK;
        }

        if (strcmp(buffer, "0") == 0)
        {
            *p_print_all = 1;
            return OK;
        }

        printf("Invalid input. Please press ENTER or type 0.\n");
    }
}

static int print_loaded_statistics(enum OutputMode output_mode,
                                   const StatList *p_stat_list)
{
    const StatsTick *p_current_tick = NULL;
    int print_all_remaining = 0;

    if (p_stat_list == NULL)
    {
        return ERROR;
    }

    ui_statistics_print_header(output_mode);

    p_current_tick = p_stat_list->p_tick_head;

    while (p_current_tick != NULL)
    {
        ui_statistics_print_tick(p_current_tick, output_mode);

        if (print_all_remaining == 0 && p_current_tick->p_next != NULL)
        {
            if (ask_tick_output_mode(&print_all_remaining) != OK)
            {
                return ERROR;
            }
        }

        p_current_tick = p_current_tick->p_next;
    }

    if (p_stat_list->p_summary != NULL)
    {
        ui_statistics_print_final(p_stat_list->p_summary, output_mode);
    }
    else
    {
        printf("Warning: No summary available in loaded statistics.\n");
    }

    return OK;
}

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

static int load_statistics_from_path(const char *p_path)
{
    enum OutputMode output_mode = NORMAL;
    StatList *p_loaded_stats = NULL;

    p_loaded_stats = malloc(sizeof(StatList));
    if (p_loaded_stats == NULL)
    {
        printf("Memory allocation for StatList failed.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }
    p_loaded_stats->p_summary = malloc(sizeof(StatsSummary));
    if (p_loaded_stats->p_summary == NULL)
    {
        printf("Memory allocation for StatsSummary failed.\n");
        free(p_loaded_stats);
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }
    memset(p_loaded_stats->p_summary, 0, sizeof(StatsSummary));

    p_loaded_stats->p_tick_head = NULL;
    p_loaded_stats->p_tick_tail = NULL;
    p_loaded_stats->p_current_tick = NULL;
    if (savehandler_load_and_print(p_path, p_loaded_stats, &output_mode) != OK)
    {
        if (p_path == NULL)
        {
            printf("Loading default statistics file failed.\n");
        }
        else
        {
            printf("Loading statistics file failed.\n");
        }

        free(p_loaded_stats);
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }

    clear_terminal();

    if (print_loaded_statistics(output_mode, p_loaded_stats) != OK)
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

static int load_default_statistics_file(void)
{
    clear_terminal();

    printf("====================================\n");
    printf("     LOAD DEFAULT STATISTICS FILE\n");
    printf("====================================\n\n");

    return load_statistics_from_path(NULL);
}

static int load_custom_statistics_file_prompt(void)
{
    char file_path[256];

    clear_terminal();

    printf("====================================\n");
    printf("      LOAD STATISTICS FROM PATH\n");
    printf("====================================\n\n");
    printf("Enter the name of your custom stats file.\n");
    printf("The file must be located in the './stats/' directory.\n");
    printf("Example: custom_run_1.csv or if the file is in a subfolder test /test/file_name.csv\n\n");
    printf("Filename/Relative Path: ");

    if (read_line(file_path, sizeof(file_path)) != OK)
    {
        printf("Input error.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }

    if (file_path[0] == '\0')
    {
        printf("No path entered.\n");
        printf("Press ENTER to continue...\n");
        press_enter_to_continue();
        return ERROR;
    }

    return load_statistics_from_path(file_path);
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
    printf("1 Load default statistics file\n");
    printf("2 Load statistics file from custom path\n");
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

    if (choice == STORAGE_MENU_LOAD_DEFAULT)
    {
        (void)load_default_statistics_file();
        return UI_STORAGE;
    }
    else if (choice == STORAGE_MENU_LOAD_CUSTOM)
    {
        (void)load_custom_statistics_file_prompt();
        return UI_STORAGE;
    }
    else if (choice == STORAGE_MENU_BACK)
    {
        return UI_HOME;
    }

    return UI_STORAGE;
}