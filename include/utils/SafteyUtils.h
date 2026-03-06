#ifndef TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAFTEYUTILS_H
#define TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAFTEYUTILS_H
enum Severity{LOW, MEDIUM, HIGH};
#define print_error_s(msg, severity) print_error_s_internal(msg, severity, __func__)
#define print_warning_s(msg) print_warning_s_internal(msg, __func__)


/**
 * Check if a given pointer is null. This works on all non-built-in types.
 * @param ptr Pointer to the object to be checked
 * @return 1 if the pointer is null.
 */
int checkNull(const void *ptr);

/**
 * Check if any given string is empty (or NULL)
 * @param src String to check
 * @return 1 if empty.
 */
int checkEmptyString(const char *src);

/**
 * Print a formatted error message.
 * @param msg The error message to be displayed.
 */
void print_error(const char *msg);

/**
 * Print a formatted warning message.
 * @param msg The error message to be displayed.
 */
void print_warning(const char *msg);

/**
 * Print a formatted error message with additional information.
 * @note Do not call this directly, as it will not work as intended. Call the macro print_error_s instead.
 * @param msg The error message to be displayed.
 * @param severity Severity level of the error.
 * @param func_name The function name this was called from. Will be autofilled by print_error_s.
 */
void print_error_s_internal(const char *msg, enum Severity severity, const char *func_name);

/**
 * Print a formatted warning message with additional information.
 * @note Do not call this directly, as it will not work as intended. Call the macro print_warning_s instead.
 * @param msg The warning message to be displayed.
 * @param func_name The function name this was called from. Will be autofilled by print_warning_s.
 */
void print_warning_s_internal(const char *msg, const char *func_name);

#endif //TEIL1_PARKHAUS_SIMULATION_PLANNUNG_SAFTEYUTILS_H