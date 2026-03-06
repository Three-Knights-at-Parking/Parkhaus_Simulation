#include "Simulation.h"

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


    queue_init()







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

