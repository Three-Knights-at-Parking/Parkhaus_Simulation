#ifndef UI_STATISTICS_H
#define UI_STATISTICS_H

/**
 * @file ui_statistics.h
 * @brief Terminal output for simulation statistics.
 *
 * This module is responsible for:
 * - printing a statistics header/legend
 * - printing tick-by-tick simulation statistics
 * - printing the final simulation summary
 *
 * Design note:
 * - This module only formats and prints already computed values.
 * - It does not calculate or modify simulation data.
 */

#include "types.h"

/* ========================================================================= */
/* Display settings                                                          */
/* ========================================================================= */

/**
 * @brief Width of the visual occupancy bar in characters.
 */
#define UI_STATS_BAR_WIDTH (20)

/* ========================================================================= */
/* Public interface                                                          */
/* ========================================================================= */

/**
 * @brief Prints the statistics header depending on output mode.
 *
 * For output mode:
 * - NONE: prints nothing
 * - NORMAL: prints compact legend
 * - VERBOSE: prints verbose legend
 * - DEBUG: uses NORMAL legend
 *
 * @param[in] p_settings Pointer to active settings.
 */
void ui_statistics_print_header(const Settings *p_settings);

/**
 * @brief Prints one tick of simulation statistics depending on output mode.
 *
 * For output mode:
 * - NONE: prints nothing
 * - NORMAL: compact tick output
 * - VERBOSE: full raw metrics
 * - DEBUG: compact tick output plus debug line
 *
 * @param[in] p_stats_tick Pointer to tick statistics.
 * @param[in] p_settings Pointer to active settings.
 */
void ui_statistics_print_tick(const StatsTick *p_stats_tick,
                              const Settings *p_settings);

/**
 * @brief Prints the final simulation summary.
 *
 * For output mode NONE, nothing is printed.
 *
 * @param[in] p_stats_summary Pointer to final summary statistics.
 * @param[in] p_settings Pointer to active settings.
 */
void ui_statistics_print_final(const StatsSummary *p_stats_summary,
                               const Settings *p_settings);

#endif /* UI_STATISTICS_H */