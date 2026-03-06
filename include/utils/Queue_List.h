#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_QUEUELIST_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_QUEUELIST_H

#include "../types.h"
#include "Queue.h"

/**
 * @brief Dynamic list for Queue pointers.
 *        Useful for managing a variable number of gate queues.
 */

/**
 * @brief Initialize a Queue_List.
 * @param p_list Pointer to Queue_List to initialize.
 * @param initial_capacity Initial storage capacity (0 uses default capacity).
 * @return 0 on success, non-zero on allocation/parameter error.
 */
int queue_list_init(Queue_List *p_self, Simulation *p_sim, uint8_t anzGates);

/**
 * @brief Release internal storage of Queue_List.
 *        Does not free Queue objects pointed to by entries.
 * @param p_list Pointer to Queue_List.
 */
void queue_list_free(Queue_List *p_list);

/**
 * @brief Append a Queue pointer to the list.
 * @param p_list Pointer to Queue_List.
 * @param p_queue Queue pointer to append.
 * @return 0 on success, non-zero on allocation/parameter error.
 */
int queue_list_push(Queue_List *p_list, Queue *p_queue);

#endif // TEIL1_PARKHAUS_SIMULATION_PLANNUNG_QUEUELIST_H
