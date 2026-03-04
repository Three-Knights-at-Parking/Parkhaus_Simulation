#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAVEHANDLER_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAVEHANDLER_H

#include "../types.h"

    /**
    * @brief Append statistics for a single simulation tick to a file and forward to UI.
    *
    * This function takes a StatsTick snapshot (typically the one just created
    * at the end of the current tick) and appends one line to a CSV
    * statistics file located inside the ../stats/ folder.
    *
    * The effective file path is resolved as follows:
    *  - The base folder ../stats/ is created if it does not exist.
    *  - If dest_path is NULL or empty, the default file ../stats/stats.csv is used.
    *  - If dest_path contains "..", a safe fallback ../stats/stats.csv is used.
    *  - Otherwise, dest_path is treated as a file name under ../stats/.
    *
    * The amount of detail depends on the OutputMode in the Settings referenced
    * by p_sim:
    *  - NONE:    no file output generated (the function returns immediately).
    *  - NORMAL:  only a minimal set of core statistics per tick is written.
    *  - VERBOSE: an extended set of Tick statistics is written.
    *
    * In addition, the given StatsTick is forwarded to the UI so it can be printed.
    *
    * @param p_sim       Pointer to the Simulation containing Settings and Parkhaus.
    * @param p_tickstats Pointer to the StatsTick snapshot to write.
    * @param dest_path   Optional file name under ../stats/ (may be NULL or empty).
    *
    * @return OK (0) on success, ERROR (-1) on error.
    */
    int savehandler_save_tick(const Simulation *p_sim,
                              const StatsTick *p_tickstats,
                              const char *dest_path);

    /**
    * @brief Save final aggregated statistics of a simulation run to a file and forward to UI.
    *
    * This function writes the aggregated statistics (StatsSummary) that were
    * computed from all StatsTick entries at the end of the simulation to a
    * csv file inside the ../stats/ folder.
    *
    * Path resolution follows the same rules as savehandler_save_tick:
    *  - Base folder ../stats/ is created if needed.
    *  - dest_path NULL/empty -> ../stats/stats.csv.
    *  - dest_path with ".."  -> ../stats/stats.csv.
    *  - otherwise            -> ../stats/<dest_path>.
    *
    * The level of detail again depends on the OutputMode in the Settings
    * referenced by p_sim. The summary is also forwarded to the UI for display.
    *
    * @param p_sim     Pointer to the Simulation containing Settings and Parkhaus.
    * @param p_summary Pointer to the aggregated statistics (StatsSummary).
    * @param dest_path Optional file name under ../stats/ (may be NULL or empty).
    *
    * @return OK (0) on success, ERROR (-1) on error.
    */
    int savehandler_save_summary(const Simulation *p_sim,
                                 const StatsSummary *p_summary,
                                 const char *dest_path);

    /**
     * @brief Load previously saved statistics from a file in ../stats/ and print or display them.
     *
     * This function loads a statistics file and forwards all lines to the UI
     * for printing. It does not modify the current
     * Simulation state.
     *
     * Path handling:
     *  - Base folder is ../stats/.
     *  - If src_path is NULL, empty, or contains "..", the default file
     *    ../stats/stats.csv is used.
     *  - Otherwise src_path is treated as a file name under ../stats/.
     *
     * @param src_path Optional file name under ../stats/ to load (may be NULL or empty).
     *
     * @return OK (0) on success, ERROR (-1) on error.
     */
    int savehandler_load_and_print(const char *src_path);

    /**
     * @brief Resolve and validate a statistics file path under the ../stats/ folder.
     *
     * Ensures that the base folder ../stats/ exists and
     * returns a full path for the statistics file:
     *  - If dest_path is NULL or empty, "../stats/stats.csv" is used.
     *  - If dest_path contains "..", a safe fallback "../stats/stats.csv" is used.
     *  - Otherwise dest_path is treated as a file name under "../stats/".
     *
     *
     * @note This function will create the necessary folder(s) under ../stats/ !
     * On error, an empty string may be returned as an error indicator to the caller.
     * @param dest_path Optional file name under ../stats/ (may be NULL or empty).
     *
     * @return Pointer to a resolved path string, or an empty string on error.
     *         Ownership and lifetime of the returned string must be defined by
     *         the implementation (e.g. static buffer or caller-allocated).
     */
    const char *savehandler_resolve_stats_path(const char *dest_path);

    /**
     * @brief Write settings metadata and CSV header line if the stats file is new/empty.
     *
     * If the file is empty, write metadata and column titles into the stats file. This function
     * also takes the output mode into account.
     *
     * @note This function is intended for internal use by the SaveHandler and assumes that
     * @p file is already opened in append or write mode. Use this function on your own risk!
     *
     * @param file Opened file handle for the stats file.
     * @param p_sim Pointer to the Simulation whose Settings are written as metadata.
     * @param mode  OutputMode that determines which header columns are used.
     */
    void savehandler_write_header_if_new(FileHandle *file,
                                         const Simulation *p_sim,
                                         enum OutputMode mode);



#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAVEHANDLER_H
