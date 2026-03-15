#include "Simulation.h"

#include <stdlib.h>

#include "Parkhaus.h"
#include "utils/SafteyUtils.h"
#include "utils/RNG.h"
#include "Queue.h"
#include "Stats.h"
#include "io/SaveHandler.h"
#include "utils/demand.h"
#include "utils/gate_routing.h"
#include "utils/StatList.h"

int simulation_init(Simulation *p_sim, const Settings *p_settings, StatList *p_StatList) {
    if (checkNull(p_sim) || checkNull(p_settings)) {
        print_error_s("central pointer error", HIGH);
        return ERROR;
    }

    p_sim->settings = (Settings *) p_settings;
    p_sim->StatList = (StatList *) p_StatList;
    p_sim->current_tick = 0;

    int status = OK;

    status = rng_init(p_sim->settings);
    if (status == ERROR)
    {
        return ERROR;
    }

    if (p_sim->StatList == NULL)
    {
        p_sim->StatList = StatList_init(p_sim);
        if (p_sim->StatList == NULL)
        {
            print_warning_s(" StatList_init failed");
            return ERROR;
        }
    }


    //erstellen der Queues
    Queue **gate_queues = calloc(p_sim->settings->gates, sizeof(Queue *));
    if (gate_queues == NULL) {
        simulation_cleanup_children(p_sim);
        return ERROR;
    }
    //erstellen der einzelnen queues
    for (uint32_t i = 0; i < p_sim->settings->gates; ++i) {
        gate_queues[i] = calloc(1U, sizeof(Queue));

        if (gate_queues[i] == NULL) {

            for (uint32_t j = 0; j < i; ++j) {

                queue_free(gate_queues[j]);
                free(gate_queues[j]);
            }
            free(gate_queues);
            simulation_cleanup_children(p_sim);
            return ERROR;
        }
        //initialisierung der einzelnen queues
        //for now default is used of the length
        if (queue_init(gate_queues[i], DEFAULT_MAX_QUEUE_LENGTH) != OK) {

            for (uint32_t j = 0; j <= i; ++j) {

                if (gate_queues[j] != NULL) {

                    queue_free(gate_queues[j]);
                    free(gate_queues[j]);
                }
            }
            free(gate_queues);
            simulation_cleanup_children(p_sim);
            return ERROR;
        }
    }

    //alocation of Parkhouse
    p_sim->parkhouse = calloc(1U, sizeof(Parkhaus));
    if (p_sim->parkhouse == NULL) {

        free(gate_queues);
        simulation_cleanup_children(p_sim);
        return ERROR;
    }
    //Parkhaus initialisierung
    if (parkhouse_init(p_sim->parkhouse, p_sim->settings, gate_queues) != OK) {
        for (uint32_t i = 0; i < p_sim->settings->gates; ++i) {
            queue_free(gate_queues[i]);
            free(gate_queues[i]);
        }
        free(gate_queues);
        free(p_sim->parkhouse);
        p_sim->parkhouse = NULL;
        simulation_cleanup_children(p_sim);
        return ERROR;
    }

    return OK;
}


//FIXME LUCA IMPLEMENT
int simulation_tick(Simulation *p_sim) {
    if (checkNull(p_sim) || checkNull(p_sim->StatList) || checkNull(p_sim->parkhouse) || checkNull(p_sim->settings)) {
        return ERROR;
    }

    p_sim->current_tick++;
    if (StatsTick_init(p_sim, p_sim->parkhouse->capacity, p_sim->current_tick) == ERROR) {
        return ERROR;
    }
    int totaldemand = demand_generate_total_perTick(p_sim->settings);
    GateRouting_DistributeTotalDemand(p_sim->settings, totaldemand, p_sim->parkhouse->gate_queues, p_sim->current_tick);

    if (parkhouse_tick((SimulationObject*) p_sim->parkhouse, p_sim->settings, p_sim->StatList, p_sim->current_tick) == ERROR) {
        return ERROR;
    }

    if (stats_tick_set_capacity(p_sim->StatList,
                             (uint16_t)p_sim->parkhouse->capacity_taken,
                             get_open_space(p_sim->parkhouse)) != OK) {
        return ERROR;
                             }

    uint32_t queue_length_end = 0U;
    for (uint32_t gate = 0U; gate < p_sim->settings->gates; ++gate) {
        if (p_sim->parkhouse->gate_queues[gate] != NULL) {
            queue_length_end += queue_length(p_sim->parkhouse->gate_queues[gate]);
        }
    }

    if (p_sim->StatList->p_current_tick != NULL) {
        if (queue_length_end > UINT8_MAX) {
            p_sim->StatList->p_current_tick->queue_length_end = UINT8_MAX;
        } else {
            p_sim->StatList->p_current_tick->queue_length_end = (uint8_t)queue_length_end;
        }

        if (savehandler_save_tick(p_sim, p_sim->StatList->p_current_tick, NULL) != OK) {
            return ERROR;
        }
    }

    return OK;
}

int simulation_start(Simulation *p_sim) {
    if (checkNull(p_sim) || checkNull(p_sim->settings) || checkNull(p_sim->parkhouse) || checkNull(p_sim->StatList)) {
        return ERROR;
    }
    p_sim->current_tick = 0U;
    if (savehandler_init_stats_file(p_sim, NULL) != OK) {
        print_warning_s("Failed to initialize stats file. Logging may fail.");
    }
    if (simulation_run(p_sim) != OK)
    {
        print_warning_s("Simulation failed");
        return ERROR;
    }


    return OK;
}

int simulation_run(Simulation *p_sim) {
    if (checkNull(p_sim) || checkNull(p_sim->settings) || checkNull(p_sim->parkhouse) || checkNull(p_sim->StatList)) {
        return ERROR;
    }


    if (p_sim->settings->max_ticks < 0) {
        print_error_s("invalid max_ticks", LOW);
        return ERROR;
    }

    for (uint32_t runs = 0; runs < p_sim->settings->max_ticks; runs++) {
        if (simulation_tick(p_sim) == ERROR) {
            return ERROR;
        }
    }

    return OK;
}

void simulation_end(Simulation *p_sim) {
    if (p_sim == NULL) {
        return;
    }

    if (p_sim->StatList != NULL && p_sim->StatList->p_summary != NULL) {
        StatsSummary summary;
        if (stats_build_summary(p_sim->StatList, p_sim->StatList->p_summary) == OK) {
            savehandler_save_summary(p_sim, p_sim->StatList->p_summary, NULL);
        }
    }
}


int free_simulation(Simulation *p_sim) {
    if (checkNull(p_sim)) {
        print_warning_s("pointer error");
        return ERROR;
    }

    simulation_cleanup_children(p_sim);
    free(p_sim);
    return OK;
}


static void simulation_cleanup_children(Simulation *p_sim) {
    if (p_sim == NULL) {
        return;
    }

    if (p_sim->parkhouse != NULL) {

        if (p_sim->parkhouse->gate_queues != NULL && p_sim->settings != NULL) {

            for (uint32_t i = 0; i < p_sim->settings->gates; ++i) {

                if (p_sim->parkhouse->gate_queues[i] != NULL) {

                    queue_free(p_sim->parkhouse->gate_queues[i]);
                    free(p_sim->parkhouse->gate_queues[i]);
                    p_sim->parkhouse->gate_queues[i] = NULL;

                }
            }
            free(p_sim->parkhouse->gate_queues);
            p_sim->parkhouse->gate_queues = NULL;
        }
        if (p_sim->StatList != NULL) {
            StatList_free(p_sim->StatList);
            p_sim->StatList = NULL;
        }
        parkhouse_free(p_sim->parkhouse);
        free(p_sim->parkhouse);
        p_sim->parkhouse = NULL;
    }

}
