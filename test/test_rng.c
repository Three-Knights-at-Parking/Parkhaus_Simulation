#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils/RNG.h"
#include "types.h"

static Settings make_settings_with_seed(int32_t seed)
{
    Settings s;
    memset(&s, 0, sizeof(s));
    s.rand_seed = seed;
    return s;
}

static void test_rng_init(void)
{
    Settings s1 = make_settings_with_seed(12345);
    Settings s2 = make_settings_with_seed(12345);
    Settings s3 = make_settings_with_seed(0);
    Settings s4 = make_settings_with_seed(-1);

    assert(rng_init(NULL) == ERROR);

    assert(rng_init(&s1) == OK);
    uint32_t a1 = rng_next_u32();

    assert(rng_init(&s2) == OK);
    uint32_t a2 = rng_next_u32();

    assert(a1 == a2);

    assert(rng_init(&s3) == OK);
    uint32_t zero_seed_value = rng_next_u32();
    assert(zero_seed_value != 0U);

    assert(rng_init(&s4) == OK);
    assert(s4.rand_seed != -1);
}

static void test_rng_next_u32_determinism(void)
{
    Settings s1 = make_settings_with_seed(777);
    Settings s2 = make_settings_with_seed(777);

    assert(rng_init(&s1) == OK);
    uint32_t seq1[5];
    for (int i = 0; i < 5; ++i)
    {
        seq1[i] = rng_next_u32();
    }

    assert(rng_init(&s2) == OK);
    for (int i = 0; i < 5; ++i)
    {
        assert(rng_next_u32() == seq1[i]);
    }
}

static void test_rng_range_int(void)
{
    Settings s = make_settings_with_seed(42);
    assert(rng_init(&s) == OK);

    for (int i = 0; i < 100; ++i)
    {
        int32_t v = rng_range_int(5, 10);
        assert(v >= 5);
        assert(v <= 10);
    }

    for (int i = 0; i < 100; ++i)
    {
        int32_t v = rng_range_int(10, 5);
        assert(v >= 5);
        assert(v <= 10);
    }

    for (int i = 0; i < 20; ++i)
    {
        assert(rng_range_int(7, 7) == 7);
    }

    for (int i = 0; i < 100; ++i)
    {
        int32_t v = rng_range_int(-3, 3);
        assert(v >= -3);
        assert(v <= 3);
    }
}

static void test_rng_percent(void)
{
    Settings s = make_settings_with_seed(99);
    assert(rng_init(&s) == OK);

    for (int i = 0; i < 500; ++i)
    {
        uint8_t v = rng_percent();
        assert(v <= 100);
    }
}

static void test_rng_parking_time(void)
{
    Settings s = make_settings_with_seed(2024);
    assert(rng_init(&s) == OK);

    for (int i = 0; i < 200; ++i)
    {
        uint32_t v = rng_parking_time(10, 20);
        assert(v >= 10U);
        assert(v <= 20U);
    }

    for (int i = 0; i < 20; ++i)
    {
        assert(rng_parking_time(15, 15) == 15U);
    }

    for (int i = 0; i < 100; ++i)
    {
        uint32_t v = rng_parking_time(1, 2);
        assert(v >= 1U);
        assert(v <= 2U);
    }
}

static void test_rng_gate_index(void)
{
    Settings s = make_settings_with_seed(31415);
    assert(rng_init(&s) == OK);

    assert(rng_gate_index(0) == 0U);

    for (int i = 0; i < 200; ++i)
    {
        uint32_t v = rng_gate_index(1);
        assert(v == 0U);
    }

    for (int i = 0; i < 500; ++i)
    {
        uint32_t v = rng_gate_index(4);
        assert(v < 4U);
    }
}

static void test_rng_poisson(void)
{
    Settings s = make_settings_with_seed(555);
    assert(rng_init(&s) == OK);

    for (int i = 0; i < 20; ++i)
    {
        assert(rng_poisson(0.0) == 0U);
    }

    for (int i = 0; i < 200; ++i)
    {
        uint32_t v = rng_poisson(2.5);
        assert(v >= 0U);
    }

    Settings s1 = make_settings_with_seed(888);
    Settings s2 = make_settings_with_seed(888);

    assert(rng_init(&s1) == OK);
    uint32_t p1 = rng_poisson(3.0);

    assert(rng_init(&s2) == OK);
    uint32_t p2 = rng_poisson(3.0);

    assert(p1 == p2);
}

void test_rng(void)
{
    test_rng_init();
    printf("RNG - init passed\n");

    test_rng_next_u32_determinism();
    printf("RNG - next_u32 determinism passed\n");

    test_rng_range_int();
    printf("RNG - range_int passed\n");

    test_rng_percent();
    printf("RNG - percent passed\n");

    test_rng_parking_time();
    printf("RNG - parking_time passed\n");

    test_rng_gate_index();
    printf("RNG - gate_index passed\n");

    test_rng_poisson();
    printf("RNG - poisson passed\n");

    printf("All RNG tests passed\n");
}