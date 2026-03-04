#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAFTEYUTILS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAFTEYUTILS_H
#include "../types.h"
enum Severity{LOW, MEDIUM, HIGH};
#define print_error_s(msg, severity) print_error_s_internal(msg, severity, __func__)
#define print_warning_s(msg) print_warning_s_internal(msg, __func__)

int checkNull(const void *ptr);

void print_error(const char *msg);
void print_warning(const char *msg);
void print_error_s_internal(const char *msg, enum Severity severity, const char *func_name);
void print_warning_s_internal(const char *msg, const char *func_name);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAFTEYUTILS_H