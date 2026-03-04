#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GRAPHHANDLER_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GRAPHHANDLER_H

    #include "../types.h"

    /**
     * @brief Generate a graph representation from a statistics file.
     * @param src_path Relative path to the statistics file to read from; must not be NULL.
     * @param dest_path Relative path to the graph output file. If left NULL, will generate
     * a new file in the root folder of the exe.
     * @return 0 on success, non-zero on error.
     */
    int graphhandler_generate_from_file(const char *src_path, const char *dest_path);

    /**
     * @brief Optionally, generate a graph directly from the current Simulation state,
     *        without reading an intermediate statistics file.
     * @param p_sim Pointer to the Simulation to read statistics from.
     * @param dest_path Relative path to the graph output file. If left NULL, will generate
     * a new file in the root folder of the exe.
     * @return 0 on success, non-zero on error.
     */
    int graphhandler_generate_from_simulation(const Simulation *p_sim,
                                              const char *dest_path);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_GRAPHHANDLER_H
