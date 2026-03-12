#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_PARKHAUS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_PARKHAUS_H
#include "types.h"
/**
 * Represents a Parkhaus owned by a Simulation.
 * It owns parked vehicles and gate queues and is processed via SimulationObject ticks.
 */

/**
 * @brief Initializes a Parkhaus from settings and gate queues.
 * @author: ibach
 * @param p_parkhaus Pointer to the Parkhaus to initialize.
 * @param p_settings Pointer to simulation settings.
 * @param p_gate_queues Pointer to the gate queue array.
 */
int parkhouse_init(Parkhaus* p_parkhaus,
                   const Settings* p_settings,
                   Queue** p_gate_queues);

/**
 * @brief Executes one simulation tick for the Parkhaus.
 * @author: ibach
 * @param p_self Pointer to the Parkhaus casted as SimulationObject.
 * @param p_settings Pointer to simulation settings.
 * @param p_StatList Pointer to the current statistics list.
 * @param current_tick Current simulation tick.
 */
int parkhouse_tick(SimulationObject* p_self, const Settings* p_settings, StatList* p_StatList,
                   uint32_t current_tick);


/**
 * @brief Removes and destroys a vehicle according to its type.
 * @author: ibach
 * @param p_vehicle Pointer to the vehicle to remove.
 */
int remove_vehicle(GenericVehicle* p_vehicle);

/**
 * @brief Frees parked vehicles and resets Parkhaus-owned references.
 * @author: ibach
 * @param p_parkhaus Pointer to the Parkhaus to free.
 */
int parkhouse_free(Parkhaus* p_parkhaus);


/**
 * @brief Removes all vehicles whose parking duration ends in this tick.
 * @author: ibach
 * @param current_tick Current simulation tick.
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_settings Pointer to simulation settings.
 * @param p_StatList Pointer to the statistics list.
 * @param pp_vehicle_list_head Pointer to the head pointer of parked vehicles.
 */
int parkhouse_tick_empty_general(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                                 StatList* p_StatList, GenericVehicle** pp_vehicle_list_head);

/**
 * @brief Processes vehicle entries for one gate in one tick.
 * @author: ibach
 * @param current_tick Current simulation tick.
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_settings Pointer to simulation settings.
 * @param p_StatList Pointer to the statistics list.
 * @param pp_vehicle_list_head Pointer to the head pointer of parked vehicles.
 * @param p_gate_queue Pointer to the gate queue.
 */
int parkhouse_tick_fill_general(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                                StatList* p_StatList, GenericVehicle** pp_vehicle_list_head, Queue* p_gate_queue);

/**
 * @brief Distributes entries across multiple gates using subticks.
 * @author: ibach
 * @param current_tick Current simulation tick.
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_settings Pointer to simulation settings.
 * @param p_StatList Pointer to the statistics list.
 * @param p_gate_queues Pointer to gate queues.
 */
int parkhouse_fill_subtick(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                           StatList* p_StatList, Queue* p_gate_queues);

/**
 * @brief Executes one subtick routine for a single gate.
 * @author: ibach
 * @param current_tick Current simulation tick.
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_settings Pointer to simulation settings.
 * @param p_StatList Pointer to the statistics list.
 * @param p_gate_queue Pointer to the gate queue.
 * @param last_cycle Flag indicating the last subtick cycle.
 */
int parkhouse_fill_subtick_routine(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                                   StatList* p_StatList, Queue* p_gate_queue, int last_cycle);

/**
 * @brief Takes one vehicle from a queue and computes required parking space.
 * @author: ibach
 * @param p_parkhaus Pointer to the Parkhaus.
 * @param p_gate_queue Pointer to the gate queue.
 * @param pp_vehicle Output pointer for the dequeued vehicle.
 */
uint16_t fill_from_queue(Parkhaus* p_parkhaus, Queue* p_gate_queue, GenericVehicle** pp_vehicle);

/**
 * @brief Stores remaining demand in queue and records queue rejections.
 * @author: ibach
 * @param p_StatList Pointer to the statistics list.
 * @param p_gate_queue Pointer to the gate queue.
 * @param demand_remaining Remaining demand in this tick.
 * @param current_tick Current simulation tick.
 * @param p_settings Pointer to simulation settings.
 */
int open_demand(StatList* p_StatList, Queue* p_gate_queue, uint16_t demand_remaining, uint32_t current_tick,
                const Settings* p_settings);

/**
 * @brief Removes a leaving vehicle and updates occupancy/state.
 * @author: ibach
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_StatList Pointer to the statistics list.
 * @param pp_vehicle_list_head Pointer to the head pointer of parked vehicles.
 * @param p_vehicle Pointer to the leaving vehicle.
 * @param current_tick Current simulation tick.
 */
int vehicle_leaving(Parkhaus* p_parkhouse, StatList* p_StatList, GenericVehicle** pp_vehicle_list_head,
                    GenericVehicle* p_vehicle, uint32_t current_tick);


/**
 * @brief Creates a random vehicle and enqueues it into a gate queue.
 * @author: ibach
 * @param p_gate_queue Pointer to the target queue.
 * @param current_tick Current simulation tick.
 * @param p_settings Pointer to simulation settings.
 */
int queue_add_random_vehicle(Queue* p_gate_queue, uint32_t current_tick, const Settings* p_settings);

/**
 * @brief Creates a random vehicle according to simulation settings.
 * @author: ibach
 * @param current_tick Current simulation tick.
 * @param p_settings Pointer to simulation settings.
 */
GenericVehicle* create_random_vehicle(uint32_t current_tick, const Settings* p_settings);

/**
 * @brief Appends a vehicle to the parked vehicle list.
 * @author: ibach
 * @param p_parkhaus Pointer to the Parkhaus.
 * @param p_vehicle Pointer to the vehicle to park.
 */
int park_vehicle(Parkhaus* p_parkhaus, GenericVehicle* p_vehicle);


/**
 * @brief Returns currently available free space in the Parkhaus.
 * @author: ibach
 * @param p_parkhouse Pointer to the Parkhaus.
 */
uint16_t get_open_space(const Parkhaus* p_parkhouse);

/**
 * @brief Updates occupancy and statistics after a vehicle exit.
 * @author: ibach
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_StatList Pointer to the statistics list.
 * @param p_vehicle Pointer to the leaving vehicle.
 * @param required_space Space occupied by the vehicle.
 * @param current_tick Current simulation tick.
 */
int update_on_vehicle_exit(Parkhaus* p_parkhouse, StatList* p_StatList, GenericVehicle* p_vehicle,
                           uint16_t required_space, uint32_t current_tick);

/**
 * @brief Updates occupancy and statistics after a vehicle entry.
 * @author: ibach
 * @param p_parkhouse Pointer to the Parkhaus.
 * @param p_StatList Pointer to the statistics list.
 * @param p_vehicle Pointer to the entering vehicle.
 * @param required_space Reserved space for the vehicle.
 * @param current_tick Current simulation tick.
 */
int update_on_vehicle_entry(Parkhaus* p_parkhouse, StatList* p_StatList, GenericVehicle* p_vehicle,
                            uint16_t required_space, uint32_t current_tick);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_PARKHAUS_H