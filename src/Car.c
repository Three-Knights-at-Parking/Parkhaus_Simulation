#include "Car.h"


#include <stdio.h>
#include <stdlib.h>
#include "utils/SafteyUtils.h"


#include "GenericVehicle.h"
#include "Parkhaus.h"

Car *car_create(uint32_t created_at, uint32_t parking_time, uint8_t spaces_needed) {
    Car *p_car = (Car *) calloc(1, sizeof(Car));
    if (p_car == NULL) {
        return NULL;
    }

    generic_vehicle_init(&p_car->base, CAR, car_tick, created_at, parking_time);

    p_car->minimum_spaces = Car_Space;
    p_car->spaces_needed = Car_Space; //will set after Car create through fill_from_queue()

    return p_car;
}

int car_destroy(Car *p_car) {
    if (p_car == NULL)
    {
        printf("car_destroy: car pointer is NULL\n");
        return ERROR;
    }
    free(p_car);
    return OK;
}

//FIXME Keep like this?
void car_tick(SimulationObject *p_self, uint32_t current_tick)
{
    GenericVehicle *p_vehicle = (GenericVehicle *) p_self;
    p_vehicle->leaving_in_ticks--;
}
