#ifndef UI_H
#define UI_H

/**
 * @file ui.h
 * @brief Public interface of the terminal-based UI (state machine, input helpers).
 *
 * This module provides:
 * - The main UI state machine (ui_start)
 * - Basic user input and validation helpers
 * - Simple terminal helper functions used by multiple UI modules
 */

#include <stdint.h>

#include "Settings.h"

/**
 * @brief UI state identifiers for the main UI state machine.
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
 * @brief Generic validation result used by UI input validation functions.
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
 * Reads and discards one line from stdin. If the user types more than fits into
 * the internal buffer, the remaining characters are discarded as well.
 */
void press_enter_to_continue(void);

/**
 * @brief Clears the terminal output in a portable way (pseudo clear).
 *
 * Prints multiple newlines to push previous output out of view.
 * (This does not truly clear the terminal scrollback buffer.)
 */
void clear_terminal(void);

/* ========================================================================= */
/* Shared input helpers                                                      */
/* ========================================================================= */

/**
 * @brief Reads a raw menu selection from stdin.
 *
 * @return Parsed integer on success, or -1 on invalid input/overflow/underflow.
 */
int user_input(void);

/**
 * @brief Validates a menu choice against a valid range [0..max_valid_number].
 *
 * @param[in] user_choice       The value entered by the user.
 * @param[in] max_valid_number  Maximum allowed menu number (minimum is always 0).
 * @return VALID if user_choice is within range, otherwise INVALID.
 */
validation_flag validate_user_input(const int user_choice, const int max_valid_number);

int trim_newline(char *p_text);
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
 * @return UI_EXIT when the application should terminate.
 */
ui_state ui_start(Settings *p_settings, Simulation *p_simulation);

#endif /* UI_H */