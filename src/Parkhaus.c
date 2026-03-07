#include "Parkhaus.h"

#include <stdlib.h>
#include <sys/stat.h>

#include "utils/SafteyUtils.h"
#include "GenericVehicle.h"
#include "Stats.h"

int parkhaus_init(Parkhaus *p_parkhaus, const Settings *p_settings, Queue **p_gate_queues) {
    if (p_parkhaus == NULL || p_settings == NULL || p_gate_queues == NULL) {
        return ERROR;
    }
    p_parkhaus->name = p_settings->name;
    p_parkhaus->capacity = p_settings->capacity;
    p_parkhaus->floors = p_settings->floors;
    p_parkhaus->capacity_taken = 0;
    p_parkhaus->gate_queues = p_gate_queues;
    p_parkhaus->p_parked_head = NULL;
    p_parkhaus->p_parked_tail = NULL;
    return OK;
}
//FIXME Ist hier nicht eine p_parkhaus pointer nötig anstelle des SimulaionObjecct da es in Parkhaus abgelegt ist?
int parkhaus_tick(SimulationObject *p_self, const Settings *p_settings, StatList *p_StatList, uint32_t current_tick) {

    if (p_self == NULL || p_settings == NULL) {
        return ERROR;
    }
    Parkhaus *p_parkhaus = (Parkhaus *)p_self;

    p_parkhaus = (Parkhaus *) p_self;
    if (p_self->type != PARKHAUS) {
        return ERROR;
    }
    int status = OK;

    //pre checking Parkhaus initialisation & conditons
    if (p_settings->gates == 0U || p_parkhaus->gate_queues == NULL) {
        return OK;
    }

    //Allgemeines leeren des Parkhaus
    status = parkhouse_tick_empty_general(current_tick, p_parkhaus,
                                        (Settings *) p_settings,
                                        p_StatList,
                                        &p_parkhaus->p_parked_head);
    if (status == ERROR)
    {
        print_error("parkhaus_tick_empty_general: ERROR in Simulation");
        return ERROR;
    }

    //Allemeine Fill Funktion sofern nur ein Gate besteht
    if (p_settings->gates == 1U) {
        status = parkhouse_tick_fill_general(current_tick,
                                            p_parkhaus,
                                            (Settings *) p_settings,
                                            p_StatList,
                                            &p_parkhaus->p_parked_head,
                                            p_parkhaus->gate_queues[0]);
        if (status == ERROR)
        {
            print_error("parkhaus_tick_fill_general: ERROR in Simulation");
            return ERROR;
        }
    }
    else
    {
        status =  parkhouse_fill_subtick(current_tick,
                                        p_parkhaus, (Settings *) p_settings,
                                        p_StatList, NULL);
        if (status == ERROR)
        {
            print_error("parkhouse_fill_subtick: ERROR in Simulation");
            return ERROR;
        }
    }

    return ERROR;
}

int parkhouse_tick_empty_general(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings,
                                 StatList *p_StatList, GenericVehicle **pp_vehicle_list_head) {
    GenericVehicle *p_vehicle;

    if (p_parkhouse == NULL || pp_vehicle_list_head == NULL) {
        return ERROR;
    }

    p_vehicle = *pp_vehicle_list_head;
    while (p_vehicle != NULL) {
        GenericVehicle *p_next = p_vehicle->p_next;
        uint32_t leave_tick = p_vehicle->created_at_tick + p_vehicle->leaving_in_ticks;

        if (current_tick >= leave_tick) {
            if (vehicle_leaving(p_parkhouse, p_StatList, pp_vehicle_list_head, p_vehicle, current_tick) == ERROR) {
                print_error("parkhaus_tick_empty_general: Vehicle_leaving ERROR");
                return ERROR;
            }
        }
        p_vehicle = p_next;
    }

    return OK;
}

//FIXME IMPLEMET
int parkhouse_tick_fill_general(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, GenericVehicle **pp_vehicle_list_head, Queue *p_gate_queue) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) pp_vehicle_list_head;
    (void) p_gate_queue;
    return OK;
}

//FIXME IMPLEMET
int parkhouse_fill_subtick(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, Queue *p_gate_queues) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) p_gate_queues;
    return OK;
}

//FIXME IMPLEMET
int parkhouse_fill_subtick_routine(uint32_t current_tick, Parkhaus *p_parkhouse, Settings *p_settings, StatList *p_StatList, Queue *p_gate_queue, int last_cycle) {
    (void) current_tick;
    (void) p_parkhouse;
    (void) p_settings;
    (void) p_StatList;
    (void) p_gate_queue;
    (void) last_cycle;
    return OK;
}

int vehicle_leaving(Parkhaus *p_parkhouse, StatList *p_StatList, GenericVehicle **pp_vehicle_list_head, GenericVehicle *p_vehicle, uint32_t current_tick) {
    GenericVehicle *p_prev = NULL;
    GenericVehicle *p_cur;
    uint16_t required_space;
    int status = OK;

    if (p_parkhouse == NULL || pp_vehicle_list_head == NULL || p_vehicle == NULL) {
        return ERROR;
    }

    p_cur = *pp_vehicle_list_head;
    while (p_cur != NULL && p_cur != p_vehicle) {
        p_prev = p_cur;
        p_cur = p_cur->p_next;
    }
    //no vehicle found
    if (p_cur == NULL) {
        print_error("vehicle_leaving: vehicle not found");
        return ERROR;
    }

    //Vehicle found -> vehicle ist head -> nächstes Vehicle als head zuweisen
    if (p_prev == NULL) {
        *pp_vehicle_list_head = p_cur->p_next;
    } else {
        p_prev->p_next = p_cur->p_next; //Vehicle davor als nächstes das Vehicle nach Current zuweisen
    }

    //vehicle = parkhaus tail -> parkhaus.tail <- prev vehicle
    if (p_parkhouse->p_parked_tail == p_cur) {
        p_parkhouse->p_parked_tail = p_prev;
    }
    if (p_parkhouse->p_parked_head == p_cur) {
        p_parkhouse->p_parked_head = p_cur->p_next;
    }

    //updating Parkhouse and Stats
    required_space = get_vehicle_minimum_space(p_vehicle);
    if (get_vehicle_space_needed(p_vehicle) >= required_space) {
        required_space = get_vehicle_space_needed(p_vehicle);
    }
    status = update_on_vehicle_exit(p_parkhouse, p_StatList, p_cur, required_space, current_tick);
    free(p_cur);

    if (p_parkhouse->p_parked_head == NULL) {
        p_parkhouse->p_parked_tail = NULL;
    }

    return status;
}

//FIXME IMPLEMET
uint16_t fill_from_queue(Parkhaus *p_parkhaus, Queue *p_gate_queue, GenericVehicle **pp_vehicle) {
    (void) p_parkhaus;
    (void) p_gate_queue;
    (void) pp_vehicle;
    return 0;
}

//FIXME IMPLEMET
int open_demand(StatList *p_StatList, Queue *p_gate_queue, uint16_t queue_max_len, uint16_t demand_remaining, uint32_t current_tick, Settings *p_settings) {
    (void) p_StatList;
    (void) p_gate_queue;
    (void) queue_max_len;
    (void) demand_remaining;
    (void) current_tick;
    (void) p_settings;
    return OK;
}

//FIXME IMPLEMET
Queue *parkhaus_create_gate_queues(uint32_t number_of_gates) {
    (void) number_of_gates;
    return NULL;
}

//FIXME IMPLEMET
int parkhaus_enqueue_at_gate(Queue *p_gate_queues, uint32_t gate_index, GenericVehicle *p_vehicle) {
    (void) p_gate_queues;
    (void) gate_index;
    (void) p_vehicle;
    return OK;
}

//FIXME IMPLEMET
int parkhaus_set_gate_demand(Queue *p_gate_queues, uint32_t gate_index, uint16_t demand_value) {
    (void) p_gate_queues;
    (void) gate_index;
    (void) demand_value;
    return OK;
}

//FIXME IMPLEMET
int queue_add_random_vehicle(Queue *p_gate_queue, uint32_t current_tick, Settings *p_settings) {
    (void) p_gate_queue;
    (void) current_tick;
    (void) p_settings;
    return OK;
}

//FIXME IMPLEMET
GenericVehicle *create_random_vehicle(uint32_t current_tick, Settings *p_settings) {
    (void) current_tick;
    (void) p_settings;
    return NULL;
}

//FIXME IMPLEMET
int park_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle) {
    if (p_parkhaus == NULL || p_vehicle == NULL) {
        return ERROR;
    }

    p_vehicle->p_next = NULL;

    if (p_parkhaus->p_parked_tail == NULL) {
        p_parkhaus->p_parked_head = p_vehicle;
        p_parkhaus->p_parked_tail = p_vehicle;
        return OK;
    }

    p_parkhaus->p_parked_tail->p_next = p_vehicle;
    p_parkhaus->p_parked_tail = p_vehicle;
    return OK;
}
//FIXME IMPLEMET
uint16_t get_open_space(const Parkhaus *p_parkhouse) {
    if (p_parkhouse == NULL || p_parkhouse->capacity_taken >= p_parkhouse->capacity) {
        return 0;
    }

    return (uint16_t) (p_parkhouse->capacity - p_parkhouse->capacity_taken);
}

//Parkhaus & Vehicle & Statistik update bei verlassen des Parkhaus
int update_on_vehicle_exit(Parkhaus *p_parkhouse, StatList *p_StatList, GenericVehicle *p_vehicle,
                            uint16_t required_space, uint32_t current_tick) {

    //GenericVehicle *p_mutable_vehicle = (GenericVehicle *) p_vehicle;

    if (p_parkhouse == NULL || p_vehicle == NULL || p_StatList == NULL) {
        print_error("update_on_vehicle_exit: pointer issue");
        return ERROR;
    }

    //left already set || car uses more space than is taken
    if (p_vehicle->park_house_left != 0U || p_parkhouse->capacity_taken < required_space)
    {
        print_error("update_on_vehicle_exit: Data Error");
        return ERROR;
    }
    else {
        p_vehicle->park_house_left = current_tick;
        p_parkhouse->capacity_taken -= required_space;
    }

    if (stats_tick_add_vehicle(p_StatList, p_vehicle, current_tick) == ERROR)
    {
        print_error("update_on_vehicle_exit: stats_tick_add_vehicle failed");
        return ERROR;
    };

    return OK;
}
//Parkhaus & Statistik update bei betreten des Parkhaus
int update_on_vehicle_entry(Parkhaus *p_parkhouse, StatList *p_StatList, GenericVehicle *p_vehicle,
                             uint16_t required_space, uint32_t current_tick)
{
    //GenericVehicle *p_mutable_vehicle = (GenericVehicle *) p_vehicle;

    if (p_parkhouse == NULL || p_vehicle == NULL || p_StatList == NULL) {
        print_error("update_on_vehicle_entry: pointer issue");
        return ERROR;
    }

    p_vehicle->park_house_entered = current_tick;

    if (p_parkhouse->capacity_taken + required_space > p_parkhouse->capacity) {
        print_error("update_on_vehicle_entry: needed space exceeds capacity");
        return ERROR;
    }
    else {
        p_parkhouse->capacity_taken += required_space;
    }

    if (stats_tick_add_vehicle(p_StatList, p_vehicle, current_tick) == ERROR)
    {
        print_error("update_on_vehicle_entry: stats_tick_add_vehicle failed");
        return ERROR;
    };
}

//FIXME IMPLEMET
int parkhaus_park_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle) {
    return park_vehicle(p_parkhaus, p_vehicle);
}

//FIXME IMPLEMET
int parkhaus_remove_vehicle(Parkhaus *p_parkhaus, GenericVehicle *p_vehicle) {
    if (p_parkhaus == NULL || p_vehicle == NULL) {
        return ERROR;
    }

    (void) p_parkhaus;
    return OK;
}

//FIXME IMPLEMET
float parkhaus_get_utilization(const Parkhaus *p_parkhaus) {
    if (p_parkhaus == NULL || p_parkhaus->capacity == 0) {
        return 0.0f;
    }

    return ((float) p_parkhaus->capacity_taken * 100.0f) / (float) p_parkhaus->capacity;
}

//FIXME IMPLEMET
void parkhaus_free(Parkhaus *p_parkhaus) {
    if (p_parkhaus == NULL) {
        return;
    }

    p_parkhaus->p_parked_head = NULL;
    p_parkhaus->p_parked_tail = NULL;
    p_parkhaus->gate_queues = NULL;
}

