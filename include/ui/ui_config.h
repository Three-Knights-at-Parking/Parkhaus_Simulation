#ifndef UI_CONFIG_H
#define UI_CONFIG_H

/**
 * @file ui_config.h
 * @brief Terminal-based configuration menu for simulation settings.
 *
 * This module is responsible for:
 * - displaying the current configuration
 * - allowing the user to modify simulation settings
 * - validating user input in the configuration menu
 *
 * Design note:
 * - This module only modifies the Settings structure.
 * - It does not execute simulation logic.
 * - Derived values (e.g. max entries per tick) are only displayed
 *   and are not stored in the Settings object.
 */

#include <stdint.h>

#include "ui.h"
#include "types.h"

/**
 * @brief Menu entries for the configuration menu.
*/
typedef enum
{
    CONFIG_MENU_BACK = 0,
    CONFIG_MENU_NAME = 1,
    CONFIG_MENU_CAPACITY = 2,
    CONFIG_MENU_FLOORS = 3,
    CONFIG_MENU_GATES = 4,
    CONFIG_MENU_GATE_ENTRY_TIME = 5,
    CONFIG_MENU_TICK_LENGTH = 6,
    CONFIG_MENU_QUEUE_MAX_LENGTH = 7,
    CONFIG_MENU_MIN_PARKING_TICKS = 8,
    CONFIG_MENU_MAX_PARKING_TICKS = 9,
    CONFIG_MENU_ENTRY_PROBABILITY = 10,
    CONFIG_MENU_MAX_TICKS = 11,
    CONFIG_MENU_RANDOM_SEED = 12,
    CONFIG_MENU_OUTPUT_MODE = 13,
    CONFIG_MENU_LOAD_SETTINGS = 14
} config_menu_choice;

/* ========================================================================= */
/* Menu limits                                                               */
/* ========================================================================= */

/**
 * @brief Maximum valid menu entry number in the configuration menu.
 *
 * Valid user input range:
 * 0 .. CONFIG_MAX_VALID_NUMBER
 */
#define CONFIG_MAX_VALID_NUMBER (14)

/* ========================================================================= */
/* Numeric limits                                                            */
/* ========================================================================= */

/**
 * @brief Minimum allowed tick length in seconds.
 */
#define MIN_TICK_SEC (10)

/**
 * @brief Maximum allowed tick length in seconds.
 */
#define MAX_TICK_SEC (86400)

/**
 * @brief Maximum allowed random seed value.
 */
#define MAX_SEED INT32_MAX

/* ========================================================================= */
/* Arrival rate input                                                        */
/* ========================================================================= */

/**
 * @brief Defines how the user specifies vehicle arrival rates.
 *
 * The configuration menu allows entering the arrival rate as:
 * - vehicles per second
 * - vehicles per minute
 * - vehicles per hour
 *
 * The value is converted internally to a probability percentage
 * per second and stored in:
 *
 * Settings::entry_probability_perSec_prec
 */
typedef enum
{
    RATE_PER_SECOND = 1, /**< Vehicles per second */
    RATE_PER_MINUTE = 2, /**< Vehicles per minute */
    RATE_PER_HOUR   = 3  /**< Vehicles per hour   */
} rate_input_mode;

/* ========================================================================= */
/* Public interface                                                          */
/* ========================================================================= */

/**
 * @brief Converts an OutputMode enum value to a readable string.
 *
 * Used for displaying the currently selected output mode
 * in the configuration screen.
 *
 * @param[in] mode Output mode enum value.
 * @return Constant string representation.
 */
const char *output_mode_to_string(enum OutputMode mode);

/**
 * @brief Calculates the maximum number of vehicles that can enter
 * the parking garage during a single simulation tick.
 *
 * The value is derived from the relation between:
 * - tick length (Settings::tick_inSec)
 * - gate entry time (Settings::gate_entry_inSec)
 *
 * Formula:
 * tick_inSec / gate_entry_inSec
 *
 * If the configuration is invalid (not divisible), the function returns 0.
 *
 * @param[in] p_settings Pointer to active settings.
 * @return Maximum possible gate entries per tick.
 */
uint16_t calc_max_possible_entries_per_tick(const Settings *p_settings);

/**
 * @brief Prints the configuration screen with current settings.
 *
 * The screen includes all configurable parameters as well as
 * derived values such as the maximum number of gate entries per tick.
 *
 * @param[in] p_settings Pointer to active settings.
 * @return OK on success, ERROR if p_settings is NULL.
 */
int print_configscreen(const Settings *p_settings);

/**
 * @brief Handles the configuration menu interaction.
 *
 * This function:
 * - prints the configuration screen
 * - reads the user menu selection
 * - updates the corresponding setting
 * - validates dependent settings where required
 *
 * @param[in,out] p_settings Pointer to the Settings object to modify.
 * @return Next UI state depending on user selection.
 */
ui_state config_menu(Settings *p_settings);

#endif /* UI_CONFIG_H */