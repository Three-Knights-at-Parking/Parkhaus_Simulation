#include "utils/Queue_List.h"

#include "utils/SafteyUtils.h"


int queue_list_init(Queue_List *p_self, Simulation *p_sim, uint32_t anzGates)
{
    p_self = calloc(anzGates, sizeof(Queue));
    if (checkNull(p_self)) {print_warning("Queue_List_init(): Out of memory");}


    p_self->base = p_sim->base;
    p_self->p_head = NULL;
    p_self->p_tail = NULL;

    for (int i = 0; i < anzGates; i++)
    {
        Queue p_queue;
        queue_init(p_queue, p_sim, NULL);
        if (p_queue == NULL) {print_warning("Queue_init(): Out of memory"); return ERROR;}

        if (queue_list_push(p_self, p_queue) == ERROR) {print_warning("queue_list_push(): pointer is NULL"); return ERROR;}
    }

    return OK;
}

void queue_list_free(Queue_List *p_list) {
    if (p_list == NULL) {print_warning("queue_list_push(): pointer is NULL");}

}

int queue_list_push(Queue_List *p_list, Queue *p_queue) {
    if (p_list == NULL || p_queue == NULL) {print_warning("queue_list_push(): pointer is NULL"); return ERROR;}

    if (p_list->p_head == NULL)
    {
        p_list->p_head = p_queue;
    }
    if (p_list->p_tail == NULL)
    {
        p_list->p_tail = p_queue;
    }
    else
    {
        p_queue->p_prev = p_list->p_tail;
        p_list->p_tail = p_queue;
    }

    return OK;
}

