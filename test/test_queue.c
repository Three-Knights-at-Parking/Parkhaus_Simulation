#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Queue.h"
#include "Car.h"
#include "types.h"

static Car *make_test_car(uint32_t created_at, uint32_t parking_time, uint8_t spaces_needed)
{
    Car *p_car = car_create(created_at, parking_time, spaces_needed);
    assert(p_car != NULL);
    return p_car;
}

static void test_queue_init_and_basic_state(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));

    assert(queue_init(NULL, 3) == ERROR);

    assert(queue_init(&queue, 0) == OK);
    assert(queue_is_empty(&queue) == 1);
    assert(queue_is_full(&queue) == 1);
    assert(queue_length(&queue) == 0);
    assert(queue_get_next(&queue) == NULL);

    assert(queue_init(&queue, 3) == OK);
    assert(queue_is_empty(&queue) == 1);
    assert(queue_is_full(&queue) == 0);
    assert(queue_length(&queue) == 0);
    assert(queue_get_next(&queue) == NULL);

    assert(queue_is_empty(NULL) == 1);
    assert(queue_is_full(NULL) == 0);
    assert(queue_length(NULL) == 0);
    assert(queue_get_next(NULL) == NULL);

    queue_free(&queue);
}

static void test_queue_enqueue_and_peek(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));
    assert(queue_init(&queue, 3) == OK);

    Car *c1 = make_test_car(1, 5, Car_Space);
    Car *c2 = make_test_car(2, 6, Car_Space);

    assert(queue_enqueue(NULL, &c1->base) == ERROR);
    assert(queue_enqueue(&queue, NULL) == ERROR);

    assert(queue_enqueue(&queue, &c1->base) == OK);
    assert(queue_is_empty(&queue) == 0);
    assert(queue_length(&queue) == 1);
    assert(queue_get_next(&queue) == &c1->base);

    assert(queue_enqueue(&queue, &c2->base) == OK);
    assert(queue_length(&queue) == 2);
    assert(queue_get_next(&queue) == &c1->base);

    queue_free(&queue);
}

static void test_queue_fifo_dequeue(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));
    assert(queue_init(&queue, 3) == OK);

    Car *c1 = make_test_car(1, 5, Car_Space);
    Car *c2 = make_test_car(2, 6, Car_Space);
    Car *c3 = make_test_car(3, 7, Car_Space);

    assert(queue_enqueue(&queue, &c1->base) == OK);
    assert(queue_enqueue(&queue, &c2->base) == OK);
    assert(queue_enqueue(&queue, &c3->base) == OK);

    assert(queue_is_full(&queue) == 1);
    assert(queue_length(&queue) == 3);

    assert(queue_dequeue(NULL) == ERROR);

    assert(queue_dequeue(&queue) == OK);
    assert(queue_length(&queue) == 2);
    assert(queue_get_next(&queue) == &c2->base);
    car_destroy(c1);

    assert(queue_dequeue(&queue) == OK);
    assert(queue_length(&queue) == 1);
    assert(queue_get_next(&queue) == &c3->base);
    car_destroy(c2);

    assert(queue_dequeue(&queue) == OK);
    assert(queue_length(&queue) == 0);
    assert(queue_is_empty(&queue) == 1);
    assert(queue_get_next(&queue) == NULL);
    car_destroy(c3);

    assert(queue_dequeue(&queue) == ERROR);

    queue_free(&queue);
}

static void test_queue_full_behavior(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));
    assert(queue_init(&queue, 2) == OK);

    Car *c1 = make_test_car(1, 5, Car_Space);
    Car *c2 = make_test_car(2, 6, Car_Space);
    Car *c3 = make_test_car(3, 7, Car_Space);

    assert(queue_enqueue(&queue, &c1->base) == OK);
    assert(queue_is_full(&queue) == 0);

    assert(queue_enqueue(&queue, &c2->base) == OK);
    assert(queue_is_full(&queue) == 1);
    assert(queue_length(&queue) == 2);

    assert(queue_enqueue(&queue, &c3->base) == ERROR);
    assert(queue_length(&queue) == 2);

    car_destroy(c3);
    queue_free(&queue);
}

static void test_queue_remove_cases(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));
    assert(queue_init(&queue, 5) == OK);

    Car *c1 = make_test_car(1, 5, Car_Space);
    Car *c2 = make_test_car(2, 6, Car_Space);
    Car *c3 = make_test_car(3, 7, Car_Space);

    assert(queue_remove(NULL, &c1->base) == ERROR);
    assert(queue_remove(&queue, NULL) == ERROR);
    assert(queue_remove(&queue, &c1->base) == ERROR);

    assert(queue_enqueue(&queue, &c1->base) == OK);
    assert(queue_enqueue(&queue, &c2->base) == OK);
    assert(queue_enqueue(&queue, &c3->base) == OK);
    assert(queue_length(&queue) == 3);

    assert(queue_remove(&queue, &c1->base) == OK);
    assert(queue_length(&queue) == 2);
    assert(queue_get_next(&queue) == &c2->base);

    assert(queue_remove(&queue, &c3->base) == OK);
    assert(queue_length(&queue) == 1);
    assert(queue_get_next(&queue) == &c2->base);

    assert(queue_remove(&queue, &c2->base) == OK);
    assert(queue_length(&queue) == 0);
    assert(queue_is_empty(&queue) == 1);
    assert(queue_get_next(&queue) == NULL);

    queue_free(&queue);
}

static void test_queue_demand_accessors(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));
    assert(queue_init(&queue, 4) == OK);

    assert(queue_get_demand(NULL) == 0);

    queue_set_demand(&queue, 7);
    assert(queue_get_demand(&queue) == 7);

    queue_set_demand(&queue, 0);
    assert(queue_get_demand(&queue) == 0);

    queue_set_demand(&queue, 123);
    assert(queue_get_demand(&queue) == 123);

    queue_free(&queue);
}

static void test_queue_free_on_empty_and_filled_queue(void)
{
    Queue queue;
    memset(&queue, 0, sizeof(queue));
    assert(queue_init(&queue, 3) == OK);

    queue_free(NULL);
    queue_free(&queue);

    assert(queue_init(&queue, 3) == OK);

    Car *c1 = make_test_car(1, 5, Car_Space);
    Car *c2 = make_test_car(2, 6, Car_Space);

    assert(queue_enqueue(&queue, &c1->base) == OK);
    assert(queue_enqueue(&queue, &c2->base) == OK);
    assert(queue_length(&queue) == 2);

    queue_free(&queue);

    assert(queue_length(&queue) == 0);
    assert(queue_get_next(&queue) == NULL);
    assert(queue_get_demand(&queue) == 0);
}

void test_queue(void)
{
    test_queue_init_and_basic_state();
    printf("Queue - init/basic state passed\n");

    test_queue_enqueue_and_peek();
    printf("Queue - enqueue/peek passed\n");

    test_queue_fifo_dequeue();
    printf("Queue - FIFO dequeue passed\n");

    test_queue_full_behavior();
    printf("Queue - full behavior passed\n");

    test_queue_remove_cases();
    printf("Queue - remove cases passed\n");

    test_queue_demand_accessors();
    printf("Queue - demand accessors passed\n");

    test_queue_free_on_empty_and_filled_queue();
    printf("Queue - free passed\n");

    printf("All Queue tests passed\n");
}