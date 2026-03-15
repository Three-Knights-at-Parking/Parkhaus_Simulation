#include "utils/StatList.h"
#include "utils/SafteyUtils.h"
#include "Stats.h"

#include <stdlib.h>
#include <string.h>

int statlist_append(const Simulation *p_sim, StatsTick *p_tick) {
    if (checkNull(p_sim) || checkNull(p_sim->StatList) || checkNull(p_tick)) {
        print_error_s("Invalid pointers passed.", HIGH);
        return ERROR;
    }

    /**
     * We're in a doubly linked list here, so it requires additional
     * setup.
     */
    StatList *p_list = p_sim->StatList;
    p_tick->p_next = NULL;
    p_tick->p_prev = p_list->p_tick_tail;

    if (p_list->p_tick_tail != NULL)
    {
        p_list->p_tick_tail->p_next = p_tick;
    }
    if (p_list->p_tick_head == NULL)
    {
        // this is the first and only element
        p_list->p_tick_head = p_tick;
    }
    p_list->p_tick_tail = p_tick;

    return OK;
}

void statlist_clear(Simulation *p_sim) {
    if (checkNull(p_sim)|| checkNull(p_sim->StatList)) {
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


int statlist_compute_summary(const Simulation *p_sim, StatsSummary *p_summary)
{
    if (checkNull(p_sim) || checkNull(p_sim->StatList) || checkNull(p_summary))
    {
        return ERROR;
    }

    if (checkNull(p_sim->StatList->p_tick_head)) {
        print_warning_s("No stats available to compute.");
        return ERROR;
    }

    return stats_build_summary(p_sim->StatList, p_summary);
}
