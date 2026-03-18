#include "utils/SafteyUtils.h"

#include <stdio.h>
#include "types.h"


int checkNull(const void *ptr) {
    return ptr == NULL;
}

void print_warning(const char *msg) {
    printf("[Warning] %s\n ", msg);
}

void print_warning_s_internal(const char *msg, const char *func_name) {
    printf("[Warning] Warning in %s(): %s\n", func_name, msg);

}

void print_error(const char *msg) {
    printf("[Error] An error occurred %s\n ", msg);
}

void print_error_s_internal(const char *msg, const enum Severity severity, const char *func_name) {
    switch (severity) {
        case LOW:
            printf("[Error / Low] An error occurred in %s(): %s\n", func_name, msg);
            break;
        case MEDIUM:
            printf("[Info] An error occurred in %s(): %s\n", func_name, msg);
            break;
        case HIGH:
            printf("[Error] An error occurred in %s(): %s\n", func_name, msg);
            break;
    }
}

int checkEmptyString(const char *src) {
    return src == NULL || src[0] == '\0';
}

void print_log_s_internal(const char *msg, const char *func_name) {
    printf("[Log / %s()]: %s\n", func_name, msg);
}