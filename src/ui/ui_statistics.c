#include <stdio.h>
#include <stdint.h>

#include "../include/ui/ui_statistics.h"
#include "../include/types.h"

/* ========================================================================= */
/* Helper functions                                                          */
/* ========================================================================= */

static const char *derive_status_text(const StatsTick *p_stats_tick)
{
    if (p_stats_tick == NULL)
    {
        return "UNKNOWN";
    }

    if (p_stats_tick->capacity_free == 0U)
    {
        return "FULL";
    }

    return "OK";
}

static float calc_util_percent(const StatsTick *p_stats_tick)
{
    if (p_stats_tick == NULL)
    {
        return 0.0f;
    }

    if (p_stats_tick->capacity_total == 0U)
    {
        return 0.0f;
    }

    return ((float)p_stats_tick->capacity_taken * 100.0f) /
           (float)p_stats_tick->capacity_total;
}

/* Calculates average queue wait time for vehicles that entered (in Ticks) */
static double calc_avg_queue_wait_entered(const StatsTick *p_stats_tick)
{
    if (p_stats_tick == NULL)
    {
        return 0.0;
    }

    if (p_stats_tick->queue_wait_entered_count == 0U)
    {
        return 0.0;
    }

    return (double)p_stats_tick->queue_wait_entered_sum_ticks /
           (double)p_stats_tick->queue_wait_entered_count;
}

/* ========================================================================= */
/* Header / Legend                                                           */
/* ========================================================================= */

static void ui_statistics_print_header_normal(void)
{
    printf("\n");
    printf("======================================================================\n");
    printf("                PARKHAUS - TICK STATISTICS (NORMAL)\n");
    printf("----------------------------------------------------------------------\n");
    printf("Legend:\n");
    printf("  Status : OK = capacity available | FULL = no free capacity\n");
    printf("  OCC    : occupied spots / total spots\n");
    printf("  Util %% : utilization of parking capacity\n");
    printf("  Queue  : queue length at tick end\n");
    printf("  Arr/In/Out/Rej: arrivals / entered / departed / rejected\n");
    printf("======================================================================\n");
    printf("\n");
}

static void ui_statistics_print_header_verbose(void)
{
    printf("\n");
    printf("======================================================================\n");
    printf("           PARKHAUS - TICK STATISTICS (VERBOSE)\n");
    printf("----------------------------------------------------------------------\n");
    printf("All available raw metrics per tick are printed.\n");
    printf("======================================================================\n");
    printf("\n");
}

void ui_statistics_print_header(const Settings *p_settings)
{
    if (p_settings == NULL)
    {
        return;
    }

    if (p_settings->output_mode == NONE)
    {
        return;
    }

    if (p_settings->output_mode == VERBOSE)
    {
        ui_statistics_print_header_verbose();
    }
    else
    {
        ui_statistics_print_header_normal();
    }
}

/* ========================================================================= */
/* Tick output                                                               */
/* ========================================================================= */

static void ui_statistics_print_tick_normal(const StatsTick *p_stats_tick)
{
    const char *p_status_text = NULL;
    float util_percent = 0.0f;
    double avg_wait = 0.0;

    if (p_stats_tick == NULL)
    {
        return;
    }

    p_status_text = derive_status_text(p_stats_tick);
    util_percent = calc_util_percent(p_stats_tick);
    avg_wait = calc_avg_queue_wait_entered(p_stats_tick);       //Will be implemented in next step

    printf("+--------------------------------------------------------------------+\n");
    printf("| Tick: %lu   Status: %s\n",
           (unsigned long)p_stats_tick->current_tick,
           p_status_text);

    printf("| OCC : %u/%u   ",
           (unsigned)p_stats_tick->capacity_taken,
           (unsigned)p_stats_tick->capacity_total);
    build_occupancy_bar(util_percent);
    printf("  %.1f%%\n", format_float_1(util_percent));

    printf("| Queue: %u | Arrivals: %u | In: %u | Out: %u | Rej: %lu\n",
           (unsigned)p_stats_tick->queue_length_end,
           (unsigned)p_stats_tick->arrivals_generated,
           (unsigned)p_stats_tick->entered,
           (unsigned)p_stats_tick->departed,
           (unsigned long)p_stats_tick->queue_rejections);

    printf("| Avg Queue Wait (entered): %.2f ticks\n", avg_wait);
    printf("+--------------------------------------------------------------------+\n");
}