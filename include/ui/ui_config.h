#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Configuration menu UI and input helpers for Settings.
 *
 * This module is responsible for:
 * - Initializing default Settings values (UI-side defaults)
 * - Editing Settings values via terminal input
 * - Printing the configuration menu screen
 *
 * Note:
 * - The settings object is owned by the UI state machine (ui_start) and is
 *   passed into this module by pointer.
 */

#include <stdint.h>
#include "ui.h"
#include "types.h"

/* Maximum valid menu number in Config menu (valid range: 0..CONFIG_MAX_VALID_NUMBER). */
#define CONFIG_MAX_VALID_NUMBER (10)

/* Name input constraints */
#define NAME_MAX_LEN (64)

/* Probability is configured in percent. */
#define MIN_PROB_PERCENT (0.0f)
#define MAX_PROB_PERCENT (100.0f)

/* Allowed ranges for settings (min/max). */
#define MIN_CAPACITY          (1)
#define MIN_FLOORS            (1)
#define MIN_GATES             (1)
#define MIN_GATE_ENTRY_SEC    (1)
#define MIN_TICK_SEC          (1)
#define MIN_REAL_EQUIV_SEC    (10)
#define MIN_MAX_TICKS         (-365)
#define MIN_SEED              (-1)

#define MAX_CAPACITY          (200)
#define MAX_FLOORS            (10)
#define MAX_GATES             (6)
#define MAX_GATE_ENTRY_SEC    (120)
#define MAX_TICK_SEC          (86400)
#define MAX_REAL_EQUIV_SEC    (86400)
#define MAX_MAX_TICKS         (100)
#define MAX_SEED              (2147483647)

/**
 * @brief Initialization state of the Settings object (UI-side).
 */
typedef enum
{
    SETTINGS_NOT_INITIALIZED = 0,
    SETTINGS_INITIALIZED
} settings_init_state_t;

/**
 * @brief Global initialization state flag used by UI menus.
 *
 * The state is set to SETTINGS_NOT_INITIALIZED at program start (welcome_message)
 * and changed to SETTINGS_INITIALIZED after default initialization.
 */
extern settings_init_state_t g_settings_state;

/**
 * @brief Initializes the Settings struct with UI default values.
 *
 * @param[out] p_settings Settings object to initialize.
 *
 * @note This function only assigns UI default values.
 *       If the data/simulation layer has its own initialization routine,
 *       it should be called there (or integrated later).
 */
void init_settings(Settings *p_settings);

/**
 * @brief Prints the configuration screen including current settings.
 *
 * @param[in] p_settings The current settings to display.
 */
void print_configscreen(const Settings *p_settings);

/**
 * @brief Validates an integer config value based on range and negativity rule.
 *
 * @param[in] value           Value entered by the user.
 * @param[in] min             Minimum allowed value.
 * @param[in] max             Maximum allowed value.
 * @param[in] allow_negative  If 0, negative values are rejected.
 * @return VALID if value is acceptable, otherwise INVALID.
 */
validation_flag validate_int_input(int value, int min, int max, int allow_negative);

/**
 * @brief Validates a float percentage input in the range [0.0 .. 100.0].
 *
 * @param[in] value Value entered by the user.
 * @return VALID if value is within [0.0 .. 100.0], otherwise INVALID.
 */
validation_flag validate_float_input_percent(float value);

/**
 * @brief Validates a string input (non-empty and length <= max_len).
 *
 * @param[in] text     User input text.
 * @param[in] max_len  Maximum allowed length.
 * @return VALID if acceptable, otherwise INVALID.
 */
validation_flag validate_string_input(const char *text, int max_len);

/**
 * @brief Reads and validates an integer value until it is valid.
 *
 * @param[in] min             Minimum allowed value.
 * @param[in] max             Maximum allowed value.
 * @param[in] allow_negative  If 0, negative values are rejected.
 * @return A validated integer value within the configured constraints.
 */
int edit_int_setting(int min, int max, int allow_negative);

/**
 * @brief Reads and validates a float percentage value until it is valid.
 *
 * @return A validated percentage value within [0.0 .. 100.0].
 */
float edit_float_setting_percent(void);

/**
 * @brief Reads and validates a string value until it is valid.
 *
 * @param[in] max_len Maximum allowed length.
 * @return A validated string (pseudocode: returned string ownership depends on your C impl).
 */
char *edit_string_setting(int max_len);

/**
 * @brief Shows output mode selection screen and returns selection in [0..3].
 *
 * @return Mode selection number (0..3).
 */
int edit_mode_select(void);

/**
 * @brief Maps numeric mode selection to OutputMode enum.
 *
 * @param[in] mode_select Number in range [0..3].
 * @return Corresponding OutputMode value.
 */
enum OutputMode apply_mode_select(int mode_select);

/**
 * @brief Converts OutputMode enum to a readable string.
 *
 * @param[in] mode Output mode enum value.
 * @return Constant string representation.
 */
const char *output_mode_to_string(enum OutputMode mode);

/**
 * @brief Handles the configuration menu interaction.
 *
 * Initializes settings with defaults on first entry and allows the user
 * to change individual fields.
 *
 * @param[in,out] p_settings The current settings (may be initialized/updated).
 * @return Next UI state depending on user selection.
 */
ui_state config_menu(Settings *p_settings);

#endif /* CONFIG_H */