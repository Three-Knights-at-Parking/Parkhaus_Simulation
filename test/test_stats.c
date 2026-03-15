#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stats.h"
#include "Parkhaus.h"
#include "Car.h"
#include "types.h"

static void setup_sim_with_list(Simulation *sim, StatList *list, StatsSummary *summary) {
    memset(sim, 0, sizeof(*sim));
    memset(list, 0, sizeof(*list));
    memset(summary, 0, sizeof(*summary));
    list->p_summary = summary;
    sim->StatList = list;
}

static Car *make_car(uint32_t created, uint32_t entered, uint32_t left, uint8_t min_space, uint8_t needed) {
    Car *car = car_create(created, 5, min_space);
    assert(car != NULL);
    car->base.park_house_entered = entered;
    car->base.park_house_left = left;
    car->minimum_spaces = min_space;
    car->spaces_needed = needed;
    return car;
}

static void test_stats_init_and_free(void) {
    Simulation sim;
    memset(&sim, 0, sizeof(sim));

    assert(StatList_init(NULL) == NULL);
    StatList *stats = StatList_init(&sim);
    assert(stats != NULL);
    assert(sim.StatList == stats);

    assert(StatsTick_init(&sim, 100, 2) == OK);
    assert(stats_get_latest_tick(stats) != NULL);
    assert(stats_get_latest_tick(stats)->current_tick == 2);

    assert(StatsTick_free(stats) == OK);
    assert(stats->p_tick_head == NULL && stats->p_tick_tail == NULL);

    assert(StatList_free(stats) == OK);
}

static void test_stats_init_and_free_errors(void) {
    Simulation sim;
    memset(&sim, 0, sizeof(sim));

    assert(StatsTick_init(NULL, 100, 1) == ERROR);
    assert(StatList_free(NULL) == ERROR);
    assert(StatsTick_free(NULL) == ERROR);

    StatList *stats = StatList_init(&sim);
    assert(stats != NULL);
    assert(StatsTick_free(stats) == OK);
    assert(StatList_free(stats) == OK);
}

static void test_stats_tick_add_and_capacity(void) {
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_sim_with_list(&sim, &list, &summary);

    assert(stats_tick_set_capacity(&list, 1, 1) == ERROR);
    assert(stats_tick_add_queue_rejections(&list, 3) == ERROR);
    assert(stats_tick_add_arrivals_generated(&list, 3) == ERROR);
    assert(stats_tick_add_blocker_full_active(&list) == ERROR);
    assert(stats_tick_add_vehicle(&list, NULL, 1) == ERROR);

    assert(StatsTick_init(&sim, 20, 1) == OK);
    assert(stats_tick_set_capacity(&list, 8, 12) == OK);
    assert(list.p_current_tick->capacity_total == 20);
    assert(stats_tick_add_queue_rejections(&list, 2) == OK);
    assert(stats_tick_add_arrivals_generated(&list, 7) == OK);
    assert(stats_tick_add_blocker_full_active(&list) == OK);

    Car *enter_car = make_car(3, 5, 0, 2, 4);
    assert(stats_tick_add_vehicle(&list, &enter_car->base, 5) == OK);
    assert(list.p_current_tick->entered == 1);
    assert(list.p_current_tick->queue_wait_entered_sum_ticks == 2);
    assert(list.p_current_tick->bad_parking_cases == 1);

    Car *leave_car = make_car(1, 2, 5, 2, 2);
    assert(stats_tick_add_vehicle(&list, &leave_car->base, 5) == OK);
    assert(list.p_current_tick->departed == 1);
    assert(list.p_current_tick->parking_duration_departed_sum_ticks == 3);

    assert(remove_vehicle((GenericVehicle *)enter_car) == OK);
    assert(remove_vehicle((GenericVehicle *)leave_car) == OK);
    assert(StatsTick_free(&list) == OK);
}

static void test_stats_build_summary(void) {
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_sim_with_list(&sim, &list, &summary);

    assert(stats_build_summary(NULL, &summary) == ERROR);
    assert(stats_build_summary(&list, NULL) == ERROR);

    StatsTick *t1 = calloc(1, sizeof(StatsTick));
    StatsTick *t2 = calloc(1, sizeof(StatsTick));
    assert(t1 && t2);

    t1->current_tick = 1; t1->capacity_total = 10; t1->capacity_taken = 10; t1->capacity_free = 0;
    t1->arrivals_generated = 5; t1->enqueued = 2; t1->entered = 3; t1->departed = 1; t1->queue_length_end = 4;
    t1->queue_rejections = 1; t1->queue_wait_entered_sum_ticks = 6; t1->queue_wait_entered_count = 3; t1->queue_wait_max_ticks_tick = 4;
    t1->parking_duration_departed_sum_ticks = 5; t1->parking_duration_departed_count = 1; t1->blocker_full_active = 1; t1->bad_parking_cases = 1;

    t2->current_tick = 2; t2->capacity_total = 10; t2->capacity_taken = 5; t2->capacity_free = 5;
    t2->arrivals_generated = 4; t2->enqueued = 1; t2->entered = 2; t2->departed = 2; t2->queue_length_end = 0;
    t2->queue_wait_entered_sum_ticks = 2; t2->queue_wait_entered_count = 1; t2->queue_wait_max_ticks_tick = 2;
    t2->parking_duration_departed_sum_ticks = 6; t2->parking_duration_departed_count = 2;

    list.p_tick_head = t1;
    list.p_tick_tail = t2;
    t1->p_next = t2;
    t2->p_prev = t1;

    assert(stats_build_summary(&list, &summary) == OK);
    assert(summary.total_ticks == 2);
    assert(summary.first_full_tick == 1);
    assert(summary.full_ticks == 1);
    assert(summary.arrivals_total == 9);
    assert(summary.entered_total == 5);
    assert(summary.departed_total == 3);
    assert(summary.queue_rejections_total == 1);
    assert(summary.queue_wait_avg_ticks == 2);
    assert(summary.queue_wait_max_ticks == 4);
    assert(summary.parking_duration_avg_ticks == 3);
    assert(summary.bad_parking_cases_total == 1);

    assert(StatsTick_free(&list) == OK);
}

static void test_stats_get_latest_tick_empty_and_nonempty(void) {
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_sim_with_list(&sim, &list, &summary);

    assert(stats_get_latest_tick(NULL) == NULL);
    assert(stats_get_latest_tick(&list) == NULL);

    assert(StatsTick_init(&sim, 50, 7) == OK);
    const StatsTick *latest = stats_get_latest_tick(&list);
    assert(latest != NULL);
    assert(latest->current_tick == 7);

    assert(StatsTick_free(&list) == OK);
}

void test_stats(void) {
    test_stats_init_and_free();
    test_stats_init_and_free_errors();
    test_stats_tick_add_and_capacity();
    test_stats_build_summary();
    test_stats_get_latest_tick_empty_and_nonempty();
    printf("Stats tests passed\n");
}
