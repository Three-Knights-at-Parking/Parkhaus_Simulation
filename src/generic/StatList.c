#include "utils/StatList.h"
#include "utils/SafteyUtils.h"

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


int statlist_compute_summary(const Simulation *p_sim, StatsSummary *p_summary) {
    if (checkNull(p_sim) || checkNull(p_sim->StatList) || checkNull(p_summary)) {
        return ERROR;
    }

    const StatList *p_list = p_sim->StatList;
    const StatsTick *p_tick = p_list->p_tick_head;

    if (checkNull(p_tick)) {
        print_warning_s("No stats available to compute.");
        return ERROR;
    }
    memset(p_summary, 0, sizeof(StatsSummary));

    p_summary->first_full_tick = -1;

    /*
     * Using Accumulators is probs the best way to do this
     * not sure tho
     */
    double sum_capacity_taken_percent = 0.0;
    uint64_t sum_queue_length = 0;
    uint64_t sum_queue_wait_ticks = 0;
    uint32_t sum_queue_wait_count = 0;
    uint64_t sum_parking_duration_ticks = 0;
    uint32_t sum_parking_duration_count = 0;
    uint32_t count_queue_active = 0;
    uint32_t count_blocker_full = 0;
    while (p_tick != NULL) {
        p_summary->total_ticks++;
        p_summary->capacity_total = p_tick->capacity_total;

        float_t current_cap_percent = 0.0f;
        if (p_tick->capacity_total > 0) {
            current_cap_percent = ((float_t)p_tick->capacity_taken / (float_t) p_tick->capacity_total) * 100.0f;
        }
        sum_capacity_taken_percent += current_cap_percent;

        if (current_cap_percent > p_summary->capacity_taken_percent_peak) {
            p_summary->capacity_taken_percent_peak = current_cap_percent;
            p_summary->capacity_taken_peak_tick = p_tick->current_tick;
        }

        if (p_tick->capacity_free == 0) {
            if (p_summary->first_full_tick == -1) {
                p_summary->first_full_tick = (int32_t)p_tick->current_tick;
            }
            p_summary->full_ticks++;
        }
        p_summary->arrivals_total += p_tick->arrivals_generated;
        p_summary->enqueued_total += p_tick->enqueued;
        p_summary->entered_total += p_tick->entered;
        p_summary->departed_total += p_tick->departed;
        sum_queue_length += p_tick->queue_length_end;
        if (p_tick->queue_length_end > p_summary->queue_length_peak) {
            p_summary->queue_length_peak = p_tick->queue_length_end;
            p_summary->queue_length_peak_tick = p_tick->current_tick;
        }

        p_summary->queue_rejections_total += p_tick->queue_rejections;
        sum_queue_wait_ticks += p_tick->queue_wait_entered_sum_ticks;
        sum_queue_wait_count += p_tick->queue_wait_entered_count;

        if (p_tick->queue_wait_max_ticks_tick > p_summary->queue_wait_max_ticks) {
            p_summary->queue_wait_max_ticks = p_tick->queue_wait_max_ticks_tick;
        }

        if (p_tick->queue_length_end > 0) {
            count_queue_active++;
        }
        sum_parking_duration_ticks += p_tick->parking_duration_departed_sum_ticks;
        sum_parking_duration_count += p_tick->parking_duration_departed_count;
        if (p_tick->blocker_full_active > 0) {
            count_blocker_full++;
        }
        p_summary->bad_parking_cases_total += p_tick->bad_parking_cases;

        p_tick = p_tick->p_next;
    }

    if (p_summary->total_ticks > 0) {
        p_summary->capacity_taken_percent_avg = (float_t)(sum_capacity_taken_percent / p_summary->total_ticks);
        p_summary->entered_per_tick_avg = (float_t)p_summary->entered_total / (float_t) p_summary->total_ticks;
        p_summary->departed_per_tick_avg = (float_t)p_summary->departed_total / (float_t) p_summary->total_ticks;
        p_summary->queue_length_avg = (float_t)sum_queue_length / (float_t) p_summary->total_ticks;
        p_summary->queue_active_ratio_percent = ((float_t)count_queue_active / (float_t) p_summary->total_ticks) * 100.0f;
        p_summary->blocker_full_ratio_percent = ((float_t)count_blocker_full / (float_t) p_summary->total_ticks) * 100.0f;
    }

    p_summary->net_occupancy_change_total = (double_t)p_summary->entered_total - (double_t)p_summary->departed_total;

    if (sum_queue_wait_count > 0) {
        p_summary->queue_wait_avg_ticks = (uint32_t)(sum_queue_wait_ticks / sum_queue_wait_count);
    }

    if (sum_parking_duration_count > 0) {
        p_summary->parking_duration_avg_ticks = (uint16_t)(sum_parking_duration_ticks / sum_parking_duration_count);
    }

    if (p_summary->entered_total > 0) {
        p_summary->bad_parking_share_percent = ((float_t)p_summary->bad_parking_cases_total / (float_t) p_summary->entered_total) * 100.0f;
    }

    return OK;
}
