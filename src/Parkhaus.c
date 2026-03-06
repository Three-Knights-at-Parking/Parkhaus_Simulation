#include "Parkhaus.h"

#include <stdlib.h>

int parkhaus_init(Parkhaus *p_parkhaus, const Settings *p_settings, Queue **p_gate_queues) {
    if (p_parkhaus == NULL || p_settings == NULL || p_gate_queues == NULL) {
        return ERROR;
    }
    p_parkhaus->name = p_settings->name;
    p_parkhaus->capacity = p_settings->capacity;
    p_parkhaus->floors = p_settings->floors;
    p_parkhaus->capacity_taken = 0;
    p_parkhaus->gate_queues = p_gate_queues;
    p_parkhaus->p_parked_head = NULL;
    p_parkhaus->p_parked_tail = NULL;
    return OK;
}

int parkhaus_tick(SimulationObject *p_self, const Settings *p_settings, StatList *p_StatList, uint32_t current_tick) {
    if (p_self == NULL || p_settings == NULL || p_StatList == NULL) {return ERROR;}

}

int parkhaus_park_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle) {
    return park_vehicle(p_parkhaus, p_vehicle);
}

int parkhaus_remove_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle) {
    if (p_parkhaus == NULL || p_vehicle == NULL) {
        return ERROR;
    }

    (void) p_parkhaus;
    return OK;
}

float parkhaus_get_utilization(const Parkhaus *p_parkhaus) {
    if (p_parkhaus == NULL || p_parkhaus->capacity == 0) {
        return 0.0f;
    }

    return ((float) p_parkhaus->capacity_taken * 100.0f) / (float) p_parkhaus->capacity;
}

void parkhaus_free(Parkhaus *p_parkhaus) {
    if (p_parkhaus == NULL) {
        return;
    }

    p_parkhaus->p_parked_head = NULL;
    p_parkhaus->p_parked_tail = NULL;
    p_parkhaus->gate_queues = NULL;
}

int vehicles_leaving_end(Parkhaus *p_parkhaus, StatList *p_StatList) {
    (void) p_parkhaus;
    (void) p_StatList;
    return OK;
}

int parkhouse_tick_empty_general(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, GenericVehicle **pp_vehicle_list_head) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) pp_vehicle_list_head;
    return OK;
}

int parkhouse_tick_fill_general(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, GenericVehicle **pp_vehicle_list_head, Queue *p_gate_queue) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) pp_vehicle_list_head;
    (void) p_gate_queue;
    return OK;
}

int parkhouse_fill_subtick(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, Queue *p_gate_queues) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) p_gate_queues;
    return OK;
}

int parkhouse_fill_subtick_routine(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, Queue *p_gate_queue, int last_cycle) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) p_gate_queue;
    (void) last_cycle;
    return OK;
}

uint16_t fill_from_queue(Parkhaus *p_parkhaus, Queue *p_gate_queue, GenericVehicle **pp_vehicle) {
    (void) p_parkhaus;
    (void) p_gate_queue;
    (void) pp_vehicle;
    return 0;
}

int open_demand(StatList *p_StatList, Queue *p_gate_queue, uint16_t queue_max_len, uint16_t demand_remaining, uint32_t current_tick, Settings *p_settings) {
    (void) p_StatList;
    (void) p_gate_queue;
    (void) queue_max_len;
    (void) demand_remaining;
    (void) current_tick;
    (void) p_settings;
    return OK;
}

int vehicle_leaving(Parkhaus *p_parkhouse, StatList *p_StatList, GenericVehicle **pp_vehicle_list_head, GenericVehicle *p_vehicle) {
    (void) p_parkhouse;
    (void) p_StatList;
    (void) pp_vehicle_list_head;
    (void) p_vehicle;
    return OK;
}

Queue *parkhaus_create_gate_queues(uint32_t number_of_gates) {
    (void) number_of_gates;
    return NULL;
}

int parkhaus_enqueue_at_gate(Queue *p_gate_queues, uint32_t gate_index, GenericVehicle *p_vehicle) {
    (void) p_gate_queues;
    (void) gate_index;
    (void) p_vehicle;
    return OK;
}

int parkhaus_set_gate_demand(Queue *p_gate_queues, uint32_t gate_index, uint16_t demand_value) {
    (void) p_gate_queues;
    (void) gate_index;
    (void) demand_value;
    return OK;
}

int queue_add_random_vehicle(Queue *p_gate_queue, uint32_t current_tick, Settings *p_settings) {
    (void) p_gate_queue;
    (void) current_tick;
    (void) p_settings;
    return OK;
}

GenericVehicle *create_random_vehicle(uint32_t current_tick, Settings *p_settings) {
    (void) current_tick;
    (void) p_settings;
    return NULL;
}

int park_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle) {
    if (p_parkhaus == NULL || p_vehicle == NULL) {
        return ERROR;
    }

    p_vehicle->p_next = NULL;

    if (p_parkhaus->p_parked_tail == NULL) {
        p_parkhaus->p_parked_head = p_vehicle;
        p_parkhaus->p_parked_tail = p_vehicle;
        return OK;
    }

    p_parkhaus->p_parked_tail->p_next = p_vehicle;
    p_parkhaus->p_parked_tail = p_vehicle;
    return OK;
}

uint16_t get_open_space(const Parkhaus *p_parkhouse) {
    if (p_parkhouse == NULL || p_parkhouse->capacity_taken >= p_parkhouse->capacity) {
        return 0;
    }

    return (uint16_t) (p_parkhouse->capacity - p_parkhouse->capacity_taken);
}

void update_on_vehicle_exit(Parkhaus *p_parkhouse, StatList *p_StatList, const GenericVehicle *p_vehicle, uint16_t required_space, uint32_t current_tick) {
    (void) p_StatList;
    (void) p_vehicle;
    (void) current_tick;
    if (p_parkhouse == NULL) {
        return;
    }

    if (p_parkhouse->capacity_taken >= required_space) {
        p_parkhouse->capacity_taken -= required_space;
    } else {
        p_parkhouse->capacity_taken = 0;
    }
}

void update_on_vehicle_entry(Parkhaus *p_parkhouse, StatList *p_StatList, const GenericVehicle *p_vehicle, uint16_t required_space, uint32_t current_tick) {
    (void) p_StatList;
    (void) p_vehicle;
    (void) current_tick;
    if (p_parkhouse == NULL) {
        return;
    }

    p_parkhouse->capacity_taken += required_space;
}
