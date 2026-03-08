#include "Stats.h"

int StatsTick_init(Simulation *p_sim, uint16_t capacity_total, uint32_t current_tick) {
}

StatList * StatList_init(Simulation *p_simulation) {
}

int StatList_free(StatList *p_stats) {
}

int StatsTick_free(StatList *p_stats) {
}

int stats_tick_set_capacity(StatList *p_stats, uint16_t taken, uint16_t free) {
}

int stats_tick_add_queue_rejections(StatList *p_stats, uint16_t amount) {
}

int stats_tick_add_arrivals_generated(StatList *p_stats, uint16_t amount) {
}

int stats_tick_add_blocker_full_active(StatList *p_stats) {
}

int stats_tick_add_vehicle(StatList *p_stats, const GenericVehicle *p_vehicle, uint32_t current_tick) {
}

const StatsTick * stats_get_latest_tick(const StatList *p_stats) {
}

int stats_build_summary(const StatList *p_stats, StatsSummary *p_summary) {
}
