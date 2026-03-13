#include "SimulationObject.h"
#include <stddef.h>
#include <stdlib.h>

#include "utils/SafteyUtils.h"

void tick(SimulationObject *p_obj, const int current_tick) {
    if (checkNull(p_obj)) {
        print_warning_s("Ticking null object. That's not a thing mate.");
        return;
    }
    if (checkNull(p_obj->tick)) {
        print_error_s("Tick function of this object is null, won't tick!", HIGH);
        return;
    }
    p_obj->tick(p_obj, current_tick);
}

void simulation_object_set_tick(SimulationObject *p_obj, const SimulationTickFunction tick_function) {
    if (checkNull(p_obj)) {
        print_warning("Setting a null pointer as function pointer in Simulation Object! This is not good.");
    }
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
