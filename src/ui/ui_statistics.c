#include <stdio.h>
#include <stdint.h>

#include "../include/ui/ui_statistics.h"
#include "../include/types.h"

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