#include "SimulationObject.h"
#include <stddef.h>
#include <stdlib.h>

void tick(SimulationObject *p_obj, const int current_tick) {
    p_obj->tick(p_obj, current_tick);
}

void simulation_object_set_tick(SimulationObject *p_obj, const SimulationTickFunction tick_function) {
    p_obj->tick = tick_function;
}

SimulationTickFunction simulation_object_get_tick(const SimulationObject *p_obj) {
    return p_obj->tick;
}

int free_simulation_object(SimulationObject *p_obj) {
    if (p_obj == NULL) {
        return ERROR;
    }
    free(p_obj);
    return OK;
}
