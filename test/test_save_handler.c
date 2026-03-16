#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io/SaveHandler.h"
#include "types.h"


static void cleanup_loaded_ticks(StatList *list) {
    if (list == NULL) {
        return;
    }

    StatsTick *current = list->p_tick_head;
    while (current != NULL) {
        StatsTick *next = current->p_next;
        free(current);
        current = next;
    }

    list->p_tick_head = NULL;
    list->p_tick_tail = NULL;
    list->p_current_tick = NULL;
}

static void test_resolve_stats_path_defaults(void) {
    assert(strcmp(savehandler_resolve_stats_path(NULL), "./stats/stats.csv") == 0);
    assert(strcmp(savehandler_resolve_stats_path(""), "./stats/stats.csv") == 0);
    assert(strcmp(savehandler_resolve_stats_path("../evil.csv"), "./stats/stats.csv") == 0);
    assert(strcmp(savehandler_resolve_stats_path("C:/some_path/evil.csv"), "./stats/stats.csv") == 0);
    assert(strcmp(savehandler_resolve_stats_path("safe.csv"), "./stats/safe.csv") == 0);
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

    assert(savehandler_init_stats_file(&sim, dest_name) == OK);
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
    summary.capacity_total = 200;
    summary.arrivals_total = 120;
    summary.enqueued_total = 115;
    summary.entered_total = 110;
    summary.departed_total = 100;
    summary.capacity_taken_percent_avg = 55.5f;
    summary.capacity_taken_percent_peak = 97.0f;
    summary.capacity_taken_peak_tick = 33;
    summary.first_full_tick = 41;
    summary.full_ticks = 3;
    summary.queue_length_avg = 2.5f;
    summary.queue_length_peak = 8;
    summary.queue_length_peak_tick = 24;
    summary.queue_rejections_total = 6;
    summary.queue_wait_avg_ticks = 4;
    summary.queue_wait_max_ticks = 12;
    summary.queue_active_ratio_percent = 44.0f;
    summary.parking_duration_avg_ticks = 18;
    summary.blocker_full_ratio_percent = 10.0f;
    summary.bad_parking_cases_total = 2;
    summary.bad_parking_share_percent = 3.0f;

    const char *full_path = savehandler_resolve_stats_path(dest_name);
    remove(full_path);

    assert(savehandler_init_stats_file(&sim, dest_name) == OK);
    assert(savehandler_save_summary(&sim, &summary, dest_name) == OK);

    FILE *f = fopen(full_path, "r");
    assert(f != NULL);

    char buffer[4096];
    const size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[bytes] = '\0';
    fclose(f);

    assert(strstr(buffer, "# Simulation Settings") != NULL);
    assert(strstr(buffer, "--- SIMULATION SUMMARY ---") != NULL);
    assert(strstr(buffer, "Total Ticks,50") != NULL);
    assert(strstr(buffer, "Capacity Total,200") != NULL);
    assert(strstr(buffer, "Peak Utilization Tick,33") != NULL);
    assert(strstr(buffer, "First FULL Tick,41") != NULL);
    assert(strstr(buffer, "FULL Ticks,3") != NULL);
    assert(strstr(buffer, "Queue Peak,8") != NULL);
    assert(strstr(buffer, "Queue Peak Tick,24") != NULL);
    assert(strstr(buffer, "Queue Rejections Total,6") != NULL);
    assert(strstr(buffer, "Avg Parking Duration (Ticks),18") != NULL);
    assert(strstr(buffer, "Blocker FULL Ratio (%),10.00") != NULL);
    assert(strstr(buffer, "Bad Parking Total,2") != NULL);

    remove(full_path);
}
static void test_load_and_print_reads_tick_and_summary(void) {
    Settings settings;
    Simulation sim;
    StatsTick tick;
    StatsSummary summary_to_save;
    StatsSummary loaded_summary;
    StatList list;
    const char *dest_name = "test_load.csv";
    const char *full_path;
    enum OutputMode mode;

    memset(&settings, 0, sizeof(settings));
    memset(&sim, 0, sizeof(sim));
    memset(&tick, 0, sizeof(tick));
    memset(&summary_to_save, 0, sizeof(summary_to_save));
    memset(&loaded_summary, 0, sizeof(loaded_summary));
    memset(&list, 0, sizeof(list));

    strcpy(settings.name, "LoadGarage");
    settings.capacity = 20;
    settings.floors = 1;
    settings.gates = 1;
    settings.gate_entry_inSec = 5;
    settings.tick_inSec = 60;
    settings.real_equivalent = 60;
    settings.output_mode = NORMAL;
    settings.max_ticks = 100;
    settings.rand_seed = 5;
    settings.entry_probability_perSec_prec = 4.0f;
    settings.is_leavable = NON_LEAVABLE;

    sim.settings = &settings;

    tick.current_tick = 1;
    tick.capacity_total = 20;
    tick.capacity_taken = 5;
    tick.enqueued = 2;
    tick.entered = 2;
    tick.departed = 0;
    tick.queue_length_end = 1;

    summary_to_save.total_ticks = 10;
    summary_to_save.arrivals_total = 25;
    summary_to_save.entered_total = 20;
    summary_to_save.departed_total = 18;
    summary_to_save.capacity_taken_percent_avg = 40.0f;
    summary_to_save.capacity_taken_percent_peak = 75.0f;
    summary_to_save.queue_length_avg = 1.5f;
    summary_to_save.queue_wait_avg_ticks = 3;
    summary_to_save.queue_wait_max_ticks = 9;
    summary_to_save.bad_parking_share_percent = 2.0f;

    full_path = savehandler_resolve_stats_path(dest_name);
    remove(full_path);

    assert(savehandler_save_tick(&sim, &tick, dest_name) == OK);
    assert(savehandler_save_summary(&sim, &summary_to_save, dest_name) == OK);

    list.p_summary = &loaded_summary;
    assert(savehandler_load_and_print(dest_name, &list, &mode) == OK);

    assert(list.p_tick_head != NULL);
    assert(list.p_tick_tail != NULL);

    assert(list.p_tick_head->current_tick == 1);
    assert(list.p_tick_head->capacity_total == 20);
    assert(list.p_tick_head->capacity_taken == 5);
    assert(list.p_tick_head->capacity_free == 15);
    assert(list.p_tick_head->enqueued == 2);
    assert(list.p_tick_head->entered == 2);
    assert(list.p_tick_head->departed == 0);
    assert(list.p_tick_head->queue_length_end == 1);

    assert(loaded_summary.total_ticks == 10);
    assert(loaded_summary.arrivals_total == 25);
    assert(loaded_summary.entered_total == 20);
    assert(loaded_summary.departed_total == 18);
    assert(loaded_summary.queue_wait_avg_ticks == 3);
    assert(loaded_summary.queue_wait_max_ticks == 9);

    cleanup_loaded_ticks(&list);
    remove(full_path);
}

void test_save_handler() {
    test_resolve_stats_path_defaults();
    printf("Save Handler - test resolve default paths passed\n");
    test_save_tick_normal_writes_header_and_data();
    printf("Save Handler - write header passed\n");
    test_save_summary_appends_summary_lines();
    printf("Save Handler - Appends Summary Line passed\n");
    test_load_and_print_reads_tick_and_summary();
    printf("Save Handler - Reads Tick and Summary passed\n");
    printf("All SaveHandler tests passed.\n");
}
