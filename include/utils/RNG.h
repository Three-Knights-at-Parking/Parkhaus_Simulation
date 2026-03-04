#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_RNG_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_RNG_H

#include "../types.h"

    /**
     * @brief Initialize the global random number generator for the simulation.
     *
     * If p_settings is not NULL and p_settings->rand_seed != -1,
     * that value is used as seed. Otherwise (p_settings == NULL or
     * p_settings->rand_seed == -1), the current time is used as seed.
     * This must be called once before any other rng_* function is used!
     *
     * @note if p_settings or p_settings->rand_seed is null, this will silently fail over to default!
     *
     * @param p_settings Pointer to Settings with rand_seed configured, or NULL.
     * @return 0 on success, non-zero on error.
     * @author Luca Perri
     */
    int rng_init(const Settings *p_settings);

    /**
     * @brief Get a random unsigned 32-bit value in the full range [0, UINT32_MAX].
     *
     * This is a low-level helper; higher-level functions like rng_range_int()
     * or rng_percent() are usually easier to use in the simulation logic.
     *
     * @return Random value in [0, UINT32_MAX].
     */
    uint32_t rng_next_u32(void);

    /**
     * @brief Get a random integer in the inclusive range [min, max].
     *
     * @note If min > max, the values are swapped internally so the function
     * still returns a value in the valid range.
     *
     * @param min Lower bound of the range (inclusive).
     * @param max Upper bound of the range (inclusive).
     * @return Random integer in [min, max].
     * @author Luca Perri
     */
    int32_t rng_range_int(int32_t min, int32_t max);

    /**
     * @brief Get a random percentage value in [0, 100].
     *
     * This is useful for probability checks, e.g. deciding whether
     * a car parks badly or whether an event occurs.
     *
     * @return Random integer between 0 and 100 (inclusive).
     * @author Luca Perri
     */
    uint8_t rng_percent(void);

    /**
     * @brief Draw a random parking time (in ticks) for a newly created vehicle.
     *
     * @note The concrete distribution will be defined in the implementation.
     * For now treat as uniform distribution.
     *
     * @param min_ticks Minimum parking time in ticks (inclusive).
     * @param max_ticks Maximum parking time in ticks (inclusive).
     * @return Random parking time in ticks in [min_ticks, max_ticks].
     * @author Luca Perri
     */
    uint32_t rng_parking_time(uint32_t min_ticks, uint32_t max_ticks);

    /**
     * @brief Draw a random gate index for a Parkhaus with num_gates gates.
     *
     * Useful if demand should be distributed randomly (implementation defined, treat as uniform for now)
     * over all gates.
     *
     * @param num_gates Number of available gates (> 0).
     * @return Gate index in [0, num_gates - 1], or 0 if num_gates == 0.
     * @author Luca Perri
     */
    uint32_t rng_gate_index(uint32_t num_gates);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_RNG_H
