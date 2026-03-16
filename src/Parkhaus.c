#include "Parkhaus.h"

#include <stdbool.h>
#include <stdlib.h>
#include "utils/SafteyUtils.h"
#include "GenericVehicle.h"
#include "Stats.h"
#include "Queue.h"
#include "utils/RNG.h"
#include "Car.h"

int parkhouse_init(Parkhaus *p_parkhouse, const Settings *p_settings, Queue **p_gate_queues) {
    if (p_parkhouse == NULL || p_settings == NULL || p_gate_queues == NULL) {
        return ERROR;
    }
    p_parkhouse->base.type = PARKHAUS;
    p_parkhouse->capacity = p_settings->capacity;
    p_parkhouse->floors = p_settings->floors;
    p_parkhouse->capacity_taken = 0;
    p_parkhouse->gate_queues = p_gate_queues;
    p_parkhouse->p_parked_head = NULL;
    p_parkhouse->p_parked_tail = NULL;
    return OK;
}

int parkhouse_tick(SimulationObject* p_self, const Settings* p_settings, StatList* p_StatList, uint32_t current_tick)
{
    if (p_self == NULL || p_settings == NULL || p_StatList == NULL) {
        print_error("parkhouse_tick: central Pointer ERROR");
        return ERROR;
    }
    Parkhaus *p_parkhouse = (Parkhaus *)p_self;

    p_parkhouse = (Parkhaus *) p_self;
    if (p_self->type != PARKHAUS) {
        return ERROR;
    }
    int status = OK;

    //pre checking Parkhaus initialisation & conditons
    if (p_settings->gates == 0U || p_parkhouse->gate_queues == NULL) {
        return ERROR;
    }

    //Allgemeines leeren des Parkhaus
    status = parkhouse_tick_empty_general(current_tick, p_parkhouse,
                                          (Settings*)p_settings,
                                          p_StatList,
                                          &p_parkhouse->p_parked_head);
    if (status == ERROR)
    {
        print_error("parkhouse_tick_empty_general: ERROR in Simulation");
        return ERROR;
    }

    //Allemeine Fill Funktion sofern nur ein Gate besteht
    if (p_settings->gates == 1U) {
        status = parkhouse_tick_fill_general(current_tick,
                                            p_parkhouse,
                                            (Settings *) p_settings,
                                            p_StatList,
                                            &p_parkhouse->p_parked_head,
                                            p_parkhouse->gate_queues[0]);
        if (status == ERROR)
        {
            print_error("parkhouse_tick_fill_general: ERROR in Simulation");
            return ERROR;
        }
    }
    else
    {
        status = parkhouse_fill_subtick(current_tick,
                                        p_parkhouse, (Settings*)p_settings,
                                        p_StatList, *p_parkhouse->gate_queues);
        if (status == ERROR)
        {
            print_error("parkhouse_fill_subtick: ERROR in Simulation");
            return ERROR;
        }
    }

    return OK;
}

int parkhouse_tick_empty_general(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                                 StatList *p_StatList, GenericVehicle **pp_vehicle_list_head) {
    GenericVehicle *p_vehicle;

    if (p_parkhouse == NULL || pp_vehicle_list_head == NULL || p_settings == NULL || p_StatList == NULL) {
        print_error("parkhouse_tick_empty_general: central Pointer ERROR");
        return ERROR;
    }

    p_vehicle = *pp_vehicle_list_head;
    while (p_vehicle != NULL) {
        GenericVehicle *p_next = p_vehicle->p_next;
        uint32_t leave_tick = p_vehicle->created_at_tick + p_vehicle->leaving_in_ticks;

        if (current_tick >= leave_tick) {
            if (vehicle_leaving(p_parkhouse, p_StatList, pp_vehicle_list_head, p_vehicle, current_tick) == ERROR) {
                print_error("parkhouse_tick_empty_general: Vehicle_leaving ERROR");
                return ERROR;
            }
        }
        p_vehicle = p_next;
    }

    return OK;
}


int parkhouse_tick_fill_general(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                                StatList* p_StatList,
                                GenericVehicle **pp_vehicle_list_head, Queue *p_gate_queue) {

    bool queue_blocked = false;
    int status = OK;

    if (p_parkhouse == NULL || p_settings == NULL || p_gate_queue == NULL || p_StatList == NULL || p_gate_queue == NULL) {
        print_error("parkhouse_tick_fill_general: central Pointer ERROR");
        return ERROR;
    }

    //demand for this Tick for this queue
    uint16_t newDemand = 0;
    newDemand = p_gate_queue->demand; //or queue_get_demand(p_gate_queue);
    if (newDemand == 0U && queue_length(p_gate_queue) == 0) {
        return OK;
    }

    //anz. der möglichen Entrys pro Tick
    uint16_t entries_limit = p_settings->real_equivalent / p_settings->gate_entry_inSec;
    uint16_t entries_done = 0;

    //Entry Cycle
    while (newDemand > 0U && entries_done < entries_limit && !queue_blocked) {
        GenericVehicle* p_vehicle = NULL;
        uint16_t required_space = 0;

        // wenn queue leer -> neues vehicle generiern & anhängen
        if (queue_is_empty(p_gate_queue)) {
            status = queue_add_random_vehicle(p_gate_queue, current_tick, p_settings);
            if (status == ERROR)
            {
                print_error("parkhouse_tick_fill_general: queue_add_random_vehicle: can't add to Queue");
                return ERROR;
            }
            //reduction of new Demand bc of filling queue up
            newDemand--;
        }

        if (!queue_is_empty(p_gate_queue))
        {

            //check if theres enough space left
            required_space = get_vehicle_minimum_space(p_gate_queue->p_head);
            if (required_space > get_open_space(p_parkhouse)) {
                stats_tick_add_blocker_full_active(p_StatList);
                queue_blocked = true;
            }
            else
            {
                required_space = fill_from_queue(p_parkhouse, p_gate_queue, &p_vehicle);
                update_on_vehicle_entry(p_parkhouse, p_StatList, p_vehicle, required_space, current_tick);

                //entry from queue
                entries_done++;
            }
        }
    }

    queue_set_demand(p_gate_queue, newDemand);
    if (newDemand > 0U) {
        status = open_demand(p_StatList, p_gate_queue, newDemand, current_tick,  p_settings);
        if (status == ERROR)
        {
            return ERROR;
        }
        queue_set_demand(p_gate_queue, 0);
    }
    return OK;
}

int parkhouse_fill_subtick(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                           StatList* p_StatList,
                           Queue* p_gate_queues)
{
    //checking for valid function call
    if (p_parkhouse == NULL || p_settings == NULL || p_StatList == NULL)
    {
        print_error_s("parkhouse_tick_fill_subtick: central Pointer ERROR", HIGH);
        return ERROR;
    }
    if (p_parkhouse->gate_queues == NULL || p_settings->gates <= 1U)
    {
        print_warning_s("Wrong use of function");
        return ERROR;
    }
    if ((p_settings->real_equivalent % p_settings->gate_entry_inSec) != 0)
    {
        print_warning_s("Tick in seconds is not compatible with gate_entry in seconds");
        return ERROR;
    }

    uint8_t gate;
    uint16_t subticks = p_settings->real_equivalent / p_settings->gate_entry_inSec;
    //this represents the max entries per tick

    //Adding total demand of all queues to StatsTick
    uint16_t total = 0;
    for (gate = 0; gate < p_settings->gates; gate++)
    {
        if (p_parkhouse->gate_queues[gate] != NULL)
        {
            total = total + queue_get_demand(p_parkhouse->gate_queues[gate]);
        }
    }
    if (total > 0U)
    {
        stats_tick_add_arrivals_generated(p_StatList, total);
    }


    for (uint16_t cycle = 0; cycle < subticks; cycle++)
    {
        const int last_cycle = (cycle == (uint16_t)(subticks - 1U));
        for (gate = 0; gate < p_settings->gates; gate++)
        {
            Queue* p_gate_queue = p_parkhouse->gate_queues[gate];
            if (p_gate_queue == NULL)
            {
                print_warning_s("Missing queue");
                return ERROR;
            }

            int status = parkhouse_fill_subtick_routine(current_tick, p_parkhouse, p_settings, p_StatList,
                                                        p_gate_queue, last_cycle);
            if (status == ERROR)
            {
                print_warning_s("parkhouse_fill_subtick_routine returned error");
                return ERROR;
            }
        }
    }
    return OK;
}

int parkhouse_fill_subtick_routine(uint32_t current_tick, Parkhaus* p_parkhouse, const Settings* p_settings,
                                   StatList* p_StatList, Queue* p_gate_queue, int last_cycle)
{
    int status = OK;

    if (p_parkhouse == NULL || p_settings == NULL || p_gate_queue == NULL || p_StatList == NULL)
    {
        print_error("parkhouse_tick_fill_subtick: central Pointer ERROR");
        return ERROR;
    }

    uint16_t demand = queue_get_demand(p_gate_queue);
    if (demand <= 0U)
    {
        print_warning_s("NO demnad there, LOW");
        return OK;
    }

    if (queue_is_empty(p_gate_queue))
    {
        status = queue_add_random_vehicle(p_gate_queue, current_tick, p_settings);
        if (status == ERROR)
        {
            print_error("parkhouse_tick_fill_general: queue_add_random_vehicle: can't add to Queue");
            return ERROR;
        }
        demand--;
    }

    GenericVehicle* p_vehicle = NULL;
    uint16_t required_space;
    if (!queue_is_empty(p_gate_queue))
    {
        //check if theres enough space left
        required_space = get_vehicle_minimum_space(p_gate_queue->p_head);
        if (required_space > get_open_space(p_parkhouse))
        {
            stats_tick_add_blocker_full_active(p_StatList);
        }
        else
        {
            required_space = fill_from_queue(p_parkhouse, p_gate_queue, &p_vehicle);
            update_on_vehicle_entry(p_parkhouse, p_StatList, p_vehicle, required_space, current_tick);
            queue_set_demand(p_gate_queue, required_space);
            demand--;
        }
    }

    queue_set_demand(p_gate_queue, demand);

    if (last_cycle && demand > 0U)
    {
        if (open_demand(p_StatList, p_gate_queue, demand, current_tick, p_settings) == ERROR)
        {
            print_warning_s("open_demand: returned error");
            return ERROR;
        }
        queue_set_demand(p_gate_queue, 0);
    }

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
    //going through the list an searching for the correct vehicle
    p_cur = *pp_vehicle_list_head;
    while (p_cur != NULL && p_cur != p_vehicle) {
        p_prev = p_cur;
        p_cur = p_cur->p_next;
    }
    //no vehicle found in list
    if (p_cur == NULL)
    {
        //deleting lost car
        remove_vehicle(p_vehicle);
        print_error("vehicle_leaving: vehicle not found in parkhouse - vehicle destroyed");
        return ERROR;
    }

    //Vehicle found -> vehicle ist head -> nächstes Vehicle als head zuweisen
    if (p_prev == NULL)
    {
        *pp_vehicle_list_head = p_cur->p_next;
    }
    else
    {
        p_prev->p_next = p_cur->p_next; //Vehicle davor als nächstes das Vehicle nach Current zuweisen
    }

    //vehicle = parkhouse tail -> parkhouse.tail <- prev vehicle
    if (p_parkhouse->p_parked_tail == p_cur) {
        p_parkhouse->p_parked_tail = p_prev;
    }
    if (p_parkhouse->p_parked_head == p_cur) {
        p_parkhouse->p_parked_head = p_cur->p_next;
    }

    //updating Parkhouse and Stats
    required_space = get_vehicle_space_needed(p_vehicle);

    status = update_on_vehicle_exit(p_parkhouse, p_StatList, p_cur, required_space, current_tick);
    if (status == ERROR)
    {
        print_error("vehicle_leaving: update_on_vehicle_exit: vehicle exited with error");
    }

    status = remove_vehicle(p_vehicle);
    if (status == ERROR)
    {
        print_error("remove_leaving: remove_vehicle: vehicle exited with error");
    }

    if (p_parkhouse->p_parked_head == NULL) {
        p_parkhouse->p_parked_tail = NULL;
    }

    return status;
}


uint16_t fill_from_queue(Parkhaus *p_parkhouse, Queue *p_gate_queue, GenericVehicle **pp_vehicle) {
    Car *p_car;
    uint16_t open_space;
    uint16_t minimum;
    uint16_t spaces_needed;
    int status = OK;

    if (p_parkhouse == NULL || p_gate_queue == NULL || pp_vehicle == NULL) {
        print_error("fill_from_queue: central pointer error");
        return ERROR;
    }
    //loading the Vehicle
    GenericVehicle *p_vehicle = queue_get_next(p_gate_queue);
    if (p_vehicle == NULL) {
        *pp_vehicle = NULL;
        print_error("fill_from_queue: queue_dequeue: vehicle pointer missing");
        return ERROR;
    }
    // Switch (p_vehicle.type)
    p_car = (Car*)p_vehicle;

    //minimum für bessere verständlichkeit
    minimum = get_vehicle_minimum_space(p_vehicle);
    open_space = get_open_space(p_parkhouse);
    spaces_needed = minimum;

    //does the vehicle fit in?
    if (spaces_needed <= open_space)
    {
        //deleting the vehicle from the queue after confirmation of fitting
        status = queue_dequeue(p_gate_queue);
        if (status == ERROR) { print_error("fill_from_queue: dequeue error"); }

        //can the vehicle even "park bad" & probability
        if (open_space >= (minimum * 2U) && rng_percent() <= BAD_PARKING_CHANCE_PERCENT) {
            spaces_needed = (minimum * 2U);
        }
        p_car->spaces_needed = spaces_needed;

        //adding the vehicle to parkhouse queue
        status = park_vehicle(p_parkhouse, p_vehicle);

        if (status == ERROR)
        {
            print_error("fill_from_queue: park error");
            free(p_vehicle);
            *pp_vehicle = NULL;
            return ERROR;

        }
    }

    *pp_vehicle = p_vehicle;
    return spaces_needed;
}

//moving left demand into queue or add to rejections
int open_demand(StatList *p_StatList, Queue *p_gate_queue, uint16_t demand_remaining,
                uint32_t current_tick, const Settings *p_settings){
    if (p_StatList == NULL || p_StatList->p_current_tick == NULL || p_gate_queue == NULL || p_settings == NULL) {
        print_error("open_demand: central pointer error");
        return ERROR;
    }
    uint16_t openDemand = demand_remaining;
    while (openDemand > 0U && queue_length(p_gate_queue) < p_gate_queue->max_size) {
        if (queue_add_random_vehicle(p_gate_queue, current_tick, p_settings) == ERROR) {
            print_error("open_demand: queue_add_random_vehicle error");
            return ERROR;
        }
        openDemand--;
        p_StatList->p_current_tick->enqueued += 1U;
    }

    if (openDemand > 0U) {
        stats_tick_add_queue_rejections(p_StatList, openDemand);
    }

    return OK;
}


int queue_add_random_vehicle(Queue* p_gate_queue, uint32_t current_tick, const Settings *p_settings) {


    if (p_gate_queue == NULL || p_settings == NULL) {
        print_error("queue_add_random_vehicle: central pointer error");
        return ERROR;
    }

    GenericVehicle* p_vehicle = create_random_vehicle(current_tick, p_settings);
    if (p_vehicle == NULL) {
        print_error("queue_add_random_vehicle: create_random_vehicle: ERROR");
        return ERROR;
    }

    if (queue_enqueue(p_gate_queue, p_vehicle) == ERROR) {
        free(p_vehicle);
        print_error("queue_add_random_vehicle: queue_enqueue: ERROR");
        return ERROR;
    }

    return OK;
}

//vorerst ausschließlich car
GenericVehicle* create_random_vehicle(uint32_t current_tick, const Settings *p_settings) {

    if (p_settings == NULL) {
        print_error("create_random_vehicle: central pointer error");
        return NULL;
    }

    uint32_t parking_ticks = rng_parking_time(p_settings->min_parking_ticks, p_settings->max_parking_ticks);

    // Switch (p_vehicle.type) + warscheinlichkeits auswahl
    Car* p_car = car_create(current_tick, parking_ticks, Car_Space);
    GenericVehicle *p_vehicle = (GenericVehicle *) p_car;


    if (p_vehicle == NULL)
    {
        print_error("create_random_vehicle: car_create: ERROR");
        return NULL;
    }

    return p_vehicle;
}

//parking / enqueueing new Vehicles at Parkhaus
int park_vehicle(Parkhaus *p_parkhouse, GenericVehicle *p_vehicle) {
    if (p_parkhouse == NULL || p_vehicle == NULL) {
        print_error("park_vehicel: central pointer error");
        return ERROR;
    }

    p_vehicle->p_next = NULL;

    if (p_parkhouse->p_parked_head == NULL)
    {
        p_parkhouse->p_parked_head = p_vehicle;
        return OK;
    }

    if (p_parkhouse->p_parked_tail == NULL)
    {
        p_parkhouse->p_parked_head->p_next = p_vehicle;
        p_parkhouse->p_parked_tail = p_vehicle;
        return OK;
    }

    if (p_parkhouse->p_parked_tail->p_next == NULL)
    {
        p_parkhouse->p_parked_tail->p_next = p_vehicle;
        p_parkhouse->p_parked_tail = p_vehicle;
        return OK;
    }
    else
    {
        print_error("park_vehicle: Parking-Vehicle-List is corrupted");
        return ERROR;
    }

    return OK;
}

uint16_t get_open_space(const Parkhaus *p_parkhouse) {
    if (p_parkhouse == NULL || p_parkhouse->capacity_taken > p_parkhouse->capacity) {
        print_error_s("get_open_space: pointer issue OR capacity_taken > capacity",HIGH);
        return ERROR;
    }

    return (uint16_t) (p_parkhouse->capacity - p_parkhouse->capacity_taken);
}

//Parkhaus & Vehicle & Statistik update bei verlassen des Parkhaus
int update_on_vehicle_exit(Parkhaus* p_parkhouse, StatList* p_StatList, GenericVehicle* p_vehicle,
                           uint16_t required_space, uint32_t current_tick)
{
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
    p_vehicle->park_house_left = current_tick;
    p_parkhouse->capacity_taken -= required_space;

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
    p_parkhouse->capacity_taken += required_space;

    if (stats_tick_add_vehicle(p_StatList, p_vehicle, current_tick) == ERROR)
    {
        print_error("update_on_vehicle_entry: stats_tick_add_vehicle failed");
        return ERROR;
    };
    return OK;
}


int remove_vehicle(GenericVehicle* p_vehicle)
{
    if (p_vehicle == NULL)
    {
        print_error("remove_vehicle: pointer issue");
        return ERROR;
    }

    // Switch (p_vehicle.type) for destroying diffrent types of vehicles correctly
    Car* p_car = (Car*)p_vehicle;
    if (car_destroy(p_car) == ERROR)
    {
        print_error("parkhouse_remove_vehicle: car_destroy failed");
        return ERROR;
    }
    return OK;
}

//Freeing parkhouse parked vehicle list & queues
int parkhouse_free(Parkhaus* p_parkhouse)
{
    if (p_parkhouse == NULL)
    {
        print_error("parkhouse_free: pointer issue");
        return ERROR;
    }
    int status = 0;
    //free Cars
    if (p_parkhouse->p_parked_head == NULL || p_parkhouse->p_parked_tail == NULL)
    {
        print_error("parkhouse_free: no cars in parkhouse");
    }
    if (p_parkhouse->p_parked_head != NULL)
    {
        //Loop for removing all parked vehicles
        GenericVehicle* p_vehicle = p_parkhouse->p_parked_head;
        GenericVehicle* p_next = p_parkhouse->p_parked_head->p_next;
        while (p_vehicle != NULL)
        {
            p_next = p_vehicle->p_next;

            //remove vehicle
            status = remove_vehicle(p_vehicle);
            if (status == ERROR)
            {
                print_error("parkhouse_free: parkhouse_remove_vehicle: failed");
                return ERROR;
            }
            p_vehicle = p_next;
        }
    }
    p_parkhouse->p_parked_head = NULL;
    p_parkhouse->p_parked_tail = NULL;
    p_parkhouse->gate_queues = NULL;
    return OK;
}

