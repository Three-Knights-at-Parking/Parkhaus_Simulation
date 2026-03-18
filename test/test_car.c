#include <assert.h>
#include <stdio.h>

#include "Car.h"
#include "types.h"

static void test_car_create(void)
{
    Car *c1 = car_create(10, 20, Car_Space);
    Car *c2 = car_create(0, 1, 2);
    Car *c3 = car_create(123, 456, 99);

    assert(c1 != NULL);
    assert(c2 != NULL);
    assert(c3 != NULL);

    assert(c1->base.base.type == CAR);
    assert(c1->base.created_at_tick == 10);
    assert(c1->base.leaving_in_ticks == 20);
    assert(c1->minimum_spaces == Car_Space);
    assert(c1->spaces_needed == Car_Space);

    assert(c2->base.base.type == CAR);
    assert(c2->base.created_at_tick == 0);
    assert(c2->base.leaving_in_ticks == 1);
    assert(c2->minimum_spaces == Car_Space);
    assert(c2->spaces_needed == Car_Space);

    assert(c3->base.base.type == CAR);
    assert(c3->base.created_at_tick == 123);
    assert(c3->base.leaving_in_ticks == 456);
    assert(c3->minimum_spaces == Car_Space);
    assert(c3->spaces_needed == Car_Space);

    assert(car_destroy(c1) == OK);
    assert(car_destroy(c2) == OK);
    assert(car_destroy(c3) == OK);
}

static void test_car_destroy(void)
{
    Car *c = car_create(5, 10, Car_Space);
    assert(c != NULL);

    assert(car_destroy(NULL) == ERROR);
    assert(car_destroy(c) == OK);
}

void test_car(void)
{
    test_car_create();
    printf("Car - create passed\n");

    test_car_destroy();
    printf("Car - destroy passed\n");

    printf("All Car tests passed\n");
}