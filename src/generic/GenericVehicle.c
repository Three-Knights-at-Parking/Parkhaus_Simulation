#include "GenericVehicle.h"

#include <stddef.h>

void generic_vehicle_init(GenericVehicle *p_vehicle, enum ObjectType type, SimulationTickFunction tick_function,
                          const uint32_t created_at, uint32_t parking_time) {
    if (p_vehicle == NULL) {
        return;
    }

    p_vehicle->base.type = type;
    p_vehicle->base.tick = tick_function;
    p_vehicle->created_at_tick = created_at;
    p_vehicle->leaving_in_ticks = parking_time;
}
