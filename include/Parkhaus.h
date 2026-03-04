#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_PARKHAUS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_PARKHAUS_H
 #include "types.h"
/**
 * Represents a Parkhaus owned by a Simulation. Handles (and owns!) all parking cars, its Queue and "inherits"
 * SimulationObject.psc for easy use of the tick() function. Simulation calls this object's free
 * function to free the Parkhaus which in turn frees the Queue and the underlying cars. Parkaus also owns
 * the Settings of a Simulation and frees it at the end.
 */

    /**
     * @brief Initialize a Parkhaus object from given Settings.
     * This sets all fields, connects the gate queues array, and prepares the parked-vehicle list.
     * @param p_parkhaus Pointer to the Parkhaus to initialize.
     * @param p_settings Pointer to the Settings used as configuration source.
     * @param p_gate_queues Pointer to the array of Queues that belongs to this Parkhaus.
     * @return 0 on success, non-zero on invalid parameters.
     *
     * @author Luca Perri
     */
    int parkhaus_init(Parkhaus *p_parkhaus,
                      const Settings *p_settings,
                      Queue **p_gate_queues);

    /**
     * @brief Tick function for Parkhaus.
     * @param p_self Pointer to the SimulationObject.psc
     * @param current_tick Current simulation tick.
     *
     * @author Luca Perri
     */
    void parkhaus_tick(SimulationObject *p_self, uint32_t current_tick);

    /**
     * @brief Try to park a vehicle in this Parkhaus.
     *        The vehicle node is added to the list of parked vehicles.
     * @param p_parkhaus Pointer to the Parkhaus.
     * @param p_vehicle Pointer to the GenericVehicle to park.
     *                  Ownership of the vehicle is transferred from the queue to the Parkhaus.
     * @return 0 on success, non-zero if no space is available or parameters are invalid.
     * @author Luca Perri
     */
    int parkhaus_park_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle);

    /**
     * @brief Remove a vehicle from the Parkhaus (e.g., when parking time is over).
     *        The vehicle is removed from the internal list and free the underlying memory.
     * @param p_vehicle Pointer to the GenericVehicle to remove.
     * @return 0 on success, non-zero if the vehicle was not found.
     * @author Luca Perri
     */
    int parkhaus_remove_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle);

    /**
     * @brief Compute the current utilization of the Parkhaus in percent.
     * @param p_parkhaus Pointer to the Parkhaus.
     * @return Utilization as float between 0.0 and 100.0.
     * @author Luca Perri
     */
    float parkhaus_get_utilization(const Parkhaus *p_parkhaus);

    /**
     * @brief Free all dynamic memory that belongs to the Parkhaus.
     *        This includes:
     *        - all parked GenericVehicle nodes,
     *        - any other dynamic resources owned by Parkhaus.
     *        - call the queues free() function.
     * Parkhaus is the owner of its Queue so it will call it's free function!
     * @param p_parkhaus Pointer to the Parkhaus.
     * @author Luca Perri
     */
    void parkhaus_free(Parkhaus *p_parkhaus);

    /**
     * @brief Verarbeitet ausfahrende Fahrzeuge am Tick-Anfang.
     * @author Simon Ibach
     */
    int parkhouse_tick_empty_general(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings,
                                     GenericVehicle **pp_car_list_head);

    /**
     * @brief Verarbeitet Einfahrten fuer ein einzelnes Gate in einem Tick.
     * * @author Simon Ibach
     */
    int parkhouse_tick_fill_general(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings,
                                    GenericVehicle **pp_car_list_head, Queue *p_gate_queue);

    /**
     * @brief Verteilt Einfahrten subtick-basiert ueber mehrere Gates.
     * * @author Simon Ibach
     */
    int parkhouse_fill_subtick(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, Queue *p_gate_queues);

    /**
     * @brief Ein einzelner Subtick-Schritt fuer genau ein Gate.
     * @author Simon Ibach
     */
    int parkhouse_fill_subtick_routine(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings,
                                       Queue *p_gate_queue, int last_cycle);

    /**
     * @brief Uebernimmt ein Fahrzeug aus der Queue und berechnet den benoetigten Platz.
     * @author Simon Ibach
     */
    uint16_t fill_from_queue(Parkhaus* p_parkhaus, Queue *p_gate_queue);

    /**
     * @brief Schreibt verbleibenden Demand in die Queue und zaehlt ggf. Rejections hoch.
     * @author Simon Ibach
     */
    int open_demand(Parkhaus *p_parkhouse, Queue *p_gate_queue, uint16_t queue_max_len, uint16_t demand_remaining);

    /**
     * @brief Entfernt ein Fahrzeug aus dem Parkhaus und gibt den Platz frei.
     * @author Simon Ibach
     */
    int car_leaving(Parkhaus *p_parkhouse, GenericVehicle **pp_car_list_head, GenericVehicle *p_car);

    /**
     * @brief Erstellt eine Queue-Struktur fuer mehrere Gates.
     * @author Simon Ibach
     */
    Queue *parkhaus_create_gate_queues(uint32_t number_of_gates);

    /**
     * @brief Fuegt ein Fahrzeug an einem Gate in die Queue ein.
     * @author Simon Ibach
     */
    int parkhaus_enqueue_at_gate(Queue *p_gate_queues, uint32_t gate_index, GenericVehicle *p_vehicle);

    /**
     * @brief Setzt den Demand-Wert eines Gates.
     * @author Simon Ibach
     */
    int parkhaus_set_gate_demand(Queue *p_gate_queues, uint32_t gate_index, uint16_t demand_value);

    /**
     * @brief Erzeugt ein zufaelliges Fahrzeug und fuegt es in eine Gate-Queue ein.
     * @author Simon Ibach
     */
    int queue_add_random_vehicle(Queue *p_gate_queue);

/**
     * @brief Berechnet den aktuell verfuegbaren Platz im Parkhaus.
     * @author Simon Ibach
     */
    uint16_t get_open_space(const Parkhaus *p_parkhouse);

    /**
     * @brief Aktualisiert Belegung und Ausfahrtszaehler beim Verlassen eines Fahrzeugs.
     * @author Simon Ibach
     */
    void update_parkhouse_on_exit(Parkhaus *p_parkhouse, uint16_t required_space);

    /**
     * @brief Aktualisiert Belegung und Einfahrtszaehler beim Einfahren eines Fahrzeugs.
     * @author Simon Ibach
     */
    void update_parkhouse_on_entry(Parkhaus *p_parkhouse, uint16_t required_space);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_PARKHAUS_H
