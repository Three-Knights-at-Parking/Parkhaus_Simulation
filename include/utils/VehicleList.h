#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_VEHICLELIST_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_VEHICLELIST_H

#include "../types.h"
/**
 * Helper class for traversing and handling dynamic linked lists for Queue and Parkhaus and future
 * expansions that might need this.
 */

    /**
     * @brief Append a vehicle to the end of a singly linked list.
     * @param pp_head Pointer to head pointer of the list.
     * @param pp_tail Pointer to tail pointer of the list.
     * @param p_vehicle Vehicle to append; its p_next must be NULL.
     */
    void vehicle_list_append(GenericVehicle **pp_head, GenericVehicle **pp_tail, GenericVehicle *p_vehicle);

    /**
     * @brief Pop the first vehicle from the list (FIFO).
     * @param pp_head Pointer to head pointer.
     * @param pp_tail Pointer to tail pointer.
     * @return Pointer to popped GenericVehicle or NULL if list is empty.
     */
    GenericVehicle *vehicle_list_pop_front(GenericVehicle **pp_head, GenericVehicle **pp_tail);

    /**
     * @brief Remove a specific vehicle from the list, preserving the order of others.
     * @param pp_head Pointer to head pointer.
     * @param pp_tail Pointer to tail pointer.
     * @param p_target vehicle to remove.
     * @return 0 on success, non-zero if not found.
     */
    int vehicle_list_remove(GenericVehicle **pp_head, GenericVehicle **pp_tail, GenericVehicle *p_target);

    /**
     * @brief Remove all vehicles from this list.
     *        This function takes ownership of the cars and will clean up every
     *        node itself. This function is usefull for cleanup after simulation
     *        end.
     * @param pp_head Pointer to head pointer of the list.
     * @param pp_tail Pointer to tail pointer of the list.
     */
    void vehicle_list_remove_all(GenericVehicle **pp_head, GenericVehicle **pp_tail);


    /**
     * @brief Count nodes in the list.
     * @param p_head Head of the list.
     * @return Number of vehicle in the list.
     */
    uint16_t vehicle_list_count(const GenericVehicle *p_head);

#endif
