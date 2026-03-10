#include <string.h>

#include "Stats.h"
#include "types.h"
#include "utils/SafteyUtils.h"

int StatsTick_init(Simulation *p_sim, uint16_t capacity_total, uint32_t current_tick) {
}

StatList * StatList_init(Simulation *p_simulation) {
}

int StatList_free(StatList *p_stats) {
}

int StatsTick_free(StatList *p_stats) {
}

int stats_tick_set_capacity(StatList *p_stats, uint16_t taken, uint16_t free) {
}

int stats_tick_add_queue_rejections(StatList *p_stats, uint16_t amount) {
}

int stats_tick_add_arrivals_generated(StatList *p_stats, uint16_t amount) {
}

int stats_tick_add_blocker_full_active(StatList *p_stats) {
}

int stats_tick_add_vehicle(StatList *p_stats, const GenericVehicle *p_vehicle, uint32_t current_tick) {
}

const StatsTick * stats_get_latest_tick(const StatList *p_stats) {
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

    checkNull(p_stats);
    checkNull(p_summary);

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

    return OK;
}
