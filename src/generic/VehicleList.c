#include "utils/VehicleList.h"
#include "utils/SafteyUtils.h"
#include <stdlib.h>

void vehicle_list_append(GenericVehicle **pp_head, GenericVehicle **pp_tail, GenericVehicle *p_vehicle) {
    if (checkNull(pp_head) || checkNull(pp_tail) || checkNull(p_vehicle)) {
        print_error_s("Invalid pointers passed.", HIGH);
        return;
    }
    p_vehicle->p_next = NULL;
    if (*pp_head == NULL) {
        *pp_head = p_vehicle;
        *pp_tail = p_vehicle;
    }
    else {
        (*pp_tail)->p_next = p_vehicle;
        *pp_tail = p_vehicle;
    }
}

GenericVehicle *vehicle_list_pop_front(GenericVehicle **pp_head, GenericVehicle **pp_tail) {
    if (checkNull(pp_head) || checkNull(pp_tail) || checkNull(*pp_head)) {
        return NULL;
    }
    GenericVehicle *p_popped = *pp_head;
    *pp_head = p_popped->p_next;
    if (*pp_head == NULL) {
        *pp_tail = NULL;
    }
    p_popped->p_next = NULL;

    return p_popped;
}

int vehicle_list_remove(GenericVehicle **pp_head, GenericVehicle **pp_tail, const GenericVehicle *p_target) {
    if (checkNull(pp_head) || checkNull(pp_tail) || checkNull(p_target) || checkNull(*pp_head)) {
        return ERROR;
    }

    GenericVehicle *p_prev = NULL;
    GenericVehicle *p_curr = *pp_head;
    while (p_curr != NULL) {
        if (p_curr == p_target) {

            // Target is head !
            if (p_prev == NULL) {
                *pp_head = p_curr->p_next;
            }
            // target is in the middle
            else {
                p_prev->p_next = p_curr->p_next;
            }

            // target is tail !
            if (p_curr == *pp_tail) {
                *pp_tail = p_prev;
            }
            p_curr->p_next = NULL;
            return OK;
        }
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }
    print_warning_s("vehicle_list_remove: Target vehicle not found in the list.");
    return ERROR;
}

void vehicle_list_remove_all(GenericVehicle **pp_head, GenericVehicle **pp_tail) {
    if (checkNull(pp_head)|| checkNull(pp_tail)) {
        return;
    }

    GenericVehicle *p_curr = *pp_head;
    GenericVehicle *p_next = NULL;

    /**
     * We take ownership here.
     */
    while (p_curr != NULL) {
        p_next = p_curr->p_next;
        free(p_curr); // This will only work as long as GenericVehicle is the first member of the Vehicle!
        p_curr = p_next;
    }
    *pp_head = NULL;
    *pp_tail = NULL;
}

uint16_t vehicle_list_count(const GenericVehicle *p_head) {
    uint16_t count = 0;
    const GenericVehicle *p_curr = p_head;

    while (p_curr != NULL) {
        count++;
        p_curr = p_curr->p_next;
    }

    return count;
}