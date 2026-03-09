#include "Simulation.h"

#include <stdlib.h>

#include "Parkhaus.h"
#include "utils/SafteyUtils.h"
#include "utils/RNG.h"
#include "Queue.h"
#include "Stats.h"
#include "io/SaveHandler.h"

int simulation_init(Simulation *p_sim, const Settings *p_settings, StatList *p_StatList) {
    checkNull(p_sim);
    checkNull(p_settings);
    checkNull(p_StatList);

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
    //FIXME CHECK IF THIS TAYS LIKE THAT
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
    p_sim->parkhaus = calloc(1U, sizeof(Parkhaus));
    if (p_sim->parkhaus == NULL) {

        free(gate_queues);
        simulation_cleanup_children(p_sim);
        return ERROR;
    }
    //Parkhaus initialisierung
    if (parkhouse_init(p_sim->parkhaus, p_sim->settings, gate_queues) != OK) {
        for (uint32_t i = 0; i < p_sim->settings->gates; ++i) {
            queue_free(gate_queues[i]);
            free(gate_queues[i]);
        }
        free(gate_queues);
        free(p_sim->parkhaus);
        p_sim->parkhaus = NULL;
        simulation_cleanup_children(p_sim);
        return ERROR;
    }

    return OK;
}


//FIXME LUCA IMPLEMENT
int simulation_tick(Simulation *p_sim) {
    p_sim->current_tick++;
    p_sim->parkhaus->base.tick((SimulationObject*) p_sim, p_sim->current_tick);
    return OK;
}

int simulation_start(Simulation *p_sim) {
    if (checkNull(p_sim) || checkNull(p_sim->settings) || checkNull(p_sim->parkhaus) || checkNull(p_sim->StatList)) {
        return ERROR;
    }

    p_sim->current_tick = 0U;
    return OK;
}

void simulation_end(Simulation *p_sim) {
    if (p_sim == NULL) {
        return;
    }

    if (p_sim->StatList != NULL) {
        StatsSummary summary;
        if (stats_build_summary(p_sim->StatList, &summary) == OK) {
            savehandler_save_summary(p_sim, &summary, NULL);
        }
    }

    simulation_cleanup_children(p_sim);
}


int free_simulation(Simulation *p_sim) {
    if (checkNull(p_sim)) {
        print_warning_s("pointer error");
        return ERROR;
    }
    parkhouse_free(p_sim->parkhaus);
    free(p_sim->parkhaus);
    free(p_sim);
    return OK;
}


static void simulation_cleanup_children(Simulation *p_sim) {
    if (p_sim == NULL) {
        return;
    }

    if (p_sim->parkhaus != NULL) {

        if (p_sim->parkhaus->gate_queues != NULL && p_sim->settings != NULL) {

            for (uint32_t i = 0; i < p_sim->settings->gates; ++i) {

                if (p_sim->parkhaus->gate_queues[i] != NULL) {

                    queue_free(p_sim->parkhaus->gate_queues[i]);
                    free(p_sim->parkhaus->gate_queues[i]);
                    p_sim->parkhaus->gate_queues[i] = NULL;

                }
            }
            free(p_sim->parkhaus->gate_queues);
            p_sim->parkhaus->gate_queues = NULL;
        }
        parkhouse_free(p_sim->parkhaus);
        free(p_sim->parkhaus);
        p_sim->parkhaus = NULL;
    }
}

