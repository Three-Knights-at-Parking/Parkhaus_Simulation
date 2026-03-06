#include "Simulation.h"

#include <stdlib.h>

#include "utils/SafteyUtils.h"
#include "utils/RNG.h"
#include "Queue.h"

int simulation_init(Simulation *p_sim, const Settings *p_settings, const StatList *stats) {
    checkNull(p_sim);
    checkNull(p_settings);
    checkNull(stats);



    p_sim->settings = (Settings *) p_settings;
    p_sim->StatList = (StatList *) stats;
    p_sim->current_tick = 0;

    if (rng_init(p_sim->settings) == ERROR)
    {
      return ERROR;
    };

    Queue *Gate_Queues = malloc(p_sim->settings->gates);

    for (int i = 0; i < p_sim->settings->gates; i++)
    {
        queue_init(&Gate_Queues[i], -1);
    }









    return OK;
}

int simulation_tick(Simulation *p_sim) {


    p_sim->current_tick++;
    return OK;
}

int simulation_start(Simulation *p_sim) {


    return OK;
}

void simulation_end(Simulation *p_sim) {

    }

}

int free_simulation(Simulation *p_sim) {

    }

    return OK;
}

