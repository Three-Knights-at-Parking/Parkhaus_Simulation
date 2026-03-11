#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io/SaveHandler.h"
#include "types.h"

static void test_resolve_stats_path_defaults(void) {
    const char *path_null = savehandler_resolve_stats_path(NULL);
    const char *path_empty = savehandler_resolve_stats_path("");
    const char *path_parent = savehandler_resolve_stats_path("../evil.csv");
    const char *path_fixed = savehandler_resolve_stats_path("C:/some_path/evil.csv");
    const char *path_custom = savehandler_resolve_stats_path("safe.csv");

    assert(path_null != NULL);
    assert(path_empty != NULL);
    assert(path_parent != NULL);
    assert(path_fixed != NULL);
    assert(path_custom != NULL);

    assert(strcmp(path_null, "./stats/stats.csv") == 0);
    assert(strcmp(path_empty, "./stats/stats.csv") == 0);
    assert(strcmp(path_parent, "./stats/stats.csv") == 0);
    assert(strcmp(path_fixed, "./stats/stats.csv") == 0);
    assert(strcmp(path_custom, "./stats/safe.csv") == 0);
}


static void test_save_tick_normal_writes_header_and_data(void) {
    Settings settings;
    Simulation sim;
    StatsTick tick;
    const char *dest_name = "test_tick_normal.csv";

    memset(&settings, 0, sizeof(settings));
    memset(&sim, 0, sizeof(sim));
    memset(&tick, 0, sizeof(tick));

    strcpy(settings.name, "Raunegg Test");
    settings.capacity = 100;
    settings.floors = 2;
    settings.gates = 1;
    settings.gate_entry_inSec = 5;
    settings.tick_inSec = 60;
    settings.real_equivalent = 60;
    settings.output_mode = NORMAL;
    settings.max_ticks = 100;
    settings.rand_seed = 42;
    settings.entry_probability_perSec_prec = 5.0f;
    settings.is_leavable = NON_LEAVABLE;

    sim.settings = &settings;

    tick.current_tick = 7;
    tick.capacity_total = 200;
    tick.capacity_taken = 80;
    tick.enqueued = 3;
    tick.entered = 2;
    tick.departed = 1;
    tick.queue_length_end = 4;

    const char *full_path = savehandler_resolve_stats_path(dest_name);
    remove(full_path);

    assert(savehandler_save_tick(&sim, &tick, dest_name) == OK);

    FILE *f = fopen(full_path, "r");
    assert(f != NULL);

    char buffer[4096];
    size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[bytes] = '\0';
    fclose(f);

    assert(strstr(buffer, "# Simulation Settings") != NULL);
    assert(strstr(buffer, "Tick,CapTotal,CapTaken,Enqueued,Entered,Departed,QueueLen") != NULL);
    assert(strstr(buffer, "7,200,80,3,2,1,4") != NULL);

    remove(full_path);
}
