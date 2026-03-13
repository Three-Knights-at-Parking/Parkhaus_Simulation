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

static int print_loaded_statistics(const Settings *p_settings,
                                   const StatList *p_stat_list);

static void free_loaded_stat_list(StatList *p_stat_list);

static int load_statistics_file_prompt(const Settings *p_settings);

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

static int print_loaded_statistics(const Settings *p_settings,
                                   const StatList *p_stat_list)
{
    const StatsTick *p_current_tick = NULL;
    int print_all_remaining = 0;

    if (p_settings == NULL || p_stat_list == NULL)
    {
        return ERROR;
    }

    ui_statistics_print_header(p_settings);

    p_current_tick = p_stat_list->p_tick_head;

    while (p_current_tick != NULL)
    {
        ui_statistics_print_tick(p_current_tick, p_settings);

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
        ui_statistics_print_final(p_stat_list->p_summary, p_settings);
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

static int load_default_statistics_file(const Settings *p_settings)
{
    clear_terminal();

    printf("====================================\n");
    printf("     LOAD DEFAULT STATISTICS FILE\n");
    printf("====================================\n\n");

    return load_statistics_from_path(p_settings, NULL);
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

    if (choice == STORAGE_MENU_LOAD_DEFAULT)
    {
        (void)load_default_statistics_file(p_settings);
        return UI_STORAGE;
    }
    else if (choice == STORAGE_MENU_LOAD_CUSTOM)
    {
        (void)load_custom_statistics_file_prompt(p_settings);
        return UI_STORAGE;
    }
    else if (choice == STORAGE_MENU_BACK)
    {
        return UI_HOME;
    }

    return UI_STORAGE;
}