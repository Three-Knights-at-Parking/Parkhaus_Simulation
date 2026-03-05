#include "Settings.h"

#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/*
 * For testing on local machine.
 * Code will NOT compile on any windows otherwise.
 */
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef ERROR
#undef ERROR
#endif
#ifdef OK
#undef OK
#endif

/*
 * Compatibility for Codespaces. DO NOT TOUCH
 */
#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif

#include "utils/SafteyUtils.h"
#define seconds_in_day 86400
#define SETTINGS_NAME_MAX_CHARS 19
#define SETTINGS_MAX_SIZE_PARAM 255

static int settings_set_name(Settings *p_settings, const char *name) {
    if (checkNull(p_settings) || checkNull(name)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }

    size_t len = strlen(name);
    if (len > SETTINGS_NAME_MAX_CHARS) {
        len = SETTINGS_NAME_MAX_CHARS;
    }

    char *buf = (char *)malloc(len + 1);
    if (checkNull(buf)) {
        print_error_s("Failed setting Parkhaus name. Out of Memory.", HIGH);
        return ERROR;
    }

    memcpy(buf, name, len);
    buf[len] = '\0';

    free(p_settings->name);
    p_settings->name = buf;
    return OK;
}

static int settings_set_owned_string(char **p_dest, const char *src) {
    if (checkNull(p_dest) || checkNull(src)) {
        return ERROR;
    }
    if (src[0] == '\0') {
        return ERROR;
    }

    const size_t len = strlen(src);
    char *buf = (char *)malloc(len + 1);
    if (checkNull(buf)) {
        return ERROR;
    }

    memcpy(buf, src, len + 1);

    free(*p_dest);
    *p_dest = buf;
    return OK;
}

int settings_load_from_file(Settings *p_settings, const char *src_path) {
    // TODO
}

int settings_save_to_file(const Settings *p_settings, const char *dest_path) {
    // TODO
}

int settings_init(Settings *p_settings, const char *src_path, const char *name, const uint16_t size, const uint8_t floors,
    const uint8_t gates, const uint16_t real_equivalent, const enum OutputMode output_mode, const int32_t max_ticks, const int32_t rand_seed) {

    if (checkNull(p_settings) || checkNull(name)  || checkNull(src_path)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    p_settings->name = NULL;
    p_settings->src_path = NULL;

    if (settings_set_real_equivalent(p_settings, real_equivalent) != OK) return ERROR;
    if (settings_set_gates(p_settings, gates) != OK) return ERROR;
    if (settings_set_size(p_settings, size) != OK) return ERROR;
    if (settings_set_floors(p_settings, floors) != OK) return ERROR;
    if (settings_set_max_ticks(p_settings, max_ticks) != OK) return ERROR;
    if (settings_set_rand_seed(p_settings, rand_seed) != OK) return ERROR;
    if (settings_set_output_mode(p_settings, output_mode) != OK) return ERROR;
    if (settings_set_name(p_settings, name) != OK) return ERROR;
    if (settings_set_src_path(p_settings, src_path) != OK) return ERROR;

    p_settings->gate_entry_inSec = 1; // FIXME @Maupher
    p_settings->mode_select = NORMAL; // FIXME Currently hardcoded as not relevant for min requirement.
    p_settings->entry_probability_perSec_prec = 1.0f;
    p_settings->is_leavable = NON_LEAVABLE;  // FIXME Currently hardcoded as not relevant for min requirement.
    return OK;
}

int settings_set_src_path(Settings *p_settings, const char *src_path) {
    if (checkNull(p_settings) || checkNull(src_path)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (src_path[0] == '\0') {
        print_error_s("src_path cannot be empty.", HIGH);
        return ERROR;
    }
    if (settings_set_owned_string(&p_settings->src_path, src_path) != OK) {
        print_error_s("Failed to set src_path (out of memory?).", HIGH);
        return ERROR;
    }
    return OK;
}

int settings_set_size(Settings *p_settings, const uint16_t size) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (size < 1 || size > (uint8_t) SETTINGS_MAX_SIZE_PARAM) {
        p_settings->capacity = 1;
        print_warning_s("Invalid capacity, setting to default (1).");
        return UNKNOWN;
    }
    p_settings->capacity = size;
    return OK;
}

int settings_set_floors(Settings *p_settings, const uint8_t floors) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (floors < 1 || floors > (uint8_t)  SETTINGS_MAX_SIZE_PARAM) {
        p_settings->floors = 1;
        print_warning_s("Invalid number of floors, setting to default (1).");
        return UNKNOWN;
    }
    p_settings->floors = floors;
    return OK;
}

int settings_set_gates(Settings *p_settings, const uint8_t gates) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (gates < 1 || gates > (uint8_t) SETTINGS_MAX_SIZE_PARAM) {
        p_settings->gates = 1;
        print_warning_s("Invalid number of gates, setting to default (1).");
        return UNKNOWN;
    }
    p_settings->gates = gates;
    return OK;
}

int settings_set_real_equivalent(Settings *p_settings, const uint16_t real_equivalent) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (real_equivalent < 10) {
        print_error_s("Real equivalent has to be at least 10 seconds, defaulting to 10 seconds.", LOW);
        p_settings->real_equivalent = 10;
        return OK;
    }
    p_settings->real_equivalent = real_equivalent;
    return OK;
}

int settings_set_output_mode(Settings *p_settings, const enum OutputMode output_mode) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (output_mode > DEBUG) {
        print_warning_s("Invalid output mode, setting to default (NORMAL).");
        p_settings->output_mode = NORMAL;
        return UNKNOWN;
    }
    p_settings->output_mode = output_mode;
    return OK;
}

int settings_set_max_ticks(Settings *p_settings, const int32_t max_ticks) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }

    if (max_ticks < -356 || max_ticks > 2147483647) {
        p_settings->max_ticks = (int32_t)(seconds_in_day / (p_settings->real_equivalent ? p_settings->real_equivalent : 10));
        print_warning_s("Invalid max ticks, setting to default (one day equivalent).");
        return UNKNOWN;
    }

    if (max_ticks < -1) {
        p_settings->max_ticks = (int32_t)(seconds_in_day * (-max_ticks) / p_settings->real_equivalent);
    } else {
        p_settings->max_ticks = max_ticks;
    }
    return OK;
}

int settings_set_rand_seed(Settings *p_settings, const int32_t rand_seed) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (rand_seed == -1) {
        p_settings->rand_seed = (int32_t)time(NULL);
        return OK;
    }
    p_settings->rand_seed = rand_seed;
    return OK;
}

int settings_to_parkhaus(const Settings *p_settings, Parkhaus *p_parkhaus) {
    // TODO
}

int delete_settings(Settings *p_settings) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    free(p_settings->name);
    free(p_settings->src_path);
    p_settings->name = NULL;
    p_settings->src_path = NULL;
    return OK;
}

int settings_is_valid_path_string(const char *path) {
    if (checkNull(path)) return 0;

    // reject empty / whitespace-only
    const unsigned char *p = (const unsigned char *)path;
    while (*p != '\0' && isspace(*p)) p++;
    if (*p == '\0') return 0;

    // reject Windows-illegal characters and control chars
    for (p = (const unsigned char *)path; *p != '\0'; ++p) {
        if (*p < 32) return 0; // control characters
        switch (*p) {
            case '<': case '>': case '"': case '|': case '?': case '*':
                return 0;
            default:
                break;
        }
    }

    return 1;
}


/**
 * Create the default settings path (and file).
 * @note This function is heavily dependent on the OS that the code is running on.
 * Therefore, we need to differentiate between Windows and Linux (Codespace OS).
 * @return
 */
static char *settings_make_default_config_path() {
    const char *default_name = "config.json";

/*
 * https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi
 * https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
 */
#ifdef _WIN32 // Development machine compatibility.
    char exe_path[MAX_PATH];
    const DWORD len = GetModuleFileNameA(NULL, exe_path, (DWORD)sizeof(exe_path));
    if (len == 0 || len >= (DWORD)sizeof(exe_path)) { // This should fallback to the relative path or working directory.
        char *fallback = (char *)malloc(strlen(default_name) + 1);
        if (fallback) memcpy(fallback, default_name, strlen(default_name) + 1);
        return fallback;
    }

    for (DWORD i = len; i > 0; --i) {
        if (exe_path[i - 1] == '\\' || exe_path[i - 1] == '/') {
            exe_path[i] = '\0';
            break;
        }
    }

    const size_t dir_len = strlen(exe_path);
    const size_t name_len = strlen(default_name);

    char *full = (char *)malloc(dir_len + name_len + 1);
    if (checkNull(full)) return NULL;

    memcpy(full, exe_path, dir_len);
    memcpy(full + dir_len, default_name, name_len + 1);
    return full;
    /*
     * This function heavily depends on the behavior of sim links on Linux.
     * Reference https://man7.org/linux/man-pages/man2/symlink.2.html for more.
     *
     * Technically, this will break if the exe path has no '/' (is at root level).
     * We don't really care enough to fix it, as executing under root will
     * likely cause other issues anyway.
     */
#elif defined(__linux__)
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1); // -1 is needed b.c. readlink does not add \0!
    if (len <= 0) { // fallback to relative path in case somthing goes wrong. This is the current working directory.
        char *fallback = (char *)malloc(strlen(default_name) + 1);
        if (fallback) memcpy(fallback, default_name, strlen(default_name) + 1);
        return fallback;
    }
    exe_path[len] = '\0';

    // strip file name -> keep directory (ending with '/')
    for (ssize_t i = len; i > 0; --i) {
        if (exe_path[i - 1] == '/') {
            exe_path[i] = '\0';
            break;
        }
    }

    size_t dir_len = strlen(exe_path);
    size_t name_len = strlen(default_name);

    char *full = (char *)malloc(dir_len + name_len + 1);
    if (checkNull(full)) return NULL;

    memcpy(full, exe_path, dir_len);
    memcpy(full + dir_len, default_name, name_len + 1);
    return full;
#else // falling back to a relative path. This will be the working directory on Linux.
    char *fallback = (char *)malloc(strlen(default_name) + 1);
    if (fallback) memcpy(fallback, default_name, strlen(default_name) + 1);
    return fallback;
#endif
}
