#ifndef UI_CONFIG_H
#define UI_CONFIG_H

/**
 * @file ui_config.h
 * @brief Configuration menu UI for editing Settings via terminal input.
 *
 * This module is responsible for:
 * - printing the current configuration
 * - editing individual settings values
 * - validating terminal input
 * - returning the next UI state
 */

#include <stdint.h>

#include "ui.h"
#include "types.h"

/* ========================================================================= */
/* Config menu limits                                                        */
/* ========================================================================= */

/**
 * @brief Maximum valid menu number in the config menu.
 *
 * Valid range:
 * 0 .. CONFIG_MAX_VALID_NUMBER
 */
#define CONFIG_MAX_VALID_NUMBER (10)

/* ========================================================================= */
/* Numeric ranges used by ui_config.c                                        */
/* ========================================================================= */

/* Probability in percent */
#define MIN_PROB_PERCENT (0.0f)
#define MAX_PROB_PERCENT (100.0f)

/* Tick length in seconds
   NOTE: currently no dedicated Settings setter exists for tick_inSec. */
#define MIN_TICK_SEC (1)
#define MAX_TICK_SEC (86400)

/* Random seed */
#define MAX_SEED (2147483647)

/* ========================================================================= */
/* Public interface                                                          */
/* ========================================================================= */
/**
 * @brief Converts OutputMode enum to a readable string.
 *
 * @param[in] mode Output mode enum value.
 * @return Constant string representation.
 */
const char *output_mode_to_string(enum OutputMode mode);

/**
 * @brief Prints the configuration screen including current settings.
 *
 * @param[in] p_settings Pointer to the current settings object.
 * @return OK on success, ERROR if p_settings is invalid.
 */
int print_configscreen(const Settings *p_settings);

/**
 * @brief Handles user interaction in the configuration menu.
 *
 * This function:
 * - prints the config screen
 * - reads and validates the menu selection
 * - edits the selected setting
 * - returns the next UI state
 *
 * @param[in,out] p_settings Pointer to the Settings object to edit.
 * @return Next UI state depending on user selection.
 */
ui_state config_menu(Settings *p_settings);

uint16_t calc_max_possible_entries_per_tick(const Settings *p_settings);

enum time_mode {SECOND, MINUTE, HOUR};

#endif /* UI_CONFIG_H */