#ifndef UI_H
#define UI_H

/*
 * File: ui.h
 * Description: Declarations for general UI functions and shared UI types.
 */

#include <stdint.h>

/**
 * @brief UI state identifiers for the main UI state machine.
 */
typedef enum
{
    UI_HOME,
    UI_SIMULATION,
    UI_KONFIG,
    UI_STORAGE,
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

/**
 * @brief Reads an integer choice from the user via terminal.
 *
 * @return The integer entered by the user.
 */
int user_input(void);

/**
 * @brief Validates a menu choice against a valid range [0..max_valid_number].
 *
 * @param[in] user_choice        The value entered by the user.
 * @param[in] max_valid_number   Maximum allowed menu number (minimum is always 0).
 * @return VALID if user_choice is within range, otherwise INVALID.
 */
validation_flag validate_user_input(int user_choice, const int max_valid_number);

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
ui_state ui_start(void);

#endif /* UI_H */