#include <assert.h>
#include <stdio.h>
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
    const char *dummy_path = "./test_dummy_config.json";
    int result = settings_init(&s,
                               dummy_path,
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
                               NON_LEAVABLE,
                               15);

    assert(result == OK);
    assert(strcmp(s.name, "Raunegg Test") == OK);
    assert(strcmp(s.src_path, "./test_dummy_config.json") == OK);
    assert(s.capacity == 100);
    assert(s.floors == 2);
    assert(s.gates == 3);
    assert(s.queue_max_length == 15);
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
    settings_init(&s, "./cfg.json", "Raunegg Test", 50, 3, 2, 60, NORMAL, 100, 1, 5, 60, 100, 1, 1, 5.0f, NON_LEAVABLE, 15);
    assert(settings_to_parkhaus(&s, &p) == OK);
    assert(strcmp(p.name, "Raunegg Test") == 0);
    assert(p.capacity == 150);
    assert(p.floors == 3);
    assert(p.capacity_taken == 0);
    assert(p.base.type == PARKHAUS);

    delete_settings(&s);
}


static void test_settings_save_and_load(void) {
    Settings s_save;
    Settings s_load;
    memset(&s_save, 0, sizeof(s_save));
    memset(&s_load, 0, sizeof(s_load));

    const char* test_file = "./test_settings_io.json";
    settings_init(&s_save, test_file, "Raunegg Test", 123, 4, 2, 30, VERBOSE, 500, 77, 8, 30, 200, 5, 2, 12.5f, LEAVABLE, 15);
    remove(test_file);
    assert(settings_save_to_file(&s_save, test_file) == OK);
    assert(settings_load_from_file(&s_load, test_file) == OK);
    assert(strcmp(s_load.name, s_save.name) == 0);
    assert(s_load.capacity == s_save.capacity);
    assert(s_load.floors == s_save.floors);
    assert(s_load.gates == s_save.gates);
    assert(s_load.real_equivalent == s_save.real_equivalent);
    assert(s_load.output_mode == s_save.output_mode);
    assert(s_load.max_ticks == s_save.max_ticks);
    assert(s_load.rand_seed == s_save.rand_seed);
    assert(s_load.gate_entry_inSec == s_save.gate_entry_inSec);
    assert(s_load.tick_inSec == s_save.tick_inSec);
    assert(s_load.max_parking_ticks == s_save.max_parking_ticks);
    assert(s_load.min_parking_ticks == s_save.min_parking_ticks);
    assert(s_load.mode_select == s_save.mode_select);
    // Float conversion from/to json might introduce inconsistencies but it should be fine. If not.. won't fix :)
    assert(s_load.entry_probability_perSec_prec == s_save.entry_probability_perSec_prec);
    assert(s_load.is_leavable == s_save.is_leavable);
    delete_settings(&s_save);
    delete_settings(&s_load);
    remove(test_file);
}

static void test_delete_settings(void) {
    Settings s;
    memset(&s, 0, sizeof(s));

    settings_set_src_path(&s, "./some_path.json");
    assert(s.src_path != NULL);

    assert(delete_settings(&s) == OK);

    assert(s.src_path == NULL);
    assert(s.name[0] == '\0');
}


void test_settings_all() {

    printf("Running Settings.c tests...\n");
    test_settings_init();
    printf("Settings - test_settings_init passed\n");
    test_settings_setters_valid();
    printf("Settings - setters valid passed\n");
    test_settings_setters_invalid();
    printf("Settings - setters invalid passed\n");
    test_settings_path_validation();
    printf("Settings - path validation passed\n");
    test_settings_to_parkhaus();
    printf("Settings - conversion passed\n");
    test_settings_save_and_load();
    printf("Settings - save and load passed\n");
    test_delete_settings();
    printf("Settings - delete passed\n");
    printf("All Settings.c tests passed successfully!\n");
}