#ifndef UI_STORAGE_H
#define UI_STORAGE_H

/**
 * @file ui_storage.h
 * @brief Storage menu UI for loading and displaying saved statistics files.
 *
 * This module is responsible for:
 * - printing the storage menu
 * - loading the default statistics file
 * - loading a statistics file from a custom path
 * - displaying loaded statistics via ui_statistics
 * - returning the next UI state
 */

#include "ui.h"

typedef enum
{
 STORAGE_MENU_BACK = 0,
 STORAGE_MENU_LOAD_DEFAULT = 1,
 STORAGE_MENU_LOAD_CUSTOM = 2
} storage_menu_choice;

/* ========================================================================= */
/* Storage menu limits                                                       */
/* ========================================================================= */

/**
 * @brief Maximum valid menu number in the storage menu.
 *
 * Valid range:
 * 0 .. STORAGE_MAX_VALID_NUMBER
 */
#define STORAGE_MAX_VALID_NUMBER (2)

/* ========================================================================= */
/* Public interface                                                          */
/* ========================================================================= */

/**
 * @brief Prints the storage menu screen.
 *
 * Displays the available storage options.
 */
void print_storagescreen(void);

/**
 * @brief Handles user interaction in the storage menu.
 *
 * This function:
 * - prints the storage menu
 * - reads and validates the user's menu selection
 * - loads the default statistics file or a custom statistics file
 * - displays the loaded statistics
 * - returns the next UI state
 *
 * @param[in] p_settings Pointer to the current settings object.
 * @return Next UI state depending on user selection.
 */
ui_state storage_menu(Settings *p_settings);

#endif /* UI_STORAGE_H */