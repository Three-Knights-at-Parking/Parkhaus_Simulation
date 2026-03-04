#ifndef UI_STATISTICS_H
#define UI_STATISTICS_H

/**
 * @file ui_statistics.h
 * @brief Terminal output formatting for simulation statistics (per tick + summary).
 *
 * This module provides output functions for:
 * - Tick-by-tick statistics (StatsTick)
 * - Final aggregated summary (StatsSummary)
 *
 * The UI only formats and prints values. The statistics are produced by the
 * simulation/data layer and passed to the UI as StatsTick / StatsSummary objects.
 */

#include "types.h"

/**
 * @brief Width (characters) of the occupancy bar shown in NORMAL output.
 */
#define UI_STATS_BAR_WIDTH (20)

/**
 * @brief Prints the statistics header depending on the output mode.
 *
 * @param[in] p_settings Settings containing the selected output mode.
 */
void ui_statistics_print_header(const Settings *p_settings);

/**
 * @brief Prints a single tick statistics block depending on output mode.
 *
 * @param[in] p_tick     Tick snapshot to print.
 * @param[in] p_settings Settings containing the selected output mode.
 */
void ui_statistics_print_tick(const StatsTick *p_tick, const Settings *p_settings);

/**
 * @brief Prints the final aggregated simulation summary.
 *
 * @param[in] p_summary  Aggregated summary statistics (may be NULL).
 * @param[in] p_settings Settings containing the selected output mode.
 */
void ui_statistics_print_final(const StatsSummary *p_summary, const Settings *p_settings);

#endif /* UI_STATISTICS_H */