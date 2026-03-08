#include "io/ConfigFileHandler.h"
#include "Settings.h"
#include "utils/SafteyUtils.h"
#include <string.h>

#define DEFAULT_CONFIG_PATH "./config.json"

int config_load_settings(Settings *p_settings, const char *src_path) {
    if (p_settings == NULL) {
        print_error_s("config_load_settings: p_settings is NULL", HIGH);
        return ERROR;
    }
    const char *effective_path = (src_path == NULL || src_path[0] == '\0') ? DEFAULT_CONFIG_PATH : src_path;
    return settings_load_from_file(p_settings, effective_path);
}

int config_save_settings(const Settings *p_settings, const char *dest_path) {
    if (p_settings == NULL) {
        print_error_s("config_save_settings: p_settings is NULL", HIGH);
        return ERROR;
    }
    const char *effective_path = (dest_path == NULL || dest_path[0] == '\0') ? DEFAULT_CONFIG_PATH : dest_path;
    return settings_save_to_file(p_settings, effective_path);
}