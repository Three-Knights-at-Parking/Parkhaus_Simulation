#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GenericVehicle.h"
#include "SimulationObject.h"
#include "utils/VehicleList.h"
#include "utils/StatList.h"
#include "types.h"

static int g_mock_tick_call_count = 0;
static int g_mock_last_tick_time = 0;

void mock_tick_function(SimulationObject *p_obj, int current_tick) {
    g_mock_tick_call_count++;
    g_mock_last_tick_time = current_tick;
}
static void test_simulation_object(void) {
    SimulationObject *p_obj = calloc(1, sizeof(SimulationObject));
    g_mock_tick_call_count = 0;
    simulation_object_set_tick(p_obj, (SimulationTickFunction) mock_tick_function);
    assert(simulation_object_get_tick(p_obj) == (SimulationTickFunction) mock_tick_function);

    tick(p_obj, 42);
    assert(g_mock_tick_call_count == 1);
    assert(g_mock_last_tick_time == 42);

    simulation_object_set_tick(p_obj, NULL);
    tick(p_obj, 43);
    assert(g_mock_tick_call_count == 1);

    assert(free_simulation_object(NULL) != OK);

    int free_res = free_simulation_object(p_obj);
    assert(free_res == OK);

}

static void test_generic_vehicle(void) {
    GenericVehicle gv;
    memset(&gv, 0, sizeof(gv));
    generic_vehicle_init(&gv, CAR, (SimulationTickFunction) mock_tick_function, 10, 100);
    assert(gv.base.type == CAR);
    assert(gv.created_at_tick == 10);
    assert(gv.leaving_in_ticks == 100);
    assert(get_vehicle_minimum_space(&gv) >= 0);
    assert(get_vehicle_space_needed(&gv) >= 0);

    assert(get_vehicle_minimum_space(NULL) <= ERROR);
    assert(get_vehicle_space_needed(NULL) <= ERROR);
}
static void test_vehicle_list_generic(void) {
    GenericVehicle *head = NULL;
    GenericVehicle *tail = NULL;
    GenericVehicle *v1 = calloc(1, sizeof(GenericVehicle));
    GenericVehicle *v2 = calloc(1, sizeof(GenericVehicle));
    GenericVehicle *v3 = calloc(1, sizeof(GenericVehicle));

    v1->base.id = 1;
    v2->base.id = 2;
    v3->base.id = 3;
    assert(vehicle_list_count(head) == 0);
    assert(vehicle_list_pop_front(&head, &tail) == NULL);
    assert(vehicle_list_remove(&head, &tail, v1) != OK);
    vehicle_list_remove_all(&head, &tail);

    assert(vehicle_list_count(head) == 0);
    vehicle_list_append(&head, &tail, v1);
    assert(vehicle_list_count(head) == 1);
    assert(head == v1 && tail == v1);
    vehicle_list_append(&head, &tail, v2);
    vehicle_list_append(&head, &tail, v3);
    assert(vehicle_list_count(head) == 3);
    assert(head == v1 && tail == v3);
    GenericVehicle *popped = vehicle_list_pop_front(&head, &tail);
    assert(popped == v1);
    assert(vehicle_list_count(head) == 2);
    assert(head == v2);
    free(popped);

    GenericVehicle *v_ghost = calloc(1, sizeof(GenericVehicle));
    v_ghost->base.id = 99;
    assert(vehicle_list_remove(&head, &tail, v_ghost) != OK);
    free(v_ghost);

    int rem_status = vehicle_list_remove(&head, &tail, v3);
    assert(rem_status == OK); // Assuming 0 is OK
    assert(vehicle_list_count(head) == 1);
    assert(head == v2 && tail == v2);
    free(v3);
    vehicle_list_remove_all(&head, &tail);
    assert(vehicle_list_count(head) == 0);
    assert(head == NULL && tail == NULL);
    // v2 is freed internally by vehicle_list_remove_all.
}
static void test_stat_list(void) {
    Simulation sim;
    memset(&sim, 0, sizeof(sim));
    StatList list;
    memset(&list, 0, sizeof(list));
    sim.StatList = &list;

    StatsSummary summary;
    memset(&summary, 0, sizeof(summary));

    // --- Edge Cases ---
    assert(statlist_compute_summary(&sim, &summary) != OK);
    statlist_clear(&sim);

    StatsTick *t1 = calloc(1, sizeof(StatsTick));
    assert(statlist_append(NULL, t1) != OK);
    assert(statlist_append(&sim, NULL) != OK);

    // --- Happy Paths ---
    StatsTick *t2 = calloc(1, sizeof(StatsTick));
    t1->current_tick = 1;
    t1->entered = 5;
    t2->current_tick = 2;
    t2->entered = 10;

    assert(statlist_append(&sim, t1) == OK);
    assert(statlist_append(&sim, t2) == OK);

    int sum_res = statlist_compute_summary(&sim, &summary);
    assert(sum_res == OK);
    statlist_clear(&sim);
}


void test_generics() {
    test_simulation_object();
    printf("Generics - SimulationObject passed \n");
    test_generic_vehicle();
    printf("Generics - Vehicle passed\n");
    test_vehicle_list_generic();
    printf("Generics - Vehicle List passed\n");
    test_stat_list();
    printf("Generics - Stat List passed\n");
    printf("All Generic tests passed successfully!\n");
}