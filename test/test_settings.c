#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Settings.h"
#include "types.h"


#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR -1
#endif
#ifndef UNKNOWN
#define UNKNOWN 1
#endif

static void test_settings_init(void) {
    Settings s;
    memset(&s, 0, sizeof(s));
    int result = settings_init(&s,
                               "./config.json",
                               "Raunegg Test",
                               100,
                               2,
                               3,
                               60,
                               NORMAL,
                               1000,
                               42,
                               5,
                               60,
                               1440,
                               10,
                               1,
                               5.0f,
                               NON_LEAVABLE);

    assert(result == OK);
    assert(strcmp(s.name, "Raunegg Test") == 0);
    assert(strcmp(s.src_path, "./config.json") == 0);
    assert(s.capacity == 100);
    assert(s.floors == 2);
    assert(s.gates == 3);
    assert(s.real_equivalent == 60);
    assert(s.max_ticks == 1000);
    assert(s.rand_seed == 42);
    delete_settings(&s);
}

static void test_settings_setters_valid(void) {
    Settings s;
    memset(&s, 0, sizeof(s));
    assert(settings_set_size(&s, 200) == OK);
    assert(s.capacity == 200);

    assert(settings_set_floors(&s, 5) == OK);
    assert(s.floors == 5);

    assert(settings_set_gates(&s, 4) == OK);
    assert(s.gates == 4);

    assert(settings_set_real_equivalent(&s, 120) == OK);
    assert(s.real_equivalent == 120);

    assert(settings_set_output_mode(&s, VERBOSE) == OK);
    assert(s.output_mode == VERBOSE);

    assert(settings_set_max_ticks(&s, 5000) == OK);
    assert(s.max_ticks == 5000);
    s.real_equivalent = 60;
    assert(settings_set_max_ticks(&s, -2) == OK);
    assert(s.max_ticks == 2880);

    assert(settings_set_rand_seed(&s, 99) == OK);
    assert(s.rand_seed == 99);

    assert(settings_set_src_path(&s, "./new_path.json") == OK);
    assert(strcmp(s.src_path, "./new_path.json") == 0);

    delete_settings(&s);
}

static void test_settings_setters_invalid(void) {
    Settings s;
    memset(&s, 0, sizeof(s));
    assert(settings_set_size(&s, SETTINGS_MAXIMUM_CAPACITY + 1) == UNKNOWN);
    assert(s.capacity == 1);
    assert(settings_set_floors(&s, SETTINGS_MAXIMUM_FLOORS + 1) == UNKNOWN);
    assert(s.floors == 1);
    assert(settings_set_gates(&s, 0) == UNKNOWN);
    assert(s.gates == 1);
    assert(settings_set_real_equivalent(&s, 5) == OK); // The implementation returns OK but should bound to 10
    assert(s.real_equivalent == SETTINGS_MINIMUM_REAL_EQUIVALENT);
    assert(settings_set_output_mode(&s, 99) == UNKNOWN);
    assert(s.output_mode == NORMAL);
    assert(settings_set_size(NULL, 100) == ERROR);
    assert(settings_set_src_path(&s, NULL) == ERROR);
    assert(settings_set_src_path(&s, "") == ERROR);
}

static void test_settings_path_validation(void) {
    assert(settings_is_valid_system_path_string("./config.json") == OK);
    assert(settings_is_valid_system_path_string("../folder/config.json") == OK);
    assert(settings_is_valid_system_path_string("config.json") == OK);

    // Invalid absolute paths (starting with slash)
    assert(settings_is_valid_system_path_string("/etc/config.json") == ERROR);
    assert(settings_is_valid_system_path_string("\\Windows\\config.json") == ERROR);

    // Empty or whitespace paths
    assert(settings_is_valid_system_path_string("") == ERROR);
    assert(settings_is_valid_system_path_string("   ") == ERROR);
    assert(settings_is_valid_system_path_string(NULL) == ERROR);

#ifdef _WIN32
    // Windows specific drive letters
    assert(settings_is_valid_system_path_string("C:/config.json") == ERROR);
    assert(settings_is_valid_system_path_string("./con<fig.json") == ERROR);
#endif
}

static void test_settings_to_parkhaus(void) {
    Settings s;
    Parkhaus p;
    memset(&s, 0, sizeof(s));
    memset(&p, 0, sizeof(p));
    settings_init(&s, "./cfg.json", "Raunegg Test", 50, 3, 2, 60, NORMAL, 100, 1, 5, 60, 100, 1, 1, 5.0f, NON_LEAVABLE);
    assert(settings_to_parkhaus(&s, &p) == OK);
    assert(strcmp(p.name, "Raunegg Test") == 0);
    assert(p.capacity == 150);
    assert(p.floors == 3);
    assert(p.capacity_taken == 0);
    assert(p.base.type == PARKHAUS);

    delete_settings(&s);
}

