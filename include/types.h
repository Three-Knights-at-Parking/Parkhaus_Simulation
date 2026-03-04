/**
 * Define all types in here. If you wish for an object to be manipulated via the Simulation tick,
 * refer to the structure docs on how to implement your type with SimulationObject.
 */
#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_TYPES_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_TYPES_H
#include <math.h>
#include <stdint.h>

typedef struct SimulationObject SimulationObject; // No touchy
typedef void (*SimulationTickFunction)(SimulationObject *p_self, uint32_t current_time); // No touchy
typedef struct Car Car;
typedef struct Parkhaus Parkhaus;
typedef struct Settings Settings;
typedef struct Queue Queue;
typedef struct Simulation Simulation;
typedef struct GenericVehicle GenericVehicle;
typedef struct StatsTick StatsTick;
typedef struct StatsSummary StatsSummary;
typedef struct StatList StatList;


/**
 * Can be expanded in the future to simulate EVs or Motorcycles etc.
 */
enum ObjectType {CAR, PARKHAUS, QUEUE};

/**
 * NONE - Here for completeness
 * NORMAL - The Normal mode, only the most import statistics and averages -> Minimum Required in "Teil 1"
 * VERBOSE - EVERYTHING, every single bit of data (this will dump performance)
 * DEBUG - NORMAL + Debug Messages (i.e. tick - enteredTick() debugInfo debugInfo)
 */
enum OutputMode {NONE, NORMAL, VERBOSE, DEBUG};

/**
 * Determines if vehicles can leave the queue early (at any position)
 */
enum QueueLeavable {LEAVABLE, NON_LEAVABLE};

/**
 * Return values for functions/validation.
 */
enum SuccessState{ ERROR = -1, OK = 0, UNKNOWN = 1};

/**
 * Minimum Vehicle Spaces
 */

enum MinimumSpace{ Bike_Space = 1, Car_Space = 2 };


/**
 * Polymorphic base for anything that has a tick.
 */
struct SimulationObject {
    int id;
    SimulationTickFunction tick;
    enum ObjectType type;
};

/**
 * Vehicle Base for everything that can enter a queue and parking slot.
 * @author Luca Perri
 */
struct GenericVehicle {
    SimulationObject base; // base object
    GenericVehicle *p_next;  // Chain with other Vehicles
    uint32_t created_at_tick; // Tick of creation
    uint32_t park_house_entered; // Entry tick, when the car started parking
    uint32_t park_house_left; // Exit tick, when the car left the parking slot
	uint32_t leaving_in_ticks;
    uint16_t current_slot; // Currently occupied parking spot, 0 if none.
    uint16_t current_floor; // Currently occupied floor, 0 if none or don't care
};

/**
 * Parkhaus parent representing a parkhaus.
 * @author Luca Perri
 */
struct Parkhaus {
    SimulationObject base; // base object.
    char name[20]; // FIXME size is currently arbitrary, we should probably move this to a defined constant in the future && Update in Documentation of Settings and Parkhaus!!
    uint16_t capacity; // Number of total parking spaces.
    uint8_t floors; // Number of floors. This is currently miscellaneous.
    uint32_t capacity_taken; // Number of slots filled.
    Queue **gate_queues; // array of Queue* with size = num_gates
    GenericVehicle *p_parked_head; // linked list of parked vehicles.
    GenericVehicle *p_parked_tail;
};

/**
 * Parent Simulation Object that owns all other child objects.
 * @author Luca Perri
 */
struct Simulation {
    Settings* settings; // The underlying
    uint32_t current_tick; // Current tick time.
    uint16_t real_equivalent; // Tick equivalent in real time (seconds)
    Parkhaus* parkhaus; // The Parkhaus for this Simulation
    StatList* StatList; // Statistikcontainer fuer Tick- und Gesamtwerte
};

/**
 * Child object in Parkhaus representing the Queue at a gate.
 * @author Luca Perri
 */
struct Queue {
    SimulationObject base; // base object.
    uint16_t capacity; // Number of waiting cars.
    GenericVehicle *p_head; // first vehicle in queue
    GenericVehicle *p_tail; // last vehicle in queue
    uint16_t demand;   // demand assigned to this gate in the current tick
    uint8_t max_size; // maximum size of Queue before no cars should be created anymore.
};
//typedef tick_t unit32_t;
//typedef places_p unit16_t

struct Car {
    GenericVehicle base; // base vehicle object
    uint8_t minimum_spaces; // How many spaces this vehicle needs at least.
    uint8_t spaces_needed; // How many spaces this vehicle needs
};

// --- EXAMPLE OF ANOTHER VEHICLE TYPE ---
//
//
// struct ElectricCar {
//     GenericVehicle base; // base vehicle object
//     uint8_t minimum_spaces; // How many spaces this vehicle needs at least.
//     uint8_t spaces_needed; // How many spaces this vehicle needs
//     uint32_t charging_time; // The amount of time this car needs to charge. Can be randomized
// };

struct Settings {
    char* src_path; // Relative path to settings file, if any. Settings takes ownership of the string.
    char* name; // The name of the parking complex. Empty if default ("Rauenegg") ##UI##
    uint16_t capacity; // Total parking spots per floor ##UI##
    uint8_t floors; // Number of floors. This is currently miscellaneous ##UI##
    uint8_t gates; // Number of gates. This will affect queue time. ##UI##
    uint16_t gate_entry_inSec; // Time needed for an vehicle to enter the parkhouse ##UI##
	uint16_t tick_inSec; //Time in seconds of one Tick ##UI##
    uint32_t max_parking_ticks; //maximum of Ticks a car is allowed to Park ##UI##
    uint32_t min_parking_ticks; //minimum of Ticks a car will park -> assumption is 1 ##UI##
	uint8_t mode_select; //0 = none / 1 = normal / 2 = verbose / 3 = Error ##UI##
	// float probability_car_bad_parking; // Probability of a car bad parking
	float entry_probability_perSec_prec; //probability of a Car entering per second ##UI##
    uint16_t real_equivalent; // Tick equivalent in real time (seconds), min. 10.
    enum OutputMode output_mode;
    enum QueueLeavable is_leavable; // Determines if vehicles can leave the queue early at any positions.
    int32_t max_ticks; // Max amount of ticks before the simulation stops. -1 for day equivalent. -2 for 2 day equivalent, ... ##UI##
    int32_t rand_seed; // Specified random seed, -1 if current time should be used.
};

/**
 * Snapshot of raw values for a single simulation tick.
 *
 * Contains only tick-local metrics plus list linkage
 * for history in the statistics container.
 * @author: ibach
 */
struct StatsTick {
    uint32_t current_tick; /**< Current tick of this snapshot. */
    StatsTick *p_prev; /**< Previous tick in the statistics list. */
    StatsTick *p_next; /**< Next tick in the statistics list. */

    /* Raw values at tick end */
    uint16_t capacity_total; /**< Total parking garage capacity. */
    uint16_t capacity_taken; /**< Occupied spaces at tick end. */
    uint16_t capacity_free; /**< Free spaces at tick end. */

    /* Raw flow values */
    uint16_t arrivals_generated; /**< Demand/arrivals in this tick. */
    uint16_t enqueued; /**< Enqueued in this tick. */
    uint16_t entered; /**< Entered garage in this tick. */
    uint16_t departed; /**< Departed from garage in this tick. */

    /* Raw queue values */
    uint8_t queue_length_end; /**< Queue length at tick end (global). */
    uint32_t queue_rejections; /**< Vehicles that could not queue (tick). */
    uint64_t queue_wait_entered_sum_ticks; /**< Total wait time of all vehicles that entered in this tick. */
    uint32_t queue_wait_entered_count; /**< Number of vehicles entered in this tick for wait-time evaluation. */
    uint32_t queue_wait_max_ticks_tick; /**< Maximum wait time among entered vehicles in this tick. */

    /* Raw parking-duration values */
    uint64_t parking_duration_departed_sum_ticks; /**< Total parking duration of all vehicles that departed in this tick. */
    uint32_t parking_duration_departed_count; /**< Number of vehicles departed in this tick for duration evaluation. */

    /* Raw quality/blocker values */
    uint16_t blocker_full_active; /**< How often a Gate was blocked in tis Tick */
    uint16_t bad_parking_cases; /**< Number of "badly parked" cases in this tick. */
};

/**
 * Aggregated final evaluation over the complete simulation.
 *
 * Includes cumulative sums, averages, and peak values
 * across all ticks already committed.
 *
 * Fallback conventions for missing basis data:
 * - Numeric averages/ratios remain 0 if their denominator is 0.
 * - `first_full_tick` remains -1 if no full tick exists.
 *
 * Denominator conventions are documented per field below.
 * @author: ibach
 */
struct StatsSummary {
    SimulationObject base;
    uint32_t total_ticks; /**< Number of evaluated ticks. */

    /* 1) Utilization & capacity */
    uint16_t capacity_total; /**< Total parking garage capacity. */
    float capacity_taken_percent_avg; /**< Average utilization in % (denominator: total_ticks; capacity_total=0 contributes 0%). */
    float capacity_taken_percent_peak; /**< Maximum utilization in %. */
    uint32_t capacity_taken_peak_tick; /**< Tick of maximum utilization. */
    int32_t first_full_tick; /**< First tick without free spaces, -1 if never full. */
    uint32_t full_ticks; /**< Number of ticks at full occupancy. */

    /* 2) Throughput / flow */
    uint64_t arrivals_total; /**< Cumulative arrivals. */
    uint64_t enqueued_total; /**< Cumulatively enqueued. */
    uint64_t entered_total; /**< Cumulative entries. */
    uint64_t departed_total; /**< Cumulative exits. */
    double net_occupancy_change_total; /**< Cumulative net occupancy change. */
    float entered_per_tick_avg; /**< Average entered per tick (denominator: total_ticks). */
    float departed_per_tick_avg; /**< Average departed per tick (denominator: total_ticks). */

    /* 3) Queue (global) */
    float queue_length_avg; /**< Average queue length across all ticks (denominator: total_ticks). */
    uint8_t queue_length_peak; /**< Maximum global queue length. */
    uint32_t queue_length_peak_tick; /**< Tick of global queue peak. */
    uint64_t queue_rejections_total; /**< Summe aller Queue-Rejections. */
    uint32_t queue_wait_avg_ticks; /**< Avg. wait time (denominator: summed queue_wait_entered_count; fallback 0 if no samples). */
    uint32_t queue_wait_max_ticks; /**< Maximum wait time in the whole simulation. */
    float queue_active_ratio_percent; /**< Share of ticks with queue>0 in % (denominator: total_ticks). */

    /* 4) Parking duration */
    uint16_t parking_duration_avg_ticks; /**< Avg. parking duration (denominator: departed duration sample count; fallback 0 if none). */

    /* 6) Blockers / cause analysis */
    float blocker_full_ratio_percent; /**< Share of ticks where "full" blocker was active (denominator: total_ticks). */

    /* 8) Quality/rule statistics */
    uint64_t bad_parking_cases_total; /**< Total number of "badly parked" cases. */
    float bad_parking_share_percent; /**< Share of "badly parked" in % (denominator: entered_total; fallback 0 if entered_total=0). */

    /* 5) ADD-ON analysis of individual gates & vehicle types */

};

/**
 * Statistics container for tick progression and cumulative totals.
 * @author: ibach
 */
struct StatList {
    SimulationObject base;
    StatsTick *p_tick_head; /**< Erster Tick in der Verlaufsliste. */
    StatsTick *p_tick_tail; /**< Letzter Tick in der Verlaufsliste. */
    StatsTick *p_current_tick; /**< Tick-Builder fuer den aktuell laufenden Tick. */
};


#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_TYPES_H
