#include "Queue.h"

#include <stdlib.h>
#include "utils/SafteyUtils.h"


int queue_init(Queue *p_self, uint16_t max_size) {
    if (p_self == NULL ) {print_warning("queue_init(): no pointer"); return ERROR;}

    p_self->capacity = 0;
    p_self->p_head = NULL;
    p_self->p_tail = NULL;
    p_self->demand = 0;
    p_self->max_size = max_size;

    return OK;
}

int queue_is_full(const Queue *p_queue) {
    if (p_queue == NULL) {
        return 0;
    }

    return p_queue->capacity >= p_queue->max_size;
}

int queue_is_empty(const Queue *p_queue) {
    if (p_queue == NULL) {
        return 1;
    }

    return p_queue->capacity == 0;
}

uint16_t queue_length(const Queue *p_queue) {
    if (p_queue == NULL) {
        return 0;
    }

    return p_queue->capacity;
}

int queue_enqueue(Queue *p_queue, GenericVehicle *p_vehicle) {
    if (p_queue == NULL || p_vehicle == NULL) {
        return ERROR;
    }
    if (queue_is_full(p_queue)) {
        return ERROR;
    }

    p_vehicle->p_next = NULL;

    if (p_queue->p_tail == NULL) {
        p_queue->p_head = p_vehicle;
        p_queue->p_tail = p_vehicle;
        p_queue->capacity = 1;
        return OK;
    }

    p_queue->p_tail->p_next = p_vehicle;
    p_queue->p_tail = p_vehicle;
    p_queue->capacity++;
    return OK;
}

int queue_dequeue(Queue *p_queue) {
    GenericVehicle *p_first;

    if (p_queue == NULL || queue_is_empty(p_queue)) {
        return ERROR;
    }

    p_first = p_queue->p_head;
    p_queue->p_head = p_first->p_next;
    p_first->p_next = NULL;

    if (p_queue->p_head == NULL) {
        p_queue->p_tail = NULL;
    }

    if (p_queue->capacity > 0) {
        p_queue->capacity--;
    }

    return OK;
}

//Done
GenericVehicle *queue_get_next(const Queue *p_queue) {
    if (p_queue == NULL || queue_is_empty(p_queue)) {
        return NULL;
    }

    return p_queue->p_head;
}

int queue_remove(Queue *p_queue, GenericVehicle *p_target) {
    GenericVehicle *p_prev = NULL;
    GenericVehicle *p_cur;

    if (p_queue == NULL || p_target == NULL) {
        return ERROR;
    }

    p_cur = p_queue->p_head;
    while (p_cur != NULL) {
        if (p_cur == p_target) {
            if (p_prev == NULL) {
                p_queue->p_head = p_cur->p_next;
            } else {
                p_prev->p_next = p_cur->p_next;
            }

            if (p_queue->p_tail == p_cur) {
                p_queue->p_tail = p_prev;
            }

            if (p_queue->capacity > 0) {
                p_queue->capacity--;
            }

            free(p_cur);
            return OK;
        }

        p_prev = p_cur;
        p_cur = p_cur->p_next;
    }

    return ERROR;
}

void queue_set_demand(Queue *p_queue, uint16_t demand_value) {
    if (p_queue == NULL) {
        return;
    }

    p_queue->demand = demand_value;
}

uint16_t queue_get_demand(const Queue *p_queue) {
    if (p_queue == NULL) {
        return 0;
    }

    return p_queue->demand;
}

void queue_tick(SimulationObject *p_self, uint32_t current_tick) {
    (void) p_self;
    (void) current_tick;
}

void queue_free(Queue *p_queue) {
    GenericVehicle *p_vehicle;
    GenericVehicle *p_next;

    if (p_queue == NULL) {
        return;
    }

    p_vehicle = p_queue->p_head;
    while (p_vehicle != NULL) {
        p_next = p_vehicle->p_next;
        free(p_vehicle);
        p_vehicle = p_next;
    }

    p_queue->p_head = NULL;
    p_queue->p_tail = NULL;
    p_queue->capacity = 0;
    p_queue->demand = 0;
}
