#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils/demand.h"
#include "utils/RNG.h"
#include "types.h"

static Settings make_settings(int32_t seed, float entry_probability_perSec_prec, uint16_t real_equivalent)
{
    Settings s;
    memset(&s, 0, sizeof(s));

    s.rand_seed = seed;
    s.entry_probability_perSec_prec = entry_probability_perSec_prec;
    s.real_equivalent = real_equivalent;

    return s;
}

static void test_demand_null_and_zero_lambda(void)
{
    Settings s_zero = make_settings(1, 0.0f, 60);
    Settings s_negative = make_settings(1, -5.0f, 60);

    assert(demand_generate_total_perTick(NULL) == ERROR);
    assert(rng_init(&s_zero) == OK);
    assert(demand_generate_total_perTick(&s_zero) == 0);

    assert(rng_init(&s_negative) == OK);
    assert(demand_generate_total_perTick(&s_negative) == 0);
}

static void test_demand_low_lambda_is_non_negative(void)
{
    Settings s = make_settings(42, 5.0f, 60);
    assert(rng_init(&s) == OK);

    for (int i = 0; i < 200; ++i)
    {
        int d = demand_generate_total_perTick(&s);
        assert(d >= 0);
        assert(d < 30);
    }
}

static void test_demand_is_deterministic_for_fixed_seed(void)
{
    Settings s1 = make_settings(12345, 7.5f, 60);
    Settings s2 = make_settings(12345, 7.5f, 60);

    assert(rng_init(&s1) == OK);
    int first = demand_generate_total_perTick(&s1);

    assert(rng_init(&s2) == OK);
    int second = demand_generate_total_perTick(&s2);

    assert(first == second);
}

static void test_demand_high_lambda_non_negative(void)
{
    Settings s = make_settings(99, 80.0f, 60);
    assert(rng_init(&s) == OK);

    for (int i = 0; i < 200; ++i)
    {
        int d = demand_generate_total_perTick(&s);
        assert(d >= 0);
    }
}

void test_demand(void)
{
    test_demand_null_and_zero_lambda();
    printf("Demand - null/zero lambda passed\n");

    test_demand_low_lambda_is_non_negative();
    printf("Demand - low lambda passed\n");

    test_demand_is_deterministic_for_fixed_seed();
    printf("Demand - deterministic seed passed\n");

    test_demand_high_lambda_non_negative();
    printf("Demand - high lambda passed\n");

    printf("All demand tests passed\n");
}