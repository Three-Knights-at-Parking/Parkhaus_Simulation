#include "Simulation.h"

#include <stdlib.h>

#include "Parkhaus.h"
#include "utils/SafteyUtils.h"
#include "utils/RNG.h"
#include "Queue.h"

int simulation_init(Simulation *p_sim, const Settings *p_settings, const StatList *p_StatList) {
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

    //erstellen der Queues
    Queue *Gate_Queues = calloc(p_sim->settings->gates, sizeof(Queue));

    if (Gate_Queues == NULL)
    {
        int j = 0;
        while (j < p_sim->settings->gates)
        {
            queue_free(&Gate_Queues[j]);
            free(&Gate_Queues[j]);
            j = j + 1;
        }
        print_error("Gate Queues allocation failed");
        free(Gate_Queues); // Fixing memory leak
        return ERROR;
    }
    //initialisierung der Queues
    for (int i = 0; i < p_sim->settings->gates; i++)
    {
        queue_init(&Gate_Queues[i], -1);
    }

    //Allocation of Parkhouse
    p_sim->parkhaus = calloc(1, sizeof(Parkhaus));
    if (p_sim->parkhaus == NULL)
    {
        int j = 0;
        while (j < p_sim->settings->gates)
        {
            queue_free(&Gate_Queues[j]);
            free(&Gate_Queues[j]);
            j = j + 1;
        }
        free(p_sim->parkhaus);
        p_sim->parkhaus = NULL;
        print_error("Parkhaus allocation failed");
        return ERROR;
    }

    status = parkhouse_init(p_sim->parkhaus, p_sim->settings, &Gate_Queues);
    if (status == ERROR)
    {
        //Parkhouse has not been initialised correctly
        int j = 0;
        while (j < p_sim->settings->gates)
        {
            queue_free(&Gate_Queues[j]);
            free(&Gate_Queues[j]);
        }
        free(p_sim->parkhaus);
        p_sim->parkhaus = NULL;
    }

    //initialisierung von Statistik List Objekt
   // p_StatList =










    return OK;
}

int simulation_tick(Simulation *p_sim) {


    p_sim->current_tick++;
    p_sim->parkhaus->base.tick((SimulationObject*) p_sim, p_sim->current_tick);
    for (int i = 0; i < p_sim->settings->gates-1; i++) {
        queue_tick(&p_sim->parkhaus->gate_queues[i]->base, p_sim->current_tick);
    }
    return OK;
}

int simulation_start(Simulation *p_sim) {


    return OK;
}

void simulation_end(Simulation *p_sim) {


}

int free_simulation(Simulation *p_sim) {
    if (checkNull(p_sim)) {
        return UNKNOWN;
    }
    parkhouse_free(p_sim->parkhaus);
    free(p_sim->parkhaus);
    free(p_sim);
    return OK;
}

