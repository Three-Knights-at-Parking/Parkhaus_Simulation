#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parkhaus.h"
#include "Queue.h"
#include "Car.h"
#include "Stats.h"
#include "utils/RNG.h"
#include "types.h"

static Settings make_settings(uint8_t gates) {
    Settings s;
    memset(&s, 0, sizeof(s));
    s.capacity = 20;
    s.floors = 1;
    s.gates = gates;
    s.real_equivalent = 60;
    s.gate_entry_inSec = 5;
    s.min_parking_ticks = 1;
    s.max_parking_ticks = 4;
    s.entry_probability_perSec_prec = 10.0f;
    s.rand_seed = 123;
    s.max_ticks = 5;
    s.output_mode = NONE;
    return s;
}

static void setup_stats(Simulation *sim, StatList *list, StatsSummary *summary) {
    memset(sim, 0, sizeof(*sim));
    memset(list, 0, sizeof(*list));
    memset(summary, 0, sizeof(*summary));
    list->p_summary = summary;
    sim->StatList = list;
}

static Queue **alloc_gate_queues(uint8_t gates, uint8_t max_size) {
    Queue **arr = calloc(gates, sizeof(Queue *));
    assert(arr != NULL);
    for (uint8_t i = 0; i < gates; ++i) {
        arr[i] = calloc(1, sizeof(Queue));
        assert(arr[i] != NULL);
        assert(queue_init(arr[i], max_size) == OK);
    }
    return arr;
}

static void free_gate_queues(Queue **arr, uint8_t gates) {
    if (!arr) return;
    for (uint8_t i = 0; i < gates; ++i) {
        if (arr[i]) {
            queue_free(arr[i]);
            free(arr[i]);
        }
    }
    free(arr);
}

static Car *mkcar(uint32_t created, uint32_t leave_ticks) {
    Car *c = car_create(created, leave_ticks, Car_Space);
    assert(c != NULL);
    return c;
}

static void test_parkhaus_init_and_space(void) {
    Settings s = make_settings(1);
    Queue **queues = alloc_gate_queues(1, 5);
    Parkhaus p;
    memset(&p, 0, sizeof(p));

    assert(parkhouse_init(NULL, &s, queues) == ERROR);
    assert(parkhouse_init(&p, NULL, queues) == ERROR);
    assert(parkhouse_init(&p, &s, NULL) == ERROR);
    assert(parkhouse_init(&p, &s, queues) == OK);

    p.capacity_taken = 0;
    assert(get_open_space(&p) == p.capacity);
    p.capacity_taken = p.capacity;
    assert(get_open_space(&p) == 0);
    p.capacity_taken = p.capacity + 1;
    assert(get_open_space(&p) == (uint16_t)ERROR);

    parkhouse_free(&p);
    free_gate_queues(queues, 1);
}

static void test_park_vehicle_and_remove_vehicle(void) {
    Parkhaus p;
    memset(&p, 0, sizeof(p));

    Car *c1 = mkcar(1, 5);
    Car *c2 = mkcar(1, 5);
    Car *c3 = mkcar(1, 5);

    assert(park_vehicle(NULL, &c1->base) == ERROR);
    assert(park_vehicle(&p, NULL) == ERROR);
    assert(park_vehicle(&p, &c1->base) == OK);
    assert(p.p_parked_head == &c1->base);

    assert(park_vehicle(&p, &c2->base) == OK);
    assert(p.p_parked_tail == &c2->base);

    assert(park_vehicle(&p, &c3->base) == OK);
    assert(p.p_parked_tail == &c3->base);

    assert(remove_vehicle(NULL) == ERROR);
    assert(parkhouse_free(&p) == OK);
}

static void test_fill_open_demand_and_queue_helpers(void) {
    Settings s = make_settings(1);
    Queue q;
    Parkhaus p;
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_stats(&sim, &list, &summary);
    memset(&q, 0, sizeof(q));
    memset(&p, 0, sizeof(p));
    assert(queue_init(&q, 2) == OK);
    p.capacity = 20;
    p.capacity_taken = 0;

    assert(StatsTick_init(&sim, 20, 1) == OK);
    assert(fill_from_queue(NULL, &q, &p.p_parked_head) == (uint16_t)ERROR);

    GenericVehicle *out = NULL;
    assert(fill_from_queue(&p, &q, &out) == (uint16_t)ERROR);

    Car *c = mkcar(1, 3);
    assert(queue_enqueue(&q, &c->base) == OK);
    uint16_t used = fill_from_queue(&p, &q, &out);
    assert(used >= Car_Space);
    assert(out == &c->base);
    assert(queue_length(&q) == 0);

    assert(open_demand(&list, &q, 3, 1, &s) == OK);
    assert(queue_length(&q) == 2);
    assert(list.p_current_tick->queue_rejections == 1);

    queue_free(&q);
    parkhouse_free(&p);
    StatsTick_free(&list);
}

static void test_vehicle_entry_exit_and_leaving(void) {
    Settings s = make_settings(1);
    Queue **queues = alloc_gate_queues(1, 5);
    Parkhaus p;
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_stats(&sim, &list, &summary);
    assert(StatsTick_init(&sim, 20, 5) == OK);

    memset(&p, 0, sizeof(p));
    assert(parkhouse_init(&p, &s, queues) == OK);

    Car *c = mkcar(3, 2);
    assert(park_vehicle(&p, &c->base) == OK);
    assert(update_on_vehicle_entry(&p, &list, &c->base, 2, 5) == OK);
    assert(p.capacity_taken == 2);

    assert(update_on_vehicle_entry(&p, &list, &c->base, 100, 5) == ERROR);
    c->base.park_house_left = 5;
    assert(update_on_vehicle_exit(&p, &list, &c->base, 2, 5) == ERROR);
    c->base.park_house_left = 0;
    assert(update_on_vehicle_exit(&p, &list, &c->base, 2, 5) == OK);
    assert(p.capacity_taken == 0);

    Car *c2 = mkcar(1, 1);
    park_vehicle(&p, &c2->base);
    p.capacity_taken = 2;
    assert(vehicle_leaving(&p, &list, &p.p_parked_head, &c2->base, 6) == OK);

    Car *ghost = mkcar(1, 1);
    assert(vehicle_leaving(&p, &list, &p.p_parked_head, &ghost->base, 6) == ERROR);

    parkhouse_free(&p);
    free_gate_queues(queues, 1);
    StatsTick_free(&list);
}

static void test_tick_paths(void) {
    Settings s = make_settings(1);
    Queue **queues = alloc_gate_queues(1, 10);
    Parkhaus p;
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_stats(&sim, &list, &summary);
    assert(StatsTick_init(&sim, 20, 1) == OK);

    memset(&p, 0, sizeof(p));
    assert(parkhouse_init(&p, &s, queues) == OK);
    assert(rng_init(&s) == OK);

    Car *leaver = mkcar(0, 1);
    assert(park_vehicle(&p, &leaver->base) == OK);
    p.capacity_taken = 2;
    assert(parkhouse_tick_empty_general(1, &p, &s, &list, &p.p_parked_head) == OK);

    queue_set_demand(queues[0], 2);
    assert(parkhouse_tick_fill_general(1, &p, &s, &list, &p.p_parked_head, queues[0]) == OK);

    assert(parkhouse_tick((SimulationObject *)&p, &s, &list, 2) == OK);

    Settings sm = make_settings(2);
    sm.real_equivalent = 61;
    Queue **mq = alloc_gate_queues(2, 5);
    Parkhaus pm;
    memset(&pm, 0, sizeof(pm));
    assert(parkhouse_init(&pm, &sm, mq) == OK);
    assert(parkhouse_fill_subtick(1, &pm, &sm, &list, *mq) == ERROR);
    sm.real_equivalent = 60;
    queue_set_demand(mq[0], 1);
    queue_set_demand(mq[1], 1);
    assert(parkhouse_fill_subtick(1, &pm, &sm, &list, *mq) == OK);
    assert(parkhouse_fill_subtick_routine(1, &pm, &sm, &list, mq[0], 1) == OK);

    parkhouse_free(&pm);
    free_gate_queues(mq, 2);
    parkhouse_free(&p);
    free_gate_queues(queues, 1);
    StatsTick_free(&list);
}

static void test_additional_error_and_edge_paths(void) {
    Settings s = make_settings(1);
    Queue **queues = alloc_gate_queues(1, 2);
    Parkhaus p;
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_stats(&sim, &list, &summary);
    memset(&p, 0, sizeof(p));

    assert(StatsTick_init(&sim, 20, 1) == OK);
    assert(parkhouse_init(&p, &s, queues) == OK);

    assert(get_open_space(NULL) == (uint16_t)ERROR);

    assert(create_random_vehicle(1, NULL) == NULL);
    assert(queue_add_random_vehicle(NULL, 1, &s) == ERROR);
    assert(queue_add_random_vehicle(queues[0], 1, NULL) == ERROR);

    assert(open_demand(NULL, queues[0], 1, 1, &s) == ERROR);
    assert(open_demand(&list, NULL, 1, 1, &s) == ERROR);

    assert(update_on_vehicle_entry(NULL, &list, NULL, 1, 1) == ERROR);
    assert(update_on_vehicle_exit(NULL, &list, NULL, 1, 1) == ERROR);

    assert(vehicle_leaving(NULL, &list, &p.p_parked_head, NULL, 1) == ERROR);
    assert(remove_vehicle(NULL) == ERROR);
    assert(parkhouse_free(NULL) == ERROR);

    /* second path for parkhouse_tick: wrong SimulationObject type */
    SimulationObject wrong;
    memset(&wrong, 0, sizeof(wrong));
    wrong.type = QUEUE;
    assert(parkhouse_tick(&wrong, &s, &list, 1) == ERROR);

    parkhouse_free(&p);
    free_gate_queues(queues, 1);
    StatsTick_free(&list);
}

void test_parkhaus(void) {
    test_parkhaus_init_and_space();
    test_park_vehicle_and_remove_vehicle();
    test_fill_open_demand_and_queue_helpers();
    test_vehicle_entry_exit_and_leaving();
    test_tick_paths();
    test_additional_error_and_edge_paths();
    printf("Parkhaus tests passed\n");
}
