#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/gate_routing.h"
#include "Queue.h"
#include "utils/RNG.h"
#include "types.h"

static void init_queue(Queue *q) {
    memset(q, 0, sizeof(*q));
    assert(queue_init(q, 10) == OK);
}

static void test_gate_routing_invalid_inputs(void) {
    Settings s;
    Queue q;
    Queue *arr[1] = {&q};
    memset(&s, 0, sizeof(s));
    init_queue(&q);

    assert(GateRouting_DistributeTotalDemand(NULL, 10, arr, 0) == ERROR);
    assert(GateRouting_DistributeTotalDemand(&s, 10, NULL, 0) == ERROR);

    s.gates = 0;
    assert(GateRouting_DistributeTotalDemand(&s, 10, arr, 0) == ERROR);
}

static void test_gate_routing_demand_distribution_sum(void) {
    Settings s;
    Queue q1, q2, q3;
    Queue *arr[3] = {&q1, &q2, &q3};
    memset(&s, 0, sizeof(s));
    s.gates = 3;
    s.rand_seed = 4;
    assert(rng_init(&s) == OK);

    init_queue(&q1); init_queue(&q2); init_queue(&q3);

    assert(GateRouting_DistributeTotalDemand(&s, 10, arr, 1) == OK);
    uint16_t d1 = queue_get_demand(&q1);
    uint16_t d2 = queue_get_demand(&q2);
    uint16_t d3 = queue_get_demand(&q3);
    assert((uint16_t)(d1 + d2 + d3) == 10);

    assert(GateRouting_DistributeTotalDemand(&s, 0, arr, 1) == OK);
    assert(queue_get_demand(&q1) + queue_get_demand(&q2) + queue_get_demand(&q3) == 0);
}

static void test_gate_routing_missing_single_queue(void) {
    Settings s;
    Queue q1;
    Queue *arr[2] = {&q1, NULL};
    memset(&s, 0, sizeof(s));
    s.gates = 2;
    init_queue(&q1);

    assert(GateRouting_DistributeTotalDemand(&s, 5, arr, 0) == ERROR);
}

/*
 * Red-team regression: with total_demand = gates + 1, exactly one gate gets one extra.
 * Over many deterministic seeds this "extra" must be able to hit every gate index.
 */
static void test_gate_routing_remainder_can_reach_all_gates(void) {
    const uint8_t gates = 4;
    int seen[gates];
    memset(seen, 0, sizeof(seen));

    for (int seed = 1; seed <= 200; ++seed) {
        Settings s;
        Queue q0, q1, q2, q3;
        Queue *arr[4] = {&q0, &q1, &q2, &q3};
        memset(&s, 0, sizeof(s));
        s.gates = gates;
        s.rand_seed = seed;

        init_queue(&q0); init_queue(&q1); init_queue(&q2); init_queue(&q3);
        assert(rng_init(&s) == OK);
        assert(GateRouting_DistributeTotalDemand(&s, (uint16_t)(gates + 1), arr, 0) == OK);

        for (int i = 0; i < gates; ++i) {
            uint16_t di = queue_get_demand(arr[i]);
            assert(di == 1 || di == 2);
            if (di == 2) {
                seen[i] = 1;
            }
        }
    }

    for (int i = 0; i < gates; ++i) {
        assert(seen[i] == 1);
    }
}

void test_gate_routing(void) {
    test_gate_routing_invalid_inputs();
    test_gate_routing_demand_distribution_sum();
    test_gate_routing_missing_single_queue();
    test_gate_routing_remainder_can_reach_all_gates();
    printf("Gate routing tests passed\n");
}
