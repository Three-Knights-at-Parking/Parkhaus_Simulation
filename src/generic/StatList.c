#include "utils/StatList.h"
#include "utils/SafteyUtils.h"

#include <stdlib.h>
#include <string.h>

int statlist_append(const Simulation *p_sim, StatsTick *p_tick) {
    if (p_sim == NULL || p_sim->StatList == NULL || p_tick == NULL) {
        print_error_s("Invalid pointers passed to statlist_append", HIGH);
        return ERROR;
    }

    /**
     * We're in a doubly linked list here, so it requires additional
     * setup.
     */
    StatList *p_list = p_sim->StatList;
    p_tick->p_next = NULL;
    p_tick->p_prev = p_list->p_tick_tail;
    if (p_list->p_tick_tail != NULL) {
        p_list->p_tick_tail->p_next = p_tick;
    } else {
        // this is the first and only element
        p_list->p_tick_head = p_tick;
    }
    p_list->p_tick_tail = p_tick;

    return OK;
}

void statlist_clear(Simulation *p_sim) {
    if (p_sim == NULL || p_sim->StatList == NULL) {
        return;
    }

    StatList *p_list = p_sim->StatList;
    StatsTick *p_tick = p_list->p_tick_head;
    StatsTick *p_next = NULL;

    while (p_tick != NULL) {
        p_next = p_tick->p_next;
        free(p_tick);
        p_tick = p_next;
    }
    p_list->p_tick_head = NULL;
    p_list->p_tick_tail = NULL;
}
