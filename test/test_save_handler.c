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
    const size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[bytes] = '\0';
    fclose(f);

    assert(strstr(buffer, "# Simulation Settings") != NULL);
    assert(strstr(buffer, "Tick,CapTotal,CapTaken,Enqueued,Entered,Departed,QueueLen") != NULL);
    assert(strstr(buffer, "7,200,80,3,2,1,4") != NULL);

    remove(full_path);
}



static void test_save_summary_appends_summary_lines(void) {
    Settings settings;
    Simulation sim;
    StatsSummary summary;
    const char *dest_name = "test_summary.csv";

    memset(&settings, 0, sizeof(settings));
    memset(&sim, 0, sizeof(sim));
    memset(&summary, 0, sizeof(summary));

    strcpy(settings.name, "Raunegg Test");
    settings.capacity = 100;
    settings.floors = 2;
    settings.gates = 2;
    settings.gate_entry_inSec = 5;
    settings.tick_inSec = 60;
    settings.real_equivalent = 60;
    settings.output_mode = VERBOSE;
    settings.max_ticks = 100;
    settings.rand_seed = 11;
    settings.entry_probability_perSec_prec = 6.5f;
    settings.is_leavable = LEAVABLE;

    sim.settings = &settings;

    summary.total_ticks = 50;
    summary.arrivals_total = 120;
    summary.entered_total = 110;
    summary.departed_total = 100;
    summary.capacity_taken_percent_avg = 55.5f;
    summary.capacity_taken_percent_peak = 97.0f;
    summary.queue_length_avg = 2.5f;
    summary.queue_wait_avg_ticks = 4;
    summary.queue_wait_max_ticks = 12;
    summary.bad_parking_share_percent = 3.0f;

    const char *full_path = savehandler_resolve_stats_path(dest_name);
    remove(full_path);

    assert(savehandler_save_summary(&sim, &summary, dest_name) == OK);

    FILE *f = fopen(full_path, "r");
    assert(f != NULL);

    char buffer[4096];
    const size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[bytes] = '\0';
    fclose(f);

    assert(strstr(buffer, "--- SIMULATION SUMMARY ---") != NULL);
    assert(strstr(buffer, "Total Ticks,50") != NULL);
    assert(strstr(buffer, "Total Arrivals,120") != NULL);
    assert(strstr(buffer, "Total Entered,110") != NULL);
    assert(strstr(buffer, "Total Departed,100") != NULL);
    assert(strstr(buffer, "Avg Wait Time (Ticks),4") != NULL);
    assert(strstr(buffer, "Max Wait Time (Ticks),12") != NULL);

    remove(full_path);
}

