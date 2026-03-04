#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_CAR_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_CAR_H

#include "types.h"
/**
 * Normal car (only type right now) that can enter a Queue and Parkhaus. Car is typically
 * owned by Queue or Parkhaus and inherits GenericVehicle (and in turn SimulationObject.psc).
 */

    /**
     * @brief Create and initialize a new Car object.
     * @param created_at Tick when this car was created.
     * @param parking_time Maximum time (in ticks) this car wants to stay
     *                     in the system (the tick it wants to leave the parking slot,
     *                     or tries to leave the queue).
     * @param spaces_needed Actual number of spaces this car will occupy.
     * @return Pointer to the newly allocated Car, or NULL on allocation failure.
     *         Ownership is with the caller.
     */
    Car *car_create(uint32_t created_at,
                    uint32_t parking_time,
                    uint8_t spaces_needed);

    /**
     * @brief Free a Car object.
     *        This does NOT remove the car from any Parkhaus or Queue lists;
     *        those structures must unlink the car before calling this. Never call this directly
     *        unless you are the owner of the car.
     * @param p_car Pointer to the Car to free (can be NULL).
     */
    void car_destroy(Car *p_car);

    /**
     * @brief Tick function for Car.
     * @param p_self Pointer to the SimulationObject.psc.
     * @param current_tick Current simulation tick.
     */
    void car_tick(SimulationObject *p_self, uint32_t current_tick);

#endif
