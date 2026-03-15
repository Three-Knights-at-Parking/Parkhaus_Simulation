#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Simulation.h"
#include "Stats.h"
#include "Settings.h"
#include "types.h"

static Settings make_settings(int32_t max_ticks, uint8_t gates) {
    Settings s;
    memset(&s, 0, sizeof(s));
    s.name[0] = 'T'; s.name[1] = '\0';
    s.capacity = 30;
    s.floors = 1;
    s.gates = gates;
    s.gate_entry_inSec = 5;
    s.tick_inSec = 60;
    s.real_equivalent = 60;
    s.output_mode = NONE;
    s.max_ticks = max_ticks;
    s.rand_seed = 7;
    s.entry_probability_perSec_prec = 10.0f;
    s.min_parking_ticks = 1;
    s.max_parking_ticks = 10;
    return s;
}

static StatList *alloc_statlist_with_summary(void) {
    StatList *list = calloc(1, sizeof(StatList));
    StatsSummary *summary = calloc(1, sizeof(StatsSummary));
    assert(list != NULL);
    assert(summary != NULL);
    list->p_summary = summary;
    return list;
}

static void test_simulation_init_inputs_and_setup(void) {
    Simulation sim;
    Settings s = make_settings(1, 1);
    StatList *list = alloc_statlist_with_summary();
    memset(&sim, 0, sizeof(sim));

    assert(s.tick_inSec > 0);
    assert(s.gate_entry_inSec > 0);
    assert((s.real_equivalent % s.gate_entry_inSec) == 0);

    assert(simulation_init(NULL, &s, list) == ERROR);
    assert(simulation_init(&sim, NULL, list) == ERROR);
    assert(simulation_init(&sim, &s, NULL) == ERROR);

    assert(simulation_init(&sim, &s, list) == OK);
    assert(sim.settings == &s);
    assert(sim.parkhouse != NULL);
    assert(sim.parkhouse->gate_queues != NULL);
    assert(sim.parkhouse->gate_queues[0] != NULL);

    simulation_end(&sim);
}

static void test_simulation_tick_and_run_variants(void) {
    Simulation sim;
    Settings s = make_settings(1, 1);
    StatList *list = alloc_statlist_with_summary();
    memset(&sim, 0, sizeof(sim));

    assert(s.tick_inSec > 0);
    assert(s.gate_entry_inSec > 0);
    assert((s.real_equivalent % s.gate_entry_inSec) == 0);

    assert(simulation_init(&sim, &s, list) == OK);
    assert(simulation_tick(&sim) == OK);
    assert(sim.current_tick == 1);
    assert(sim.StatList->p_current_tick != NULL);

    s.max_ticks = 0;
    assert(simulation_run(&sim) == OK);

    s.max_ticks = -2;
    assert(simulation_run(&sim) == ERROR);

    assert(simulation_start(&sim) == ERROR);

    simulation_end(&sim);
}

static void test_simulation_null_and_invalid_runtime_paths(void) {
    Simulation sim;
    Settings s = make_settings(1, 1);
    StatList *list = alloc_statlist_with_summary();
    memset(&sim, 0, sizeof(sim));

    assert(simulation_tick(NULL) == ERROR);
    assert(simulation_run(NULL) == ERROR);
    simulation_end(NULL);

    assert(simulation_init(&sim, &s, list) == OK);
    sim.parkhouse = NULL;
    assert(simulation_tick(&sim) == ERROR);
    assert(simulation_run(&sim) == ERROR);
    simulation_end(&sim);
}

static void test_simulation_end_and_free(void) {
    assert(free_simulation(NULL) == ERROR);

    Simulation *heap_sim = calloc(1, sizeof(Simulation));
    Settings *heap_settings = calloc(1, sizeof(Settings));
    StatList *heap_list = calloc(1, sizeof(StatList));
    StatsSummary *heap_summary = calloc(1, sizeof(StatsSummary));
    assert(heap_sim && heap_settings && heap_list && heap_summary);

    *heap_settings = make_settings(0, 1);
    assert(heap_settings->tick_inSec > 0);
    assert(heap_settings->gate_entry_inSec > 0);
    assert((heap_settings->real_equivalent % heap_settings->gate_entry_inSec) == 0);
    heap_list->p_summary = heap_summary;

    assert(simulation_init(heap_sim, heap_settings, heap_list) == OK);
    assert(simulation_start(heap_sim) == OK);
    assert(free_simulation(heap_sim) == OK);

    free(heap_settings);
}

void test_simulation(void) {
    test_simulation_init_inputs_and_setup();
    test_simulation_tick_and_run_variants();
    test_simulation_null_and_invalid_runtime_paths();
    test_simulation_end_and_free();
    printf("Simulation tests passed\n");
}
