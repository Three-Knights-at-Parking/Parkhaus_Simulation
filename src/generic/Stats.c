#include <string.h>
#include <stdlib.h>

#include "Stats.h"
#include "Queue.h"
#include "types.h"
#include "utils/SafteyUtils.h"
#include "utils/StatList.h"

int StatsTick_init(Simulation *p_sim, uint16_t capacity_total, uint32_t current_tick) {
    StatsTick *p_tick = NULL;

    if (p_sim == NULL || p_sim->StatList == NULL) {
        print_error_s("Invalid pointers passed.", HIGH);
        return ERROR;
    }

    p_tick = (StatsTick *)calloc(1U, sizeof(StatsTick));
    if (p_tick == NULL) {
        print_error_s("Out of memory while creating StatsTick.", HIGH);
        return ERROR;
    }

    p_tick->current_tick = current_tick;
    p_tick->capacity_total = capacity_total;
    p_tick->capacity_taken = 0U;
    p_tick->capacity_free = 0U;

    if (statlist_append(p_sim, p_tick) != OK) {
        free(p_tick);
        return ERROR;
    }

    p_sim->StatList->p_current_tick = p_tick;
    return OK;
}

StatList * StatList_init(Simulation *p_simulation) {
    StatList *p_stats = NULL;

    if (checkNull(p_simulation)) {
        print_error_s("Simulation pointer must not be NULL.", HIGH);
        return NULL;
    }

    p_stats = (StatList *)calloc(1, sizeof(StatList));
    if (p_stats == NULL) {
        print_error_s("Out of memory while creating StatList.", HIGH);
        free(p_stats);
        return NULL;
    }

    p_stats->p_summary = (StatsSummary *)calloc(1U, sizeof(StatsSummary));
    if (p_stats->p_summary == NULL) {
        print_error_s("Out of memory while creating StatsSummary.", HIGH);
        free(p_stats);
        return NULL;
    }

    p_simulation->StatList = p_stats;
    return p_stats;
}

int StatList_free(StatList *p_stats) {
    if (checkNull(p_stats)) {
        return ERROR;
    }

    StatsTick_free(p_stats);

    if (p_stats->p_summary != NULL) {
        free(p_stats->p_summary);
        p_stats->p_summary = NULL;
    }

    free(p_stats);
    return OK;
}

int StatsTick_free(StatList *p_stats) {
    StatsTick *p_tick = NULL;

    if (checkNull(p_stats)) {
        return ERROR;
    }

    p_tick = p_stats->p_tick_head;
    while (p_tick != NULL) {
        StatsTick *p_next = p_tick->p_next;
        free(p_tick);
        p_tick = p_next;
    }

    p_stats->p_tick_head = NULL;
    p_stats->p_tick_tail = NULL;
    p_stats->p_current_tick = NULL;

    return OK;
}

int stats_tick_set_capacity(StatList *p_stats, uint16_t taken, uint16_t free) {
    if (checkNull(p_stats) || checkNull(p_stats->p_current_tick)) {
        return ERROR;
    }

    p_stats->p_current_tick->capacity_taken = taken;
    p_stats->p_current_tick->capacity_free = free;
    p_stats->p_current_tick->capacity_total = (uint16_t)(taken + free);

    return OK;
}

int stats_tick_set_queue_length_end(StatList *p_stats, Queue * const *pp_gate_queues, uint32_t gates) {
    uint64_t queue_length_end = 0U;

    if (checkNull(p_stats) || checkNull(p_stats->p_current_tick) || checkNull(pp_gate_queues)) {
        return ERROR;
    }

    for (uint32_t gate = 0U; gate < gates; ++gate) {
        if (pp_gate_queues[gate] != NULL) {
            queue_length_end += queue_length(pp_gate_queues[gate]);
        }
    }

    if (queue_length_end > UINT16_MAX) {
        p_stats->p_current_tick->queue_length_end = UINT16_MAX;
    } else {
        p_stats->p_current_tick->queue_length_end = (uint16_t)queue_length_end;
    }

    return OK;
}

int stats_tick_add_queue_rejections(StatList *p_stats, uint16_t amount) {
    if (checkNull(p_stats) || checkNull(p_stats->p_current_tick)) {
        return ERROR;
    }

    p_stats->p_current_tick->queue_rejections += amount;
    return OK;
}

int stats_tick_add_arrivals_generated(StatList *p_stats, uint16_t amount) {
    if (checkNull(p_stats) || checkNull(p_stats->p_current_tick)) {
        return ERROR;
    }

    p_stats->p_current_tick->arrivals_generated += amount;
    return OK;
}

int stats_tick_add_blocker_full_active(StatList *p_stats) {
    if (checkNull(p_stats) || checkNull(p_stats->p_current_tick)) {
        return ERROR;
    }

    p_stats->p_current_tick->blocker_full_active += 1U;
    return OK;
}

int stats_tick_add_vehicle(StatList *p_stats, const GenericVehicle *p_vehicle, uint32_t current_tick) {
    StatsTick *p_tick = NULL;

    if (checkNull(p_stats) || checkNull(p_vehicle) || checkNull(p_stats->p_current_tick)) {
        return ERROR;
    }

    p_tick = p_stats->p_current_tick;

    if (p_vehicle->park_house_entered == current_tick) {
        uint32_t entered_at = p_vehicle->park_house_entered;
        p_tick->entered += 1U;

        if (entered_at > p_vehicle->created_at_tick) {
            uint32_t wait_ticks = entered_at - p_vehicle->created_at_tick;
            p_tick->queue_wait_entered_sum_ticks += wait_ticks;
            p_tick->queue_wait_entered_count += 1U;
            if (wait_ticks > p_tick->queue_wait_max_ticks_tick) {
                p_tick->queue_wait_max_ticks_tick = wait_ticks;
            }
        }
    }

    if (p_vehicle->park_house_left == current_tick) {
        p_tick->departed += 1U;

        if (p_vehicle->park_house_entered > 0U && p_vehicle->park_house_left >= p_vehicle->park_house_entered) {
            uint32_t parking_duration = p_vehicle->park_house_left - p_vehicle->park_house_entered;
            p_tick->parking_duration_departed_sum_ticks += parking_duration;
            p_tick->parking_duration_departed_count += 1U;
        }
        else
        {
            print_error_s("parkhouse_lef of vehicle is bigger than current tick???", HIGH);
        }
    }

    if (p_vehicle->base.type == CAR) {
        const Car *p_car = (const Car *)p_vehicle;
        if (p_car->spaces_needed > p_car->minimum_spaces) {
            p_tick->bad_parking_cases += 1U;
        }
    }

    return OK;
}

const StatsTick * stats_get_latest_tick(const StatList *p_stats) {
    if (checkNull(p_stats)) {
        return NULL;
    }

    return p_stats->p_tick_tail;
}

int stats_build_summary(const StatList *p_stats, StatsSummary *p_summary)
{
    const StatsTick *p_next_tick = NULL;

    double sum_capacity_taken_percent = 0.0;
    uint64_t sum_queue_length_end = 0U;
    uint64_t sum_entered = 0U;
    uint64_t sum_departed = 0U;
    uint64_t sum_queue_wait_entered_ticks = 0U;
    uint64_t sum_queue_wait_entered_count = 0U;
    uint64_t sum_parking_duration_departed_ticks = 0U;
    uint64_t sum_parking_duration_departed_count = 0U;
    uint32_t queue_active_ticks = 0U;
    uint32_t blocker_full_ticks = 0U;

    if (checkNull(p_stats) || checkNull(p_summary)) {
        return ERROR;
    }

    memset(p_summary, 0, sizeof(StatsSummary));
    p_summary->first_full_tick = -1;

    p_next_tick = p_stats->p_tick_head;
    while (p_next_tick != NULL)
    {
        const StatsTick *p_tick = p_next_tick;
        float current_capacity_percent = 0.0f;

        p_summary->total_ticks++;

        p_summary->arrivals_total += p_tick->arrivals_generated;
        p_summary->enqueued_total += p_tick->enqueued;
        p_summary->entered_total += p_tick->entered;
        p_summary->departed_total += p_tick->departed;
        p_summary->net_occupancy_change_total +=
            ((double)p_tick->entered - (double)p_tick->departed);
        p_summary->queue_rejections_total += p_tick->queue_rejections;
        p_summary->bad_parking_cases_total += p_tick->bad_parking_cases;

        if (p_tick->capacity_total > 0U)
        {
            current_capacity_percent =
                ((float)p_tick->capacity_taken * 100.0f) /
                (float)p_tick->capacity_total;
        }
        else
        {
            current_capacity_percent = 0.0f;
        }

        sum_capacity_taken_percent += current_capacity_percent;
        sum_queue_length_end += p_tick->queue_length_end;
        sum_entered += p_tick->entered;
        sum_departed += p_tick->departed;

        if (p_tick->capacity_free == 0U)
        {
            p_summary->full_ticks++;

            if (p_summary->first_full_tick < 0)
            {
                p_summary->first_full_tick = (int32_t)p_tick->current_tick;
            }
        }

        if (current_capacity_percent > p_summary->capacity_taken_percent_peak)
        {
            p_summary->capacity_taken_percent_peak = current_capacity_percent;
            p_summary->capacity_taken_peak_tick = p_tick->current_tick;
        }

        if (p_tick->queue_length_end > p_summary->queue_length_peak)
        {
            p_summary->queue_length_peak = p_tick->queue_length_end;
            p_summary->queue_length_peak_tick = p_tick->current_tick;
        }

        if (p_tick->queue_length_end > 0U)
        {
            queue_active_ticks++;
        }

        if (p_tick->blocker_full_active > 0U)
        {
            blocker_full_ticks++;
        }

        sum_queue_wait_entered_ticks += p_tick->queue_wait_entered_sum_ticks;
        sum_queue_wait_entered_count += p_tick->queue_wait_entered_count;

        if (p_tick->queue_wait_max_ticks_tick > p_summary->queue_wait_max_ticks)
        {
            p_summary->queue_wait_max_ticks = p_tick->queue_wait_max_ticks_tick;
        }

        sum_parking_duration_departed_ticks +=
            p_tick->parking_duration_departed_sum_ticks;
        sum_parking_duration_departed_count +=
            p_tick->parking_duration_departed_count;

        if (p_summary->total_ticks == 1U)
        {
            p_summary->capacity_total = p_tick->capacity_total;
        }

        p_next_tick = p_tick->p_next;
    }

    if (p_summary->total_ticks > 0U)
    {
        p_summary->capacity_taken_percent_avg =
            (float)(sum_capacity_taken_percent / (double)p_summary->total_ticks);

        p_summary->entered_per_tick_avg =
            (float)((double)sum_entered / (double)p_summary->total_ticks);

        p_summary->departed_per_tick_avg =
            (float)((double)sum_departed / (double)p_summary->total_ticks);

        p_summary->queue_length_avg =
            (float)((double)sum_queue_length_end / (double)p_summary->total_ticks);

        p_summary->queue_active_ratio_percent =
            ((float)queue_active_ticks * 100.0f) / (float)p_summary->total_ticks;

        p_summary->blocker_full_ratio_percent =
            ((float)blocker_full_ticks * 100.0f) / (float)p_summary->total_ticks;
    }

    if (sum_queue_wait_entered_count > 0U)
    {
        p_summary->queue_wait_avg_ticks =
            (uint32_t)(sum_queue_wait_entered_ticks / sum_queue_wait_entered_count);
    }
    else
    {
        p_summary->queue_wait_avg_ticks = 0U;
    }

    if (sum_parking_duration_departed_count > 0U)
    {
        p_summary->parking_duration_avg_ticks =
            (uint16_t)(sum_parking_duration_departed_ticks / sum_parking_duration_departed_count);
    }
    else
    {
        p_summary->parking_duration_avg_ticks = 0U;
    }

    if (p_summary->entered_total > 0U)
    {
        p_summary->bad_parking_share_percent =
            ((float)p_summary->bad_parking_cases_total * 100.0f) / (float)p_summary->entered_total;
    }
    else
    {
        p_summary->bad_parking_share_percent = 0.0f;
    }

    return OK;
}
