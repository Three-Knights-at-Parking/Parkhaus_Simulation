#ifndef STORAGE_H
#define STORAGE_H

/*
 * File: storage.h
 * Description: Declarations for storage navigation and file/directory operations.
 */

#include "ui.h"

/* Maximum valid menu number in main storage menu (0..STORAGE_MAX_VALID_NUMBER). */
#define STORAGE_MAX_VALID_NUMBER 1

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
void browse_directory(char *p_current_path);

/**
 * @brief Shows options for a selected directory (enter/delete/back).
 *
 * @param[in] p_dir_path Path of the selected directory.
 */
void directory_options(char *p_dir_path);

/**
 * @brief Shows options for a selected file (open/delete/back).
 *
 * @param[in] p_file_path Path of the selected file.
 */
void file_options(char *p_file_path);

/**
 * @brief Confirmation prompt before deleting a file or directory.
 *
 * @param[in] p_object_path Path of the object to delete.
 * @param[in] p_object_type Object type string (e.g., "File" or "Directory").
 */
void deleting_verification(char *p_object_path, char *p_object_type);

/**
 * @brief Prints a text file to the terminal.
 *
 * @param[in] p_path Path to the file.
 */
void print_file_to_terminal(char *p_path);

/**
 * @brief Recursively deletes a directory and its contents.
 *
 * @param[in] p_path Path to the directory.
 */
void delete_directory(char *p_path);

/**
 * @brief Deletes a single file.
 *
 * @param[in] p_path Path to the file.
 */
void delete_file(char *p_path);

#endif /* STORAGE_H */