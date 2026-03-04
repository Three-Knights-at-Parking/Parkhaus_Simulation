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
    float_t capacity_taken; // Number of slots filled.
    uint32_t num_gates; // Number of gates.
    Queue **gate_queues; // array of Queue* with size = num_gates
    GenericVehicle *p_parked_head; // linked list of parked vehicles.
    GenericVehicle *p_parked_tail;
    uint16_t missed_car_entries; // How many car spawns where missed because of full queue.
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
    uint16_t gate_entry_inSec; // Time needed for an vehicle to enter der parkhouse ##UI##
	uint16_t tick_inSec; //Time in seconds of one Tick ##UI##
	uint8_t mode_select; //0 = none / 1 = normal / 2 = verbose / 3 = Error ##UI##
	// float entry_probability_car_spawn_prec; // Probability of car entering queue in the first place
	float entry_probability_perSec_prec; //probability of a Car entering per second ##UI##
    uint16_t real_equivalent; // Tick equivalent in real time (seconds), min. 10.
    enum OutputMode output_mode;
    enum QueueLeavable is_leavable; // Determines if vehicles can leave the queue early at any positions.
    int32_t max_ticks; // Max amount of ticks before the simulation stops. -1 for day equivalent. -2 for 2 day equivalent, ... ##UI##
    int32_t rand_seed; // Specified random seed, -1 if current time should be used.
};

/**
 * Momentaufnahme aller Kennzahlen am Ende eines Ticks.
 *
 * FIXME List in Simulation speichern? Oder durchschnitte direkt abspeichern?
 * @author: ibach
 */
typedef struct StatsTick {
    SimulationObject base;
    uint32_t tick; /**< Aktueller Tick dieser Momentaufnahme. */
    StatsTick *p_prev; /**< Vorheriger Tick in der Statistikliste. */
    StatsTick *p_next; /**< Naechster Tick in der Statistikliste. */

    /* Rohwerte Tick-Ende */
    uint16_t capacity_total; /**< Gesamtkapazitaet Parkhaus. */
    uint16_t capacity_taken; /**< Belegte Plaetze am Tick-Ende. */
    uint16_t capacity_free; /**< Freie Plaetze am Tick-Ende. */

    /* Rohwerte Flow */
    uint16_t arrivals_generated; /**< Demand/Ankuenfte in diesem Tick. */
    uint16_t enqueued; /**< In Queue aufgenommen in diesem Tick. */
    uint16_t entered; /**< In Parkhaus eingefahren in diesem Tick. */
    uint16_t departed; /**< Aus Parkhaus ausgefahren in diesem Tick. */

    /* Rohwerte Queue */
    uint8_t queue_length_end; /**< Queue-Laenge am Tick-Ende (global). */
    uint32_t queue_rejections; /**< Fahrzeuge, die nicht anstehen konnten (Tick). */
    uint64_t queue_wait_entered_sum_ticks; /**< Summe Wartezeit aller in diesem Tick eingetretenen Fahrzeuge. */
    uint32_t queue_wait_entered_count; /**< Anzahl in diesem Tick eingetretener Fahrzeuge fuer Wait-Auswertung. */

    /* Rohwerte Parkdauer */
    uint64_t parking_duration_departed_sum_ticks; /**< Summe Parkdauer aller in diesem Tick ausgefahrenen Fahrzeuge. */
    uint32_t parking_duration_departed_count; /**< Anzahl in diesem Tick ausgefahrener Fahrzeuge fuer Dauer-Auswertung. */

    /* Rohwerte Qualitaet/Blocker */
    uint8_t blocker_full_active; /**< 1, wenn Tick wegen voller Kapazitaet blockiert war, sonst 0. */
    uint16_t bad_parking_cases; /**< Anzahl "schlecht geparkt" in diesem Tick. */

} StatsTick;

/**
 * Aggregierte Endauswertung ueber die vollstaendige Simulation.
 *
 * Beinhaltet kumulierte Summen, Mittelwerte und Peak-Werte
 * ueber alle bereits commiteten Ticks.
 * @author: ibach
 */
struct StatsSummary {
    SimulationObject base;
    uint32_t total_ticks; /**< Anzahl ausgewerteter Ticks. */

    /* 1) Auslastung & Kapazität */
    uint16_t capacity_total; /**< Gesamtkapazitaet des Parkhauses. */
    float capacity_taken_percent_avg; /**< Durchschnittliche Auslastung über alle Ticks in %. */
    float capacity_taken_percent_peak; /**< Maximale Auslastung in %. */
    uint32_t capacity_taken_peak_tick; /**< Tick der maximalen Auslastung. */
    int32_t first_full_tick; /**< Erster Tick ohne freie Plätze, -1 wenn nie voll. */
    uint32_t full_ticks; /**< Anzahl Ticks mit voller Belegung. */

    /* 2) Durchsatz / Flow */
    uint64_t arrivals_total; /**< Kumulierte Ankünfte. */
    uint64_t enqueued_total; /**< Kumuliert in Queue aufgenommen. */
    uint64_t entered_total; /**< Kumulierte Einfahrten. */
    uint64_t departed_total; /**< Kumulierte Ausfahrten. */
    double net_occupancy_change_total; /**< Kumulierte Nettoänderung der Belegung. */
    float entered_per_tick_avg; /**< Durchschnittlich entered pro Tick. */
    float departed_per_tick_avg; /**< Durchschnittlich departed pro Tick. */

    /* 3) Queue / Warteschlange (global) */
    float queue_length_avg; /**< Durchschnittliche Queue-Laenge ueber alle Ticks. */
    uint8_t queue_length_peak; /**< Maximale Queue-Länge global. */
    uint32_t queue_length_peak_tick; /**< Tick des globalen Queue-Peaks. */
    uint64_t queue_rejections_total; /**< Summe aller Queue-Rejections. */
    uint32_t queue_wait_avg_ticks; /**< Ø-Wartezeit über alle erfolgreich eingetretenen Fahrzeuge. */
    uint32_t queue_wait_max_ticks; /**< Maximale Wartezeit in der gesamten Simulation. */
    float queue_active_ratio_percent; /**< Anteil Ticks mit Queue>0 in %. */

    /* 4) Parkdauer */
    uint16_t parking_duration_avg_ticks; /**< Ø-Parkdauer aller ausgefahrenen Fahrzeuge. */

    /* 6) Blocker / Ursachenanalyse */
    float blocker_full_ratio_percent; /**< Anteil Ticks, in denen "voll" als Blocker aktiv war. */

    /* 8) Qualitäts-/Regel-Statistiken */
    uint64_t bad_parking_cases_total; /**< Gesamtanzahl "schlecht geparkt". */
    float bad_parking_share_percent; /**< Anteil "schlecht geparkt" über die Gesamtlaufzeit in %. */

    /* 5) ADD-ON Betrachtung der einzelnen Gates & Fahrzeugtypen */

} StatsSummary;

/**
 * Statistikcontainer fuer Tick-Verlauf und kumulierte Gesamtwerte.
 * @author: ibach
 */
struct StatList {
    StatsTick *p_tick_head; /**< Erster Tick in der Verlaufsliste. */
    StatsTick *p_tick_tail; /**< Letzter Tick in der Verlaufsliste. */
    StatsTick *p_current_tick; /**< Tick-Builder fuer den aktuell laufenden Tick. */
};


#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_TYPES_H
