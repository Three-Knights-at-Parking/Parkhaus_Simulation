#include "utils/gate_routing.h"

#include <stdlib.h>
#include "Queue.h"
#include "utils/RNG.h"
#include "utils/SafteyUtils.h"

int GateRouting_DistributeTotalDemand(const Settings* settings,
                                      const uint16_t total_demand,
                                      Queue** gate_queues,
                                      const uint32_t current_tick)
{
    if (checkNull(settings) || checkNull(gate_queues))
    {
        print_warning_s("primary pointer error");
        return ERROR;
    }
    if (settings->gates == 0U)
    {
        print_warning_s("no gates there");
        return ERROR;
    }

    for (int i = 0; i < settings->gates; ++i)
    {
        if (checkNull(gate_queues[i]))
        {
            print_warning_s("Gate missing");
            return ERROR;
        }
    }

    //es wird angenommen das die gewichtung aller eingänge einheitlich ist
    const uint16_t gates = settings->gates;
    const uint16_t base_share = (uint16_t)(total_demand / gates);
    uint16_t remaining = (uint16_t)(total_demand % gates); //if uneven

    for (int i = 0; i < gates; ++i)
    {
        queue_set_demand(gate_queues[i], base_share);
    }

    //zufallsverteilung von demand der übrig ist
    for (int i = 0; i < remaining; ++i)
    {
        uint32_t random_gate = rng_gate_index(remaining);
        queue_set_demand(gate_queues[random_gate], queue_get_demand(gate_queues[random_gate]) + 1);
    }

    uint32_t sum = 0U;
    for (uint32_t i = 0; i < gates; ++i)
    {
        sum += queue_get_demand(gate_queues[i]);
    }

    if (sum != total_demand)
    {
        print_error("GateRouting_DistributeTotalDemand: demand sum mismatch");
        return ERROR;
    }

    return OK;
}
