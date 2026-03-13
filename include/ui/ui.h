#ifndef UI_H
#define UI_H

/**
 * @file ui.h
 * @brief Public interface of the terminal-based UI.
 *
 * This module provides:
 * - the main UI state machine
 * - shared terminal helper functions
 * - shared input parsing and validation helpers
 */

#include "types.h"

/* ========================================================================= */
/* UI state definitions                                                      */
/* ========================================================================= */

/**
 * @brief UI states used by the main UI state machine.
 */
typedef enum
{
    UI_HOME,
    UI_SIMULATION,
    UI_KONFIG,
    UI_STORAGE,
    UI_HELP,
    UI_EXIT
} ui_state;

/**
 * @brief Generic validation result for UI input checks.
 */
typedef enum
{
    INVALID,
    VALID
} validation_flag;

/* ========================================================================= */
/* Shared UI helper functions                                                */
/* ========================================================================= */

/**
 * @brief Waits for the user to press ENTER.
 *
 * Reads and discards one line from stdin. If the user types more than fits
 * into the internal buffer, the remaining characters are discarded as well.
 */
void press_enter_to_continue(void);

/**
 * @brief Clears the terminal output in a portable way.
 *
 * Prints multiple newlines to push previous output out of view.
 * This does not clear the terminal scrollback buffer.
 */
void clear_terminal(void);

/* ========================================================================= */
/* Shared input helper functions                                             */
/* ========================================================================= */

/**
 * @brief Reads a raw menu selection from stdin.
 *
 * @return Parsed integer on success, or -1 on invalid input,
 *         overflow or underflow.
 */
int user_input(void);

/**
 * @brief Validates a menu choice against a valid range [0..max_valid_number].
 *
 * @param[in] user_choice The value entered by the user.
 * @param[in] max_valid_number Maximum allowed menu number.
 * @return VALID if user_choice is within range, otherwise INVALID.
 */
validation_flag validate_user_input(const int user_choice,
                                    const int max_valid_number);

/**
 * @brief Removes a trailing newline character from a string.
 *
 * @param[in,out] p_text Pointer to the string to modify.
 * @return OK on success, ERROR if p_text is NULL or empty.
 */
int trim_newline(char *p_text);

/**
 * @brief Reads one line from stdin into a buffer.
 *
 * If the entered line is longer than the buffer, the remaining
 * characters are discarded. A trailing newline is removed.
 *
 * @param[out] p_buffer Destination buffer.
 * @param[in] buffer_len Size of the destination buffer.
 * @return OK on success, ERROR on invalid parameters or input failure.
 */
int read_line(char *p_buffer, size_t buffer_len);

/* ========================================================================= */
/* UI state machine                                                          */
/* ========================================================================= */

/**
 * @brief Prints the welcome message and waits for user confirmation.
 *
 * @return UI_HOME as next UI state.
 */
ui_state welcome_message(void);

/**
 * @brief Starts the UI state machine and handles navigation between menus.
 *
 * @param[in] p_settings Pointer to the active settings object.
 * @param[in] p_simulation Pointer to the active simulation object.
 * @return UI_EXIT when the application should terminate.
 */
ui_state ui_start(Settings *p_settings, Simulation *p_simulation);

#endif /* UI_H */