#include "Settings.h"
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/*
 * For testing on local machine.
 * Code will NOT compile on any windows otherwise.
 */
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define MKDIR(path) _mkdir(path)
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
#define MKDIR(path) mkdir(path, 0777)
#endif

#include "utils/SafteyUtils.h"
#include "json-c/json.h"
#include "json-c/json_object.h"
#include "stdio.h"
#include "sys/stat.h"
static int settings_set_name(Settings *p_settings, const char *name);
static int settings_set_owned_string(char **p_dest, const char *src);
static int settings_create_path_to_file(const char* path);
static char *settings_make_default_config_path();
static int settings_is_relative_path(const char *path);



int settings_load_from_file(Settings *p_settings, const char *src_path) {
    if (checkNull(p_settings) || checkNull(src_path)) {
        print_error_s("Fields cannot be null.", HIGH);
        return ERROR;
    }
    json_object *parsed_json = json_object_from_file(src_path);
    if (checkNull(parsed_json)) {
        print_error_s("Failed to open or parse settings file. File might not exist or contain invalid JSON.", HIGH);
        return ERROR;
    }

    json_object *tmp_obj;
    // json_object_object_get_ex returns true (1) if the key exists, false (0) if it doesn't.
    if (json_object_object_get_ex(parsed_json, "name", &tmp_obj)) {
        settings_set_name(p_settings, json_object_get_string(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "src_path", &tmp_obj)) {
        settings_set_src_path(p_settings, json_object_get_string(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "capacity", &tmp_obj)) {
        settings_set_size(p_settings, (uint16_t)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "floors", &tmp_obj)) {
        settings_set_floors(p_settings, (uint8_t)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "gates", &tmp_obj)) {
        settings_set_gates(p_settings, (uint8_t)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "real_equivalent", &tmp_obj)) {
        settings_set_real_equivalent(p_settings, (uint16_t)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "output_mode", &tmp_obj)) {
        settings_set_output_mode(p_settings, (enum OutputMode)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "max_ticks", &tmp_obj)) {
        settings_set_max_ticks(p_settings, (int32_t)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "rand_seed", &tmp_obj)) {
        settings_set_rand_seed(p_settings, (int32_t)json_object_get_int(tmp_obj));
    }
    if (json_object_object_get_ex(parsed_json, "gate_entry_inSec", &tmp_obj)) {
        p_settings->gate_entry_inSec = (uint16_t)json_object_get_int(tmp_obj);
    }
    if (json_object_object_get_ex(parsed_json, "tick_inSec", &tmp_obj)) {
        p_settings->tick_inSec = (uint16_t)json_object_get_int(tmp_obj);
    }
    if (json_object_object_get_ex(parsed_json, "max_parking_ticks", &tmp_obj)) {
        p_settings->max_parking_ticks = (uint32_t)json_object_get_int(tmp_obj);
    }
    if (json_object_object_get_ex(parsed_json, "min_parking_ticks", &tmp_obj)) {
        p_settings->min_parking_ticks = (uint32_t)json_object_get_int(tmp_obj);
    }
    if (json_object_object_get_ex(parsed_json, "mode_select", &tmp_obj)) {
        p_settings->mode_select = (uint8_t)json_object_get_int(tmp_obj);
    }
    if (json_object_object_get_ex(parsed_json, "entry_probability_perSec_prec", &tmp_obj)) {
        p_settings->entry_probability_perSec_prec = (float)json_object_get_double(tmp_obj);
    }
    if (json_object_object_get_ex(parsed_json, "is_leavable", &tmp_obj)) {
        p_settings->is_leavable = (enum QueueLeavable)json_object_get_int(tmp_obj);
    }

    // json_object_put decrements the reference count and frees memory when it hits 0.
    // See https://json-c.github.io/json-c/json-c-0.18/doc/html/json__object_8h.html#afabf61f932cd64a4122ca8092452eed5
    json_object_put(parsed_json);

    return OK;
}

int settings_save_to_file(const Settings *p_settings, const char *dest_path) {
    if (checkNull(p_settings)) {
        print_error_s("Settings cannot be null.", HIGH);
        return ERROR;
    }

    const char *final_path = dest_path;
    char *allocated_path = NULL;

    if (settings_is_valid_system_path_string(dest_path) != OK) {
        print_warning_s("dest_path is not a valid path, creating and using default path");
        allocated_path = settings_make_default_config_path();
        final_path = allocated_path;
    }else {
        if (settings_create_path_to_file(dest_path) != OK) {
            print_error_s("Something went wrong while creating the path to the settings file. Defaulting.", HIGH);
            allocated_path = settings_make_default_config_path();
            final_path = allocated_path;
        }
    }

    json_object *obj = json_object_new_object();
    if (checkNull(obj)) return ERROR;

    if (!checkNull(p_settings->name)) {
        json_object_object_add(obj, "name", json_object_new_string(p_settings->name[0]));
    } else {
        json_object_object_add(obj, "name", json_object_new_string(""));
    }

    if (p_settings->src_path) {
        json_object_object_add(obj, "src_path", json_object_new_string(p_settings->src_path));
    } else {
        json_object_object_add(obj, "src_path", json_object_new_string(""));
    }

    json_object_object_add(obj, "capacity", json_object_new_int((int32_t) p_settings->capacity));
    json_object_object_add(obj, "floors", json_object_new_int((int32_t) p_settings->floors));
    json_object_object_add(obj, "gates", json_object_new_int((int32_t) p_settings->gates));
    json_object_object_add(obj, "real_equivalent", json_object_new_int((int32_t) p_settings->real_equivalent));
    json_object_object_add(obj, "output_mode", json_object_new_int((int32_t) p_settings->output_mode));
    json_object_object_add(obj, "max_ticks", json_object_new_int((int32_t) p_settings->max_ticks));
    json_object_object_add(obj, "rand_seed", json_object_new_int((int32_t) p_settings->rand_seed));
    json_object_object_add(obj, "gate_entry_inSec", json_object_new_int((int32_t) p_settings->gate_entry_inSec));
    json_object_object_add(obj, "tick_inSec", json_object_new_int((int32_t) p_settings->tick_inSec));
    json_object_object_add(obj, "max_parking_ticks", json_object_new_int64((int64_t) p_settings->max_parking_ticks));
    json_object_object_add(obj, "min_parking_ticks", json_object_new_int64((int64_t) p_settings->min_parking_ticks));
    json_object_object_add(obj, "mode_select", json_object_new_int((int32_t) p_settings->mode_select));

    // JSON-C uses double for floating point numbers
    json_object_object_add(obj, "entry_probability_perSec_prec", json_object_new_double((double)p_settings->entry_probability_perSec_prec));
    json_object_object_add(obj, "is_leavable", json_object_new_int((int32_t) p_settings->is_leavable));
    const int result = json_object_to_file_ext(final_path, obj, JSON_C_TO_STRING_PRETTY);

    json_object_put(obj);
    free(allocated_path);

    if (result < 0) {
        print_error_s("Failed to write settings file. Ensure the directory exists.", HIGH);
        return ERROR;
    }
    return OK;
}

int settings_init(Settings *p_settings,
                  const char *src_path,
                  const char *name,
                  const uint16_t capacity,
                  const uint8_t floors,
                  const uint8_t gates,
                  const uint16_t real_equivalent,
                  const enum OutputMode output_mode,
                  const int32_t max_ticks,
                  const int32_t rand_seed,
                  const uint16_t gate_entry_inSec,
                  const uint16_t tick_inSec,
                  const uint32_t max_parking_ticks,
                  const uint32_t min_parking_ticks,
                  const uint8_t mode_select,
                  const float entry_probability_perSec_prec,
                  const enum QueueLeavable is_leavable) {

    if (checkNull(p_settings) || checkNull(name)  || checkNull(src_path)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    *p_settings->name = "\0";
    p_settings->src_path = NULL;

    if (settings_set_real_equivalent(p_settings, real_equivalent) != OK) return ERROR;
    if (settings_set_gates(p_settings, gates) != OK) return ERROR;
    if (settings_set_size(p_settings, capacity) != OK) return ERROR;
    if (settings_set_floors(p_settings, floors) != OK) return ERROR;
    if (settings_set_max_ticks(p_settings, max_ticks) != OK) return ERROR;
    if (settings_set_rand_seed(p_settings, rand_seed) != OK) return ERROR;
    if (settings_set_output_mode(p_settings, output_mode) != OK) return ERROR;
    if (settings_set_name(p_settings, name) != OK) return ERROR;
    if (settings_set_src_path(p_settings, src_path) != OK) return ERROR;

    p_settings->gate_entry_inSec = gate_entry_inSec;
    p_settings->tick_inSec = tick_inSec;
    p_settings->max_parking_ticks = max_parking_ticks;
    p_settings->min_parking_ticks = min_parking_ticks;
    p_settings->mode_select = mode_select;
    p_settings->entry_probability_perSec_prec = entry_probability_perSec_prec;
    p_settings->is_leavable = is_leavable;

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
    if (real_equivalent < SETTINGS_MINIMUM_REAL_EQUIVALENT) {
        print_error_s("Real equivalent has to be at least 10 seconds, defaulting to 10 seconds.", LOW);
        p_settings->real_equivalent = SETTINGS_MINIMUM_REAL_EQUIVALENT;
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

    if (max_ticks < SETTINGS_MAXIMUM_DAY_TICKS || max_ticks > SETTINGS_MAXIMUM_TICKS) {
        if (p_settings->real_equivalent) {
            p_settings->max_ticks = (int32_t)(SECONDS_IN_DAY / p_settings->real_equivalent);
        } else {
            p_settings->max_ticks = (int32_t)(SECONDS_IN_DAY / 10);
        }
        print_warning_s("Invalid max ticks, setting to default (one day equivalent).");
        return UNKNOWN;
    }

    if (max_ticks < -1) {
        p_settings->max_ticks = (int32_t)(SECONDS_IN_DAY * (-max_ticks) / p_settings->real_equivalent);
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
    if (checkNull(p_settings) || checkNull(p_parkhaus)) {
        print_error_s("Settings or Parkhaus pointer cannot be null.", HIGH);
        return ERROR;
    }
    const char *default_name = SETTINGS_DEFAULT_NAME;
    const char *name_to_use;
    if (checkNull(&p_settings->name) && *p_settings->name[0] != '\0') {
        name_to_use = p_settings->name[0];
    } else {
        name_to_use = default_name;
    }

    strncpy(p_parkhaus->name, name_to_use, sizeof(p_parkhaus->name) - 1);
    p_parkhaus->name[sizeof(p_parkhaus->name) - 1] = '\0';
    p_parkhaus->capacity = p_settings->capacity * p_settings->floors;
    p_parkhaus->floors = p_settings->floors;
    p_parkhaus->capacity_taken = 0;
    p_parkhaus->gate_queues = NULL;
    p_parkhaus->p_parked_head = NULL;
    p_parkhaus->p_parked_tail = NULL;
    p_parkhaus->base.type = PARKHAUS;
    p_parkhaus->base.tick = NULL;
    p_parkhaus->base.id = 0;

    return OK;
}

int delete_settings(Settings *p_settings) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    free(p_settings->name); // FIXME this might not work as expected.
    free(p_settings->src_path);
    *p_settings->name = "\0";
    p_settings->src_path = NULL;
    return OK;
}

int settings_is_valid_system_path_string(const char *path) {
    if (checkNull(path)) {
        print_error_s("Path cannot be null.", HIGH);
        return ERROR;
    }
    const unsigned char *p = (const unsigned char *)path;
    while (*p != '\0' && isspace(*p)) p++;
    if (*p == '\0') {
        print_error_s("Path can't be an empty space.", HIGH);
        return ERROR;
    }
    if (settings_is_relative_path(path) != OK) {
        print_error_s("Path is not a valid relative path.", MEDIUM);
        return ERROR;
    }
#ifdef _WIN32 // Keeping this to stay compatible in case we need to compile on Windows
    for (p = (const unsigned char *)path; *p != '\0'; ++p) {
        if (*p < 32) {
            print_error_s("Your path contains invalid characters for Windows.", MEDIUM);
            return ERROR;
        }
        switch (*p) {
            case '<': case '>': case '"': case '|': case '?': case '*':
                return ERROR;
            default:
                break;
        }
    }

    return OK;
#elif defined(__linux__)
    for (p = (const unsigned char *)path; *p != '\0'; ++p) {
        if (*p < 32) {
            print_error_s("Your path contains invalid characters for Linux.", MEDIUM);
            return ERROR;
        }
    }
    return OK;
#    else
    print_error_s("We couldn't determine your operating system.", HIGH)
    return ERROR;
#    endif
}
/**
 * Checks if a path is strictly relative.
 * Disallows absolute paths on both Linux and Windows.
 * @param path The path string to evaluate.
 * @return OK if relative, ERROR if absolute or invalid.
 */
static int settings_is_relative_path(const char *path) {
    if (checkNull(path)) {
        return ERROR;
    }
    const unsigned char *p = (const unsigned char *)path; // skipping leading whitespaces
    while (*p != '\0' && isspace(*p)) {
        p++;
    }
    if (*p == '\0') {
        return ERROR;
    }
    if (*p == '/' || *p == '\\') { // relative paths need to start with "." or ".."
        print_warning_s("Absolute paths starting with slashes are not allowed.");
        return ERROR;
    }
#ifdef _WIN32
    if (isalpha(p[0]) && p[1] == ':') {
        print_error_s("Absolute paths with drive letters are not allowed.", MEDIUM);
        return ERROR;
    }
#endif
    return OK;
}
/**
 * Create the path to the settings file, if it doesn't exist yet.
 * @note This function assumes you have already checked the path is a valid
 * path with settings_is_valid_system_path_string. It does not check that itself.
 * @param path The path the settings file needs to be created at.
 * @return 0 on success, non-zero on error.
 */
static int settings_create_path_to_file(const char* path) {
    if (checkNull(path)) {
        print_error_s("Path cannot be null.", HIGH);
        return ERROR;
    }
    if (settings_is_relative_path(path) != OK) {
        print_error_s("Path is not a valid relative path.", MEDIUM);
        return ERROR;
    }
    const size_t len = strlen(path);
    char *path_copy = (char *)malloc(len + 1);
    if (checkNull(path_copy)) {
        print_error_s("Out of memory when allocating path.", HIGH);
        return ERROR;
    }
    memcpy(path_copy, path, len + 1);
    char *last_slash = strrchr(path_copy, '/');
    if (last_slash == NULL) {
        free(path_copy);
        return OK;
    }
    *last_slash = '\0';
    for (char *p = path_copy + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char temp = *p;
            *p = '\0';
            if (MKDIR(path_copy) != 0) {
                if (errno != EEXIST) {
                    print_error_s("Failed to create intermediate directory.", HIGH);
                    free(path_copy);
                    return ERROR;
                }
            }
            *p = temp;
        }
    }
    if (MKDIR(path_copy) != 0 && errno != EEXIST) {
        print_error_s("Failed to create final directory.", HIGH);
        free(path_copy);
        return ERROR;
    }

    free(path_copy);
    return OK;
}

/**
 * Create the default settings path.
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
#ifdef _WIN32 // Keeping this to stay compatible in case we need to compile on Windows
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
    const ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1); // -1 is needed b.c. readlink does not add \0!
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

    const size_t dir_len = strlen(exe_path);
    const size_t name_len = strlen(default_name);

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
static int settings_set_name(Settings *p_settings, const char *name) {
    if (checkNull(p_settings) || checkNull(name)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }

    size_t len = strlen(name);
    if (len > SETTINGS_NAME_MAX_LENGTH) {
        len = SETTINGS_NAME_MAX_LENGTH;
    }

    char *buf = (char *)malloc(len + 1);
    if (checkNull(buf)) {
        print_error_s("Failed setting Parkhaus name. Out of Memory.", HIGH);
        return ERROR;
    }

    memcpy(buf, name, len);
    buf[len] = '\0';

    free(p_settings->name); // FIXME this might not work as expected.
    *p_settings->name = buf;
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
