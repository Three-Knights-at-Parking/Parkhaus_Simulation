#ifndef UI_STORAGE_H
#define UI_STORAGE_H

/**
 * @file ui_storage.h
 * @brief Storage menu UI for loading and displaying saved statistics files.
 *
 * This module is responsible for:
 * - printing the storage menu
 * - loading a saved statistics file
 * - displaying loaded statistics via ui_statistics
 * - returning the next UI state
 */

#include "ui.h"

/* ========================================================================= */
/* Storage menu limits                                                       */
/* ========================================================================= */

/**
 * @brief Maximum valid menu number in the storage menu.
 *
 * Valid range:
 * 0 .. STORAGE_MAX_VALID_NUMBER
 */
#define STORAGE_MAX_VALID_NUMBER (1)

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
 * - prints the storage screen
 * - reads and validates the user's menu selection
 * - loads a saved statistics file if requested
 * - prints the loaded statistics
 * - returns the next UI state
 *
 * @param[in] p_settings Pointer to the current settings object.
 * @return Next UI state depending on user selection.
 */
ui_state storage_menu(Settings *p_settings);

/**
 * @brief Browses a directory and allows navigation through its entries.
 *
 * @param[in] p_current_path Path to the directory to browse.
 */
//void browse_directory(const char *p_current_path);

/**
 * @brief Shows options for a selected directory (enter/delete/back).
 *
 * @param[in] p_dir_path Path of the selected directory.
 */
//void directory_options(const char *p_dir_path);

/**
 * @brief Shows options for a selected file (open/delete/back).
 *
 * @param[in] p_file_path Path of the selected file.
 */
//void file_options(const char *p_file_path);

/**
 * @brief Confirmation prompt before deleting a file or directory.
 *
 * @param[in] p_object_path Path of the object to delete.
 * @param[in] p_object_type Object type string (e.g., "File" or "Directory").
 */
//void deleting_verification(const char *p_object_path, const char *p_object_type);

/**
 * @brief Prints a text file to the terminal.
 *
 * @param[in] p_path Path to the file.
 */
//void print_file_to_terminal(const char *p_path);

/**
 * @brief Recursively deletes a directory and its contents.
 *
 * @param[in] p_path Path to the directory.
 */
//void delete_directory(const char *p_path);

/**
 * @brief Deletes a single file.
 *
 * @param[in] p_path Path to the file.
 */
//void delete_file(const char *p_path);

#endif /* STORAGE_H */