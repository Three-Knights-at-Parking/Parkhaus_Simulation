#ifndef SIMULATION_H
#define SIMULATION_H

/*
 * File: simulation.h
 * Description: Declarations for simulation menu and simulation start interface.
 */

#include "ui.h"
#include "../config.h"

/* Max valid menu number in simulation menu (0..SIMULATION_MAX_VALID_NUMBER). */
#define SIMULATION_MAX_VALID_NUMBER 2

/* Max valid menu number in post simulation prompt (0..SIM_POST_MAX_VALID_NUMBER). */
#define SIM_POST_MAX_VALID_NUMBER 1

/**
 * @brief Prints the simulation menu including current configuration.
 *
 * @param[in] settings Current settings to display.
 */
void print_simulationscreen(Settings settings);

/**
 * @brief Post-simulation user prompt that offers jumping to the storage folder.
 *
 * @param[in] p_sim_output_path Path to the directory where simulation files were saved.
 */
void post_simulation_prompt(char *p_sim_output_path);

/**
 * @brief Handles the simulation menu interaction.
 *
 * @param[in] settings Current settings used to start the simulation.
 * @return Next UI state depending on user selection.
 */
ui_state simulation_menu(Settings settings);

/**
 * @brief Starts the simulation (core function; implemented by simulation logic module).
 *
 * Must create a new output directory for this simulation run and return its path.
 *
 * @param[in] settings Current settings used for the simulation.
 * @return Pointer to a string containing the output directory path.
 */
char *start_simulation(Settings settings);

#endif /* SIMULATION_H */