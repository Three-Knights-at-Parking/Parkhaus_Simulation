#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SETTINGS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SETTINGS_H

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
     * @param name The name of this parking complex. Will default to Rauenegg and cut off at 20 characters.
     * @param size 16-bit Integer representing the parking slots per floor. It's a good idea to make this a power of 2.
     * @param floors 8-bit Integer representing the number of floors. Default is 1 if empty.
     * @param gates 8-bit Integer representing the number of gates. Default is 1 if empty.
     * @param real_equivalent 16-bit Integer representing the realtime equivalent of one tick
     *                        (minimum 10 Seconds per tick). Default is 60.
     * @param output_mode See docs for OutputMode for more info.
     * @param max_ticks Signed 32-bit integer representing the max amount of ticks to simulate.
     *                  -(n) for n-days equivalent (Simulate n days).
     * @param rand_seed The random seed to use for this simulation. -1 if you want to use your current UTC timestamp.
     * @return 0 on success, non-zero if parameters are invalid.
     */
    int settings_init(Settings *p_settings,
                      const char *src_path,
                      const char *name,
                      uint16_t size,
                      uint8_t floors,
                      uint8_t gates,
                      uint16_t real_equivalent,
                      enum OutputMode output_mode,
                      int32_t max_ticks,
                      int32_t rand_seed);

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

    #endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SETTINGS_H
