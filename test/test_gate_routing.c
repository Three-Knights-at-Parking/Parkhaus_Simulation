#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/gate_routing.h"
#include "Queue.h"
#include "Stats.h"
#include "utils/RNG.h"
#include "types.h"

static void init_queue(Queue *q)
{
    memset(q, 0, sizeof(*q));
    assert(queue_init(q, 10) == OK);
}

static void init_sim_with_stats(Simulation *p_sim, StatList *p_list, StatsSummary *p_summary)
{
    memset(p_sim, 0, sizeof(*p_sim));
    memset(p_list, 0, sizeof(*p_list));
    memset(p_summary, 0, sizeof(*p_summary));

    p_list->p_summary = p_summary;
    p_sim->StatList = p_list;
}

static void test_gate_routing_invalid_inputs(void)
{
    Simulation sim;
    StatList list;
    StatsSummary summary;
    Settings s;
    Queue q;
    Queue *arr[1] = {&q};

    init_sim_with_stats(&sim, &list, &summary);
    memset(&s, 0, sizeof(s));
    init_queue(&q);

    assert(StatsTick_init(&sim, 10, 0) == OK);

    assert(GateRouting_DistributeTotalDemand(NULL, &s, 10, arr, 0) == ERROR);
    assert(GateRouting_DistributeTotalDemand(&sim, NULL, 10, arr, 0) == ERROR);
    assert(GateRouting_DistributeTotalDemand(&sim, &s, 10, NULL, 0) == ERROR);

    s.gates = 0;
    assert(GateRouting_DistributeTotalDemand(&sim, &s, 10, arr, 0) == ERROR);

    StatsTick_free(&list);
}

static void test_gate_routing_demand_distribution_sum(void)
{
    Simulation sim;
    StatList list;
    StatsSummary summary;
    Settings s;
    Queue q1, q2, q3;
    Queue *arr[3] = {&q1, &q2, &q3};

    init_sim_with_stats(&sim, &list, &summary);
    memset(&s, 0, sizeof(s));
    s.gates = 3;
    s.rand_seed = 4;

    assert(StatsTick_init(&sim, 30, 1) == OK);
    assert(rng_init(&s) == OK);

    init_queue(&q1);
    init_queue(&q2);
    init_queue(&q3);

    assert(GateRouting_DistributeTotalDemand(&sim, &s, 10, arr, 1) == OK);

    uint16_t d1 = queue_get_demand(&q1);
    uint16_t d2 = queue_get_demand(&q2);
    uint16_t d3 = queue_get_demand(&q3);

    assert((uint16_t)(d1 + d2 + d3) == 10);
    assert(list.p_current_tick != NULL);
    assert(list.p_current_tick->arrivals_generated == 10);

    assert(GateRouting_DistributeTotalDemand(&sim, &s, 0, arr, 1) == OK);
    assert((uint16_t)(queue_get_demand(&q1) + queue_get_demand(&q2) + queue_get_demand(&q3)) == 0);
    assert(list.p_current_tick->arrivals_generated == 10);

    StatsTick_free(&list);
}

static void test_gate_routing_missing_single_queue(void)
{
    Simulation sim;
    StatList list;
    StatsSummary summary;
    Settings s;
    Queue q1;
    Queue *arr[2] = {&q1, NULL};

    init_sim_with_stats(&sim, &list, &summary);
    memset(&s, 0, sizeof(s));
    s.gates = 2;

    init_queue(&q1);
    assert(StatsTick_init(&sim, 20, 0) == OK);

    assert(GateRouting_DistributeTotalDemand(&sim, &s, 5, arr, 0) == ERROR);

    StatsTick_free(&list);
}

static void test_gate_routing_remainder_can_reach_all_gates(void)
{
    const uint8_t gates = 4;
    int seen[4] = {0, 0, 0, 0};

    for (int seed = 1; seed <= 200; ++seed)
    {
        Simulation sim;
        StatList list;
        StatsSummary summary;
        Settings s;
        Queue q0, q1, q2, q3;
        Queue *arr[4] = {&q0, &q1, &q2, &q3};

        init_sim_with_stats(&sim, &list, &summary);
        memset(&s, 0, sizeof(s));
        s.gates = gates;
        s.rand_seed = seed;

        init_queue(&q0);
        init_queue(&q1);
        init_queue(&q2);
        init_queue(&q3);

        assert(StatsTick_init(&sim, 40, 0) == OK);
        assert(rng_init(&s) == OK);
        assert(GateRouting_DistributeTotalDemand(&sim, &s, (uint16_t)(gates + 1), arr, 0) == OK);

        for (int i = 0; i < gates; ++i)
        {
            uint16_t di = queue_get_demand(arr[i]);
            assert(di == 1 || di == 2);
            if (di == 2)
            {
                seen[i] = 1;
            }
        }

        StatsTick_free(&list);
    }

    for (int i = 0; i < gates; ++i)
    {
        assert(seen[i] == 1);
    }
}

void test_gate_routing(void)
{
    test_gate_routing_invalid_inputs();
    test_gate_routing_demand_distribution_sum();
    test_gate_routing_missing_single_queue();
    test_gate_routing_remainder_can_reach_all_gates();
    printf("Gate routing tests passed\n");
}