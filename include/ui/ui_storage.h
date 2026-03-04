#ifndef STORAGE_H
#define STORAGE_H

/**
 * @file storage.h
 * @brief Storage navigation menu and basic file/directory operations.
 *
 * This module provides a simple terminal-based browser for the runtime directory
 * where simulation outputs are stored.
 *
 * Features:
 * - Browse directories and list entries
 * - Open and print text files to terminal
 * - Delete files and recursively delete directories
 *
 * Safety:
 * - The root runtime directory must not be deleted.
 */

#include "ui.h"

/* Maximum valid menu number in main storage menu (valid range: 0..STORAGE_MAX_VALID_NUMBER). */
#define STORAGE_MAX_VALID_NUMBER (1)

/**
 * @brief Root directory that contains simulation output folders.
 *
 * @note Adjust this path to match your project setup (e.g. "./runtime" or "../runtime").
 *       In the C implementation this may be provided by the data/storage layer instead.
 */
#define RUNTIME_PATH ("./runtime")

/**
 * @brief Prints the storage main screen.
 */
void print_storagescreen(void);

/**
 * @brief Handles the storage menu interaction.
 *
 * @return Next UI state depending on user selection.
 */
ui_state storage_menu(void);

/**
 * @brief Browses a directory and allows navigation through its entries.
 *
 * @param[in] p_current_path Path to the directory to browse.
 */
void browse_directory(const char *p_current_path);

/**
 * @brief Shows options for a selected directory (enter/delete/back).
 *
 * @param[in] p_dir_path Path of the selected directory.
 */
void directory_options(const char *p_dir_path);

/**
 * @brief Shows options for a selected file (open/delete/back).
 *
 * @param[in] p_file_path Path of the selected file.
 */
void file_options(const char *p_file_path);

/**
 * @brief Confirmation prompt before deleting a file or directory.
 *
 * @param[in] p_object_path Path of the object to delete.
 * @param[in] p_object_type Object type string (e.g., "File" or "Directory").
 */
void deleting_verification(const char *p_object_path, const char *p_object_type);

/**
 * @brief Prints a text file to the terminal.
 *
 * @param[in] p_path Path to the file.
 */
void print_file_to_terminal(const char *p_path);

/**
 * @brief Recursively deletes a directory and its contents.
 *
 * @param[in] p_path Path to the directory.
 */
void delete_directory(const char *p_path);

/**
 * @brief Deletes a single file.
 *
 * @param[in] p_path Path to the file.
 */
void delete_file(const char *p_path);

#endif /* STORAGE_H */