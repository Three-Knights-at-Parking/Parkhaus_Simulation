#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SETTINGS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SETTINGS_H

#define SETTINGS_DEFAULT_NAME "Rauenegg"
#define SETTINGS_DEFAULT_PATH "./config.json"
#define SETTINGS_NAME_MAX_LENGTH 19

#define SETTINGS_DEFAULT_CAPACITY 255       // Reasonable starting capacity per floor
#define SETTINGS_MINIMUM_CAPACITY 1
#define SETTINGS_MAXIMUM_CAPACITY 400


#define SETTINGS_DEFAULT_FLOORS 1           // Default to 1 floor
#define SETTINGS_MINIMUM_FLOORS 1           // Default to 1 floor
#define SETTINGS_MAXIMUM_FLOORS 10           // Default to 1 floor

#define SETTINGS_DEFAULT_GATES 1            // Default to 1 gate
#define SETTINGS_MINIMUM_GATES 1            // Default to 1 gate
#define SETTINGS_MAXIMUM_GATES 10           // Default to 1 gate

#define SETTINGS_DEFAULT_REAL_EQUIVALENT 60 // 1 tick = 60 seconds (1 minute)
#define SETTINGS_MINIMUM_REAL_EQUIVALENT 10 // 1 tick = 10 seconds
#define SETTINGS_MAXIMUM_REAL_EQUIVALENT 3600 // 1 tick = 1 hour

#define SETTINGS_DEFAULT_OUTPUT_MODE NORMAL // Default output mode
#define SETTINGS_DEFAULT_MAX_TICKS (-1)     // -1 represents 1 day of simulation
#define SETTINGS_MAXIMUM_DAY_TICKS (-365)     // Max one year of simulation
#define SETTINGS_MAXIMUM_TICKS INT32_MAX     // Max one year of simulation
#define SETTINGS_DEFAULT_MAX_TICKS (-1)     // -1 represents 1 day of simulation

#define SETTINGS_DEFAULT_RAND_SEED (-1)     // -1 tells the RNG to use the current UTC time

#define SETTINGS_DEFAULT_GATE_ENTRY_SEC 5   // 5 seconds to process one car at the gate
#define SETTINGS_MINIMUM_GATE_ENTRY_SEC 3  // 3 seconds to process one car at the gate
#define SETTINGS_MAXIMUM_GATE_ENTRY_SEC 10  // 10 seconds to process one car at the gate

#define SETTINGS_DEFAULT_TICK_SEC 60        // Logical tick step in seconds (usually matches real_equivalent)

#define SETTINGS_DEFAULT_MAXIMUM_PARKING_TICKS 1440 // e.g., 24 hours (1440 minutes) max parking
#define SETTINGS_DEFAULT_MINIMUM_PARKING_TICKS 1    // Minimum parking time

#define SETTINGS_DEFAULT_MODE_SELECT 1          // 1 = normal mode

#define SETTINGS_DEFAULT_ENTRY_PROB 5.0f        // 5.0% probability of a car arriving per second
#define SETTINGS_MINIMUM_ENTRY_PROB 1.0f        // 5.0% probability of a car arriving per second
#define SETTINGS_MAXIMUM_ENTRY_PROB 100.0f        // 5.0% probability of a car arriving per second

#define SETTINGS_DEFAULT_IS_LEAVABLE NON_LEAVABLE // Cars cannot leave the queue
#define SECONDS_IN_DAY 86400
#define SETTINGS_MAX_SIZE_PARAM 255

#include "types.h"
/**
 * Base Settings Object that carries all settings for the Simulation. Can be modified via
 * the CLI or loaded from a file.
 */

    /**
     * Load Settings straight from a config file into an existing Settings object.
     * @param p_settings Pointer to the Settings object to fill.
     * @param src_path The RELATIVE PATH to look for the config file. IF EMPTY or NULL,
     *                 settings_load_from_file will look for config.json in the parent_folder
     *                 of the .exe (equivalent to ../config.json).
     * @return 0 on success, non-zero on error.
     */
    int settings_load_from_file(Settings *p_settings, const char *src_path);

    /**
     * @brief Save these Settings to a file.
     * @param p_settings Pointer to the Settings object to read from.
     * @param dest_path The RELATIVE PATH to the destination to save to, will default to
     *                  ../config.json if empty or NULL.
     * @return 0 on success, non-zero on error.
     */
    int settings_save_to_file(const Settings *p_settings, const char *dest_path);

    /**
     * @brief Initialize a new Settings Object.
     * @param p_settings Pointer to the Settings object to initialize.
     * @param src_path The RELATIVE PATH to the config file. This can't be empty.
     * @param name The name of the parking complex. Will default to Rauenegg and cut off at 20 characters.
     * @param capacity 16-bit Integer representing the parking slots per floor. It's a good idea to make this a power of 2.
     * @param floors 8-bit Integer representing the number of floors. Default is 1 if empty.
     * @param gates 8-bit Integer representing the number of gates. Default is 1 if empty.
     * @param real_equivalent 16-bit Integer representing the realtime equivalent of one tick
     *                        (minimum 10 Seconds per tick). Default is 60.
     * @param output_mode See docs for OutputMode for more info.
     * @param max_ticks Signed 32-bit integer representing the max amount of ticks to simulate.
     *                  -(n) for n-days equivalent (Simulate n days).
     * @param rand_seed The random seed to use for this simulation. -1 if you want to use your current UTC timestamp.
     * @param gate_entry_inSec Time needed for an vehicle to enter the parkhouse
     * @param tick_inSec Time in seconds of one Tick
     * @param max_parking_ticks maximum of Ticks a car is allowed to Park
     * @param min_parking_ticks minimum of Ticks a car will park
     * @param mode_select 0 = none / 1 = normal / 2 = verbose / 3 = Error
     * @param entry_probability_perSec_prec probability of a Car entering per second
     * @param is_leavable Determines if vehicles can leave the queue early at any positions.
     * @return 0 on success, non-zero if parameters are invalid.
     */
    int settings_init(Settings *p_settings,
                      const char *src_path,
                      const char *name,
                      uint16_t capacity,
                      uint8_t floors,
                      uint8_t gates,
                      uint16_t real_equivalent,
                      enum OutputMode output_mode,
                      int32_t max_ticks,
                      int32_t rand_seed,
                      uint16_t gate_entry_inSec,
                      uint16_t tick_inSec,
                      uint32_t max_parking_ticks,
                      uint32_t min_parking_ticks,
                      uint8_t mode_select,
                      float entry_probability_perSec_prec,
                      enum QueueLeavable is_leavable);

    /**
     * @brief Set a path to a config file to read from.
     * @param p_settings Pointer to the Settings object to modify.
     * @param src_path The RELATIVE PATH to the config file. This can't be empty.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_src_path(Settings *p_settings, const char *src_path);

    /**
     * @brief Set the parking slots per floor on this parking complex.
     * @param p_settings Pointer to the Settings object to modify.
     * @param size Positive 16-bit Integer representing the parking slots per floor.
     *             It's a good idea to make this a power of 2.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_size(Settings *p_settings, uint16_t size);

    /**
     * @brief Set the numbers of floors for this parking complex.
     * @param p_settings Pointer to the Settings object to modify.
     * @param floors Positive 8-bit Integer representing the number of floors. Default is 1.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_floors(Settings *p_settings, uint8_t floors);

    /**
     * @brief Set the numbers of gates for this parking complex.
     * @param p_settings Pointer to the Settings object to modify.
     * @param gates Positive 8-bit Integer representing the number of gates. Default is 1 if empty.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_gates(Settings *p_settings, uint8_t gates);

    /**
     * @brief Set how much time will progress with one tick. A minimum of 10 seconds per tick is required. Default is 60.
     * @param p_settings Pointer to the Settings object to modify.
     * @param real_equivalent Positive 16-bit Integer representing the realtime equivalent of one tick in seconds.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_real_equivalent(Settings *p_settings, uint16_t real_equivalent);

    /**
     * @brief Set the OutputMode for this simulation.
     * @param p_settings Pointer to the Settings object to modify.
     * @param output_mode OutputMode to set to. See the docs for OutputMode for definitions.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_output_mode(Settings *p_settings, enum OutputMode output_mode);

    /**
     * @brief Set the maximum ticks that will be simulated. -(n) for n-days equivalent (Simulate n amount days).
     * @param p_settings Pointer to the Settings object to modify.
     * @param max_ticks Signed 32-bit integer representing the max amount of ticks to simulate.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_max_ticks(Settings *p_settings, int32_t max_ticks);

    /**
     * @brief Set the random seed for this simulation. -1 if you want to use your current UTC timestamp.
     * @param p_settings Pointer to the Settings object to modify.
     * @param rand_seed The random seed to use for this simulation.
     * @return 0 on success, non-zero on error.
     */
    int settings_set_rand_seed(Settings *p_settings, int32_t rand_seed);

    /**
     * @brief Convert this to a Parkhaus-Object straight from this settings source.
     *        The created Parkhaus will default to the name "Rauenegg", cut off at max 20 characters otherwise
     *        and it will have empty lists in place.
     * @param p_settings Pointer to the Settings used as source.
     * @param p_parkhaus Pointer to the Parkhaus object to initialize.
     * @return 0 on success, non-zero on error.
     */
    int settings_to_parkhaus(const Settings *p_settings, Parkhaus *p_parkhaus);


    /**
     * Free up this Settings-Object and it's associated memory. Settings object has ownership of all underlying objects.
     * @return 0 on success, non-zero on error.
     */
    int delete_settings(Settings *p_settings);

    /**
     * Check if the string is a valid path that we can write the settings to.
     * @param path String representing the path (relative format)
     * @return 0 if valid, non_zero if invalid
     */
    int settings_is_valid_system_path_string(const char *path);


    #endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SETTINGS_H
