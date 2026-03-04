#ifndef CONFIG_H
#define CONFIG_H

/*
 * File: config.h
 * Description: Settings structure and configuration menu declarations.
 */

#include <stdint.h>
#include "ui.h"

/* Maximum valid menu number in Config menu (0..CONFIG_MAX_VALID_NUMBER). */
#define CONFIG_MAX_VALID_NUMBER 6

/* Allowed ranges for settings (min/max). */
#define MIN_SIZE       1
#define MIN_FLOORS     1
#define MIN_GATES      1
#define MIN_TICK       10
#define MIN_MAX_TICKS  -365
#define MIN_SEED       -1

#define MAX_SIZE       200
#define MAX_FLOORS     10
#define MAX_GATES      6
#define MAX_TICK       8640
#define MAX_MAX_TICKS  100
#define MAX_SEED       2147483647

/**
 * @brief Simulation settings configurable by the user.
 */
typedef struct
{
    uint16_t size;            /* Total parking spots per floor */
    uint8_t  floors;          /* Number of floors */
    uint8_t  gates;           /* Number of gates */
    uint16_t real_equivalent; /* Tick equivalent in real time (seconds), min. 10 */
    int32_t  max_ticks;       /* Max ticks before stop; -1/-2/... for day equivalents */
    int32_t  rand_seed;       /* Random seed; -1 means use current time */
} Settings;

/**
 * @brief Indicates whether settings are initialized with default values.
 */
typedef enum
{
    NOT_INITIALIZED,
    INITIALIZED
} settings_state_flag;

/**
 * @brief Initializes Settings with default values.
 *
 * @return Settings structure with default configuration.
 */
Settings init_settings(void);

/**
 * @brief Prints the configuration screen including current settings.
 *
 * @param[in] settings The current settings to display.
 */
void print_configscreen(Settings settings);

/**
 * @brief Validates a numeric config value based on range and negativity rule.
 *
 * @param[in] new_value       Value entered by the user.
 * @param[in] min             Minimum allowed value.
 * @param[in] max             Maximum allowed value.
 * @param[in] allow_negative  If FALSE, negative values are rejected.
 * @return VALID if value is acceptable, otherwise INVALID.
 */
validation_flag validate_user_input_config(int new_value, int min, int max, int allow_negative);

/**
 * @brief Reads and validates a config value until it is valid.
 *
 * @param[in] min             Minimum allowed value.
 * @param[in] max             Maximum allowed value.
 * @param[in] allow_negative  If FALSE, negative values are rejected.
 * @return A validated integer value within the configured constraints.
 */
int edit_setting(int min, int max, int allow_negative);

/**
 * @brief Handles the configuration menu interaction.
 *
 * @param[in,out] settings The current settings (may be initialized/updated).
 * @return Next UI state depending on user selection.
 */
ui_state config_menu(Settings settings);

#endif /* CONFIG_H */