#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATIONOBJECT_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SIMULATIONOBJECT_H

#include "types.h"
/**
 * Base Simulation Object that is attached to every object that needs to be ticked.
 * This MUST be the first child of any Object that wants to implement SimulationObject.psc,
 * as calling the tick function is done via destructive casting! See GenericVehicle.h for reference.
 */

    /**
     * Tick this SimulationObject.psc once.
     * This will call the tick-function stored in p_obj->tick if it is not NULL.
     * @param p_obj Pointer to the SimulationObject.psc to tick.
     * @param current_tick Current simulation time (tick counter).
     */
    void tick(SimulationObject *p_obj, int current_tick);

    /**
     * Setter function to set the tick-function for this SimulationObject.psc.
     * Generally you should only set the tick function at init of the object and not change it.
     * @param p_obj Pointer to the SimulationObject.psc whose tick function should be set.
     * @param tick_function Pointer to the SimulationTickFunction for this object.
     */
    void simulation_object_set_tick(SimulationObject *p_obj,
                                    SimulationTickFunction tick_function);

    /**
     * If you ever need to get the SimulationFunction for this Object.
     * @param p_obj Pointer to the SimulationObject.psc.
     * @return The SimulationTickFunction for this Object (may be NULL).
     */
    SimulationTickFunction simulation_object_get_tick(const SimulationObject *p_obj);

    /**
     * Free this Simulation-Object's memory. SimulationObject.psc does not take ownership of the tick function that belongs to the parent!
     * In general, you never want to call this function directly, unless you created a plain Simulation-Object under your control.
     * Call the parent object's free function, which takes ownership of the underlying Simulation-Object and the associated tick
     * function.
     * @param p_obj Pointer to the SimulationObject.psc.
     * @return 0 on success, non-zero on error.
     */
    int free_simulation_object(SimulationObject *p_obj);

#endif
