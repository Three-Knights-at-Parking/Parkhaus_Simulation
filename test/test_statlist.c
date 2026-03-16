#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/StatList.h"
#include "Stats.h"
#include "types.h"

static void setup_sim(Simulation *sim, StatList *list, StatsSummary *summary) {
    memset(sim, 0, sizeof(*sim));
    memset(list, 0, sizeof(*list));
    memset(summary, 0, sizeof(*summary));
    list->p_summary = summary;
    sim->StatList = list;
}

static void test_statlist_append_and_clear(void) {
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_sim(&sim, &list, &summary);

    StatsTick *a = calloc(1, sizeof(StatsTick));
    StatsTick *b = calloc(1, sizeof(StatsTick));
    assert(a && b);

    assert(statlist_append(NULL, a) == ERROR);
    assert(statlist_append(&sim, NULL) == ERROR);

    assert(statlist_append(&sim, a) == OK);
    assert(list.p_tick_head == a && list.p_tick_tail == a);

    assert(statlist_append(&sim, b) == OK);
    assert(list.p_tick_head == a && list.p_tick_tail == b);
    assert(a->p_next == b);
    assert(b->p_prev == a);

    statlist_clear(NULL);
    statlist_clear(&sim);
    assert(list.p_tick_head == NULL && list.p_tick_tail == NULL);

    /* red-team: clear should be idempotent and keep list empty */
    statlist_clear(&sim);
    assert(list.p_tick_head == NULL && list.p_tick_tail == NULL);
}

static void test_statlist_compute_summary(void) {
    Simulation sim;
    StatList list;
    StatsSummary summary;
    setup_sim(&sim, &list, &summary);

    assert(statlist_compute_summary(NULL, &summary) == ERROR);
    assert(statlist_compute_summary(&sim, NULL) == ERROR);
    assert(statlist_compute_summary(&sim, &summary) == ERROR);

    StatsTick *t1 = calloc(1, sizeof(StatsTick));
    assert(t1 != NULL);
    t1->current_tick = 1;
    t1->capacity_total = 20;
    t1->capacity_taken = 10;
    t1->capacity_free = 10;
    t1->entered = 3;
    list.p_tick_head = t1;
    list.p_tick_tail = t1;

    assert(statlist_compute_summary(&sim, &summary) == OK);
    assert(summary.total_ticks == 1);
    assert(summary.entered_total == 3);

    statlist_clear(&sim);
    assert(statlist_compute_summary(&sim, &summary) == ERROR);
}

void test_statlist(void) {
    test_statlist_append_and_clear();
    test_statlist_compute_summary();
    printf("StatList tests passed\n");
}
