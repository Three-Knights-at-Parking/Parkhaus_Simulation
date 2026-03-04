#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_CONFIGFILEHANDLER_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_CONFIGFILEHANDLER_H

#include "../types.h"

    /**
     * @brief Load settings from a configuration file into an existing Settings object.
     * @param p_settings Pointer to the Settings object to fill.
     * @param src_path Relative path to the config file; if NULL or empty, config will look
     * for a config.json in the root/parent directory of the EXE.
     * @return 0 on success, non-zero on error.
     */
    int config_load_settings(Settings *p_settings, const char *src_path);

    /**
     * @brief Save settings to a configuration file.
     * @param p_settings Pointer to the Settings object to read from.
     * @param dest_path Relative path to the output file; if NULL or empty,
     * file will be saved to the root/parent directory of the EXE.
     *
     * @note if dest_path is null or misconfigured, this will silently default
     * to the root folder off the EXE equivalent to ../config.json! dest_path needs
     * to be a json in future implementations!
     *
     * @return 0 on success, non-zero on error.
     */
    int config_save_settings(const Settings *p_settings, const char *dest_path);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_CONFIGFILEHANDLER_H
