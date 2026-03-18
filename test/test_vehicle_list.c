#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils/VehicleList.h"
#include "Car.h"
#include "types.h"

static Car *make_test_car(uint32_t created_at)
{
    Car *c = car_create(created_at, created_at + 10U, Car_Space);
    assert(c != NULL);
    return c;
}

static void test_vehicle_list_count_and_append(void)
{
    GenericVehicle *head = NULL;
    GenericVehicle *tail = NULL;

    Car *c1 = make_test_car(1);
    Car *c2 = make_test_car(2);
    Car *c3 = make_test_car(3);

    assert(vehicle_list_count(NULL) == 0);

    vehicle_list_append(NULL, &tail, &c1->base);
    assert(head == NULL && tail == NULL);

    vehicle_list_append(&head, &tail, NULL);
    assert(head == NULL && tail == NULL);

    vehicle_list_append(&head, &tail, &c1->base);
    assert(head == &c1->base);
    assert(tail == &c1->base);
    assert(vehicle_list_count(head) == 1);

    vehicle_list_append(&head, &tail, &c2->base);
    vehicle_list_append(&head, &tail, &c3->base);
    assert(head == &c1->base);
    assert(tail == &c3->base);
    assert(vehicle_list_count(head) == 3);

    car_destroy(c1);
    car_destroy(c2);
    car_destroy(c3);
}

static void test_vehicle_list_pop_front(void)
{
    GenericVehicle *head = NULL;
    GenericVehicle *tail = NULL;

    Car *c1 = make_test_car(10);
    Car *c2 = make_test_car(20);
    Car *c3 = make_test_car(30);

    assert(vehicle_list_pop_front(NULL, &tail) == NULL);
    assert(vehicle_list_pop_front(&head, NULL) == NULL);
    assert(vehicle_list_pop_front(&head, &tail) == NULL);

    vehicle_list_append(&head, &tail, &c1->base);
    vehicle_list_append(&head, &tail, &c2->base);
    vehicle_list_append(&head, &tail, &c3->base);

    GenericVehicle *p = vehicle_list_pop_front(&head, &tail);
    assert(p == &c1->base);
    assert(head == &c2->base);
    assert(tail == &c3->base);
    assert(p->p_next == NULL);

    p = vehicle_list_pop_front(&head, &tail);
    assert(p == &c2->base);
    assert(head == &c3->base);
    assert(tail == &c3->base);

    p = vehicle_list_pop_front(&head, &tail);
    assert(p == &c3->base);
    assert(head == NULL);
    assert(tail == NULL);

    assert(vehicle_list_pop_front(&head, &tail) == NULL);

    car_destroy(c1);
    car_destroy(c2);
    car_destroy(c3);
}

static void test_vehicle_list_remove(void)
{
    GenericVehicle *head = NULL;
    GenericVehicle *tail = NULL;

    Car *c1 = make_test_car(100);
    Car *c2 = make_test_car(200);
    Car *c3 = make_test_car(300);
    Car *c4 = make_test_car(400);

    vehicle_list_append(&head, &tail, &c1->base);
    vehicle_list_append(&head, &tail, &c2->base);
    vehicle_list_append(&head, &tail, &c3->base);
    vehicle_list_append(&head, &tail, &c4->base);

    assert(vehicle_list_remove(NULL, &tail, &c1->base) == ERROR);
    assert(vehicle_list_remove(&head, NULL, &c1->base) == ERROR);
    assert(vehicle_list_remove(&head, &tail, NULL) == ERROR);
    assert(vehicle_list_remove(&head, &tail, NULL) == ERROR);

    assert(vehicle_list_remove(&head, &tail, &c1->base) == OK);
    assert(head == &c2->base);
    assert(tail == &c4->base);
    assert(c1->base.p_next == NULL);

    assert(vehicle_list_remove(&head, &tail, &c3->base) == OK);
    assert(head == &c2->base);
    assert(tail == &c4->base);
    assert(c3->base.p_next == NULL);

    assert(vehicle_list_remove(&head, &tail, &c4->base) == OK);
    assert(head == &c2->base);
    assert(tail == &c2->base);
    assert(c4->base.p_next == NULL);

    assert(vehicle_list_remove(&head, &tail, &c4->base) == ERROR);

    car_destroy(c1);
    car_destroy(c2);
    car_destroy(c3);
    car_destroy(c4);
}
static void test_vehicle_list_remove_all(void)
{
    GenericVehicle *head = NULL;
    GenericVehicle *tail = NULL;

    Car *c1 = make_test_car(1);
    Car *c2 = make_test_car(2);
    Car *c3 = make_test_car(3);

    vehicle_list_append(&head, &tail, &c1->base);
    vehicle_list_append(&head, &tail, &c2->base);
    vehicle_list_append(&head, &tail, &c3->base);

    assert(vehicle_list_count(head) == 3);

    vehicle_list_remove_all(NULL, &tail);
    vehicle_list_remove_all(&head, NULL);

    vehicle_list_remove_all(&head, &tail);
    assert(head == NULL);
    assert(tail == NULL);

    // ownership has been transferred to remove_all(), so no car_destroy() here
}

void test_vehicle_list_generic(void)
{
    test_vehicle_list_count_and_append();
    printf("VehicleList - count/append passed\n");

    test_vehicle_list_pop_front();
    printf("VehicleList - pop_front passed\n");

    test_vehicle_list_remove();
    printf("VehicleList - remove passed\n");

    test_vehicle_list_remove_all();
    printf("VehicleList - remove_all passed\n");

    printf("All VehicleList tests passed\n");
}