#include "GenericVehicle.h"
#include <stdlib.h>

#include "utils/SafteyUtils.h"


void generic_vehicle_init(GenericVehicle *p_vehicle,
                          enum ObjectType type,
                          SimulationTickFunction tick_function,
                          uint32_t created_at,
                          uint32_t parking_time) {
    if (p_vehicle == NULL) {
        return;
    }

    p_vehicle->base.id = 0;
    p_vehicle->base.type = type;
    p_vehicle->base.tick = tick_function;

    p_vehicle->p_next = NULL;
    p_vehicle->created_at_tick = created_at;
    p_vehicle->park_house_entered = 0;
    p_vehicle->park_house_left = 0;
    p_vehicle->leaving_in_ticks = parking_time;
    p_vehicle->current_slot = 0; //not implemented now
    p_vehicle->current_floor = 0; //not implemented now
}

void generic_vehicle_tick(SimulationObject *p_self, uint32_t current_tick)
{
    (void)current_tick;
    GenericVehicle *p_vehicle = (GenericVehicle *) p_self;
    p_vehicle->leaving_in_ticks--;
    if (p_vehicle->leaving_in_ticks == 0U) {
    }
}

int get_vehicle_minimum_space(GenericVehicle *p_vehicle)
{
    if (checkNull(p_vehicle)) {
        return ERROR;
    }
    //if (p_vehicle.type == CAR) //future Feature
    if (p_vehicle->base.type == CAR)
    {
        Car *p_car = (Car *) p_vehicle;
        return p_car->minimum_spaces;
    }
    return ERROR;
}

int get_vehicle_space_needed(GenericVehicle *p_vehicle)
{
    if (checkNull(p_vehicle)) {
        return ERROR;
    }
    //if (p_vehicle.type == CAR) //future Feature

    Car *p_car = (Car *) p_vehicle;

    return p_car->spaces_needed;

}
