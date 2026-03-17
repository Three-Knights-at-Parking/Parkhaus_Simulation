#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "io/ConfigFileHandler.h"
#include "Settings.h"
#include "types.h"

static void init_test_settings(Settings *s)
{
    memset(s, 0, sizeof(*s));

    strcpy(s->name, "Config Test");
    s->capacity = 123;
    s->floors = 4;
    s->gates = 2;
    s->real_equivalent = 60;
    s->output_mode = VERBOSE;
    s->max_ticks = 500;
    s->rand_seed = 42;
    s->gate_entry_inSec = 5;
    s->tick_inSec = 60;
    s->max_parking_ticks = 200;
    s->min_parking_ticks = 5;
    s->mode_select = 1;
    s->entry_probability_perSec_prec = 12.5f;
    s->is_leavable = LEAVABLE;
    s->queue_max_length = 15;
    s->src_path = NULL;
    s->stats_path = NULL;
}

static void test_config_save_and_load_roundtrip(void)
{
    Settings original;
    Settings loaded;
    const char *path = "./test_config.json";

    init_test_settings(&original);
    memset(&loaded, 0, sizeof(loaded));

    remove(path);

    assert(config_save_settings(&original, path) == OK);
    assert(config_load_settings(&loaded, path) == OK);

    assert(strcmp(loaded.name, original.name) == 0);
    assert(loaded.capacity == original.capacity);
    assert(loaded.floors == original.floors);
    assert(loaded.gates == original.gates);
    assert(loaded.real_equivalent == original.real_equivalent);
    assert(loaded.output_mode == original.output_mode);
    assert(loaded.max_ticks == original.max_ticks);
    assert(loaded.rand_seed == original.rand_seed);
    assert(loaded.gate_entry_inSec == original.gate_entry_inSec);
    assert(loaded.tick_inSec == original.tick_inSec);
    assert(loaded.max_parking_ticks == original.max_parking_ticks);
    assert(loaded.min_parking_ticks == original.min_parking_ticks);
    assert(loaded.mode_select == original.mode_select);
    assert(loaded.entry_probability_perSec_prec == original.entry_probability_perSec_prec);
    assert(loaded.is_leavable == original.is_leavable);
    assert(loaded.queue_max_length == original.queue_max_length);

    delete_settings(&loaded);
    remove(path);
}

static void test_config_null_settings(void)
{
    Settings s;
    init_test_settings(&s);

    assert(config_save_settings(NULL, "./x.json") == ERROR);
    assert(config_load_settings(NULL, "./x.json") == ERROR);

    delete_settings(&s);
}

static void test_config_default_path_handling(void)
{
    Settings s_save;
    Settings s_load;
    const char *path = "./config.json";

    init_test_settings(&s_save);
    memset(&s_load, 0, sizeof(s_load));

    remove(path);

    assert(config_save_settings(&s_save, "") == OK);
    assert(config_load_settings(&s_load, "") == OK);

    assert(s_load.capacity == s_save.capacity);
    assert(s_load.floors == s_save.floors);
    assert(s_load.gates == s_save.gates);
    assert(s_load.queue_max_length == s_save.queue_max_length);

    delete_settings(&s_save);
    delete_settings(&s_load);

    remove(path);
}

void test_config_file_handler(void)
{
    test_config_save_and_load_roundtrip();
    printf("ConfigFileHandler - roundtrip passed\n");

    test_config_null_settings();
    printf("ConfigFileHandler - null settings passed\n");

    test_config_default_path_handling();
    printf("ConfigFileHandler - default path handling passed\n");

    printf("All ConfigFileHandler tests passed\n");
}