#include "Settings.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "utils/SafteyUtils.h"
#define seconds_in_day 86400
#define SETTINGS_NAME_MAX_CHARS 19
#define SETTINGS_MAX_SIZE_PARAM 256

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
        print_error_s("Out of memory.", HIGH);
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

    size_t len = strlen(src);
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
    if (size < 1 || size > SETTINGS_MAX_SIZE_PARAM) {
        p_settings->capacity = 1;
        print_warning_s("Invalid capacity, setting to default (1).");
        return OK;
    }
    p_settings->capacity = size;
    return OK;
}

int settings_set_floors(Settings *p_settings, const uint8_t floors) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (floors < 1 || floors > SETTINGS_MAX_SIZE_PARAM) {
        p_settings->floors = 1;
        print_warning_s("Invalid number of floors, setting to default (1).");
        return OK;
    }
    p_settings->floors = floors;
    return OK;
}

int settings_set_gates(Settings *p_settings, const uint8_t gates) {
    if (checkNull(p_settings)) {
        print_error_s("Field cannot be null.", HIGH);
        return ERROR;
    }
    if (gates < 1 || gates > SETTINGS_MAX_SIZE_PARAM) {
        p_settings->gates = 1;
        print_warning_s("Invalid number of gates, setting to default (1).");
        return OK;
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
    if (output_mode < NONE || output_mode > DEBUG) {
        print_warning_s("Invalid output mode, setting to default (NORMAL).");
        p_settings->output_mode = NORMAL;
        return OK;
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
        return OK;
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
