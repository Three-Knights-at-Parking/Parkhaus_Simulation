#include "io/SaveHandler.h"

#include <inttypes.h>

#include "utils/SafteyUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui/ui_statistics.h"
#ifdef _WIN32
    #include <direct.h>
    #define MAKE_DIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MAKE_DIR(path) mkdir(path, 0777)
#endif

#define STATS_BASE_DIR "./stats/"
#define DEFAULT_STATS_FILE "stats.csv"
#define DEFAULT_STATS_PATH "./stats/stats.csv"
#define MAX_PATH_LENGTH 512

static int savehandler_parse_tick_line(const char *line, StatsTick *tick);
static void savehandler_append_tick(StatList *list, StatsTick *tick);
static void savehandler_parse_summary_line(const char *line, StatsSummary *summary);

const char *savehandler_resolve_stats_path(const char *dest_path) {
    // Using a static buffer so the pointer remains valid after the function returns,
    // saving the caller from having to manually free() the memory.
    static char resolved_path[MAX_PATH_LENGTH];
    memset(resolved_path, 0, MAX_PATH_LENGTH);

    // We ignore the return value here. If the directory already exists,
    // MAKE_DIR will return an error (-1), which is exactly the behavior we expect and can safely ignore.
    // https://linux.die.net/man/2/mkdir
    MAKE_DIR(STATS_BASE_DIR);
    if (dest_path == NULL || dest_path[0] == '\0') {
        strncpy(resolved_path, DEFAULT_STATS_PATH, MAX_PATH_LENGTH - 1);
    } else if (strstr(dest_path, "..") != NULL) {
        // Path contains ".."; we do not allow upwards traversal, since it allows writing to arbitrary paths! Force safe fallback.
        print_warning_s("Path contained '..'. Falling back to safe default.");
        strncpy(resolved_path, DEFAULT_STATS_PATH, MAX_PATH_LENGTH - 1);
    } else if (strstr(dest_path, ":") != NULL) {
        // Path contains ":", we do not allow absolute paths, since it allows writing to arbitrary paths! Force safe fallback.
        print_warning_s("Path contained ':'. Falling back to safe default.");
        strncpy(resolved_path, DEFAULT_STATS_PATH, MAX_PATH_LENGTH - 1);
    } else {
        if (snprintf(resolved_path, MAX_PATH_LENGTH, "%s%s", STATS_BASE_DIR, dest_path) >= MAX_PATH_LENGTH) {
            print_warning_s("Path too long. Falling back to safe default.");
            strncpy(resolved_path, DEFAULT_STATS_PATH, MAX_PATH_LENGTH - 1);
        }
    }
    return resolved_path;
}

void savehandler_write_header_if_new(void *file, const Simulation *p_sim, enum OutputMode mode) {
    if (file == NULL || mode == NONE) return;
    FILE *f = (FILE *)file;
    const Settings *p_settings = p_sim->settings;
    const char *name = (p_settings->name[0] != '\0') ? p_settings->name : "Rauenegg";
    fprintf(f, "# Simulation Settings\n");
    fprintf(f, "# Name,%s\n", name);
    fprintf(f, "# CapacityPerFloor,%u\n", p_settings->capacity);
    fprintf(f, "# Floors,%u\n", p_settings->floors);
    fprintf(f, "# Gates,%u\n", p_settings->gates);
    fprintf(f, "# GateEntryInSec,%u\n", p_settings->gate_entry_inSec);
    fprintf(f, "# TickInSec,%u\n", p_settings->tick_inSec);
    fprintf(f, "# RealEquivalent,%u\n", p_settings->real_equivalent);
    fprintf(f, "# OutputMode,%d\n", (int)p_settings->output_mode);
    fprintf(f, "# MaxTicks,%d\n", p_settings->max_ticks);
    fprintf(f, "# RandSeed,%d\n", p_settings->rand_seed);
    fprintf(f, "# EntryProbabilityPerSecPrec,%.2f\n", p_settings->entry_probability_perSec_prec);
    fprintf(f, "# QueueLeavable,%d\n", (int)p_settings->is_leavable);
    fprintf(f, "#\n");
    if (mode == NORMAL) {
        fprintf(f, "Tick,CapTotal,CapTaken,Enqueued,Entered,Departed,QueueLen\n");
    } else if (mode == VERBOSE) {
        fprintf(f, "Tick,CapTotal,CapTaken,CapFree,Arrivals,Enqueued,Entered,Departed,"
                   "QueueLenEnd,QueueRejections,WaitTimeAvg,MaxWaitTime,BlockerFull,BadParking\n");
    }
}

int savehandler_save_tick(const Simulation *p_sim, const StatsTick *p_tickstats, const char *dest_path) {
    if (p_sim == NULL || p_tickstats == NULL || p_sim->settings == NULL) {
        return ERROR;
    }
    // TODO: Adjust this field access based on your exact Settings struct definition!
    const enum OutputMode mode = p_sim->settings->output_mode;

    if (mode == NONE) {
        return OK;
    }
    const char *resolved_path = savehandler_resolve_stats_path(dest_path);
    if (resolved_path[0] == '\0') return ERROR;
    FILE *f = fopen(resolved_path, "a+");
    if (f == NULL) {
        print_error_s("Could not open stats file for writing.", HIGH);
        return ERROR;
    }
    if (fseek(f, 0, SEEK_END) == 0) {
        long file_size = ftell(f);
        if (file_size == 0) {
            savehandler_write_header_if_new(f, p_sim, mode);
        }
    }
    if (mode == NORMAL) {
        fprintf(f, "%u,%u,%u,%u,%u,%u,%u\n",
                p_tickstats->current_tick, p_tickstats->capacity_total, p_tickstats->capacity_taken,
                p_tickstats->enqueued, p_tickstats->entered, p_tickstats->departed,
                p_tickstats->queue_length_end);
    } else if (mode == VERBOSE) {
        uint32_t avg_wait = 0;
        if (p_tickstats->queue_wait_entered_count > 0) {
            avg_wait = (uint32_t)(p_tickstats->queue_wait_entered_sum_ticks / p_tickstats->queue_wait_entered_count);
        }

        fprintf(f, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
                p_tickstats->current_tick, p_tickstats->capacity_total, p_tickstats->capacity_taken, p_tickstats->capacity_free,
                p_tickstats->arrivals_generated, p_tickstats->enqueued, p_tickstats->entered, p_tickstats->departed,
                p_tickstats->queue_length_end, p_tickstats->queue_rejections, avg_wait, p_tickstats->queue_wait_max_ticks_tick,
                p_tickstats->blocker_full_active, p_tickstats->bad_parking_cases);
    }

    fclose(f);
    ui_statistics_print_tick(p_tickstats, p_sim->settings);

    return OK;
}

int savehandler_save_summary(const Simulation *p_sim, const StatsSummary *p_summary, const char *dest_path) {
    if (p_sim == NULL || p_summary == NULL || p_sim->settings == NULL) {
        return ERROR;
    }
    const enum OutputMode mode = p_sim->settings->output_mode;
    if (mode == NONE) return OK;

    const char *resolved_path = savehandler_resolve_stats_path(dest_path);
    if (resolved_path[0] == '\0') return ERROR;

    FILE *f = fopen(resolved_path, "a");
    if (f == NULL) {
        print_error_s("Could not open stats file for writing.", HIGH);
        return ERROR;
    }


    fprintf(f, "\n--- SIMULATION SUMMARY ---\n");
    fprintf(f, "Total Ticks,%u\n", p_summary->total_ticks);
    fprintf(f, "Total Arrivals,%" PRIu64 "\n", p_summary->arrivals_total);
    fprintf(f, "Total Entered,%" PRIu64 "\n", p_summary->entered_total);
    fprintf(f, "Total Departed,%" PRIu64 "\n", p_summary->departed_total);
    fprintf(f, "Avg Utilization (%%),%.2f\n", p_summary->capacity_taken_percent_avg);
    fprintf(f, "Peak Utilization (%%),%.2f\n", p_summary->capacity_taken_percent_peak);
    fprintf(f, "Avg Queue Length,%.2f\n", p_summary->queue_length_avg);
    fprintf(f, "Avg Wait Time (Ticks),%u\n", p_summary->queue_wait_avg_ticks);
    fprintf(f, "Max Wait Time (Ticks),%u\n", p_summary->queue_wait_max_ticks);
    fprintf(f, "Bad Parking Share (%%),%.2f\n", p_summary->bad_parking_share_percent);

    fclose(f);
    ui_statistics_print_final(p_summary, p_sim->settings);
    return OK;
}

int savehandler_load_and_print(const char *src_path, StatList* list) {
    if (list == NULL) {
        return ERROR;
    }
    const char *resolved_path = savehandler_resolve_stats_path(src_path);
    if (resolved_path[0] == '\0') {
        return ERROR;
    }
    FILE *f = fopen(resolved_path, "r");
    if (f == NULL) {
        print_error_s("No existing stats file found to load.", LOW);
        return ERROR;
    }

    list->p_tick_head = NULL;
    list->p_tick_tail = NULL;
    list->p_current_tick = NULL;

    if (list->p_summary != NULL) {
        memset(list->p_summary, 0, sizeof(*list->p_summary));
    }

    char buffer[1024];
    int in_summary_section = 0;

    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (buffer[0] == '\0') {
            continue;
        }

        if (strncmp(buffer, "--- SIMULATION SUMMARY ---", 26) == 0) {
            in_summary_section = 1;
            continue;
        }

        if (buffer[0] == '#') {
            continue;
        }

        if (strncmp(buffer, "Tick,", 5) == 0) {
            continue;
        }

        if (in_summary_section) {
            if (list->p_summary != NULL) {
                savehandler_parse_summary_line(buffer, list->p_summary);
            }
            continue;
        }

        StatsTick *tick = (StatsTick *)malloc(sizeof(StatsTick));
        if (tick == NULL) {
            fclose(f);
            print_error_s("Out of memory while loading stats.", HIGH);
            return ERROR;
        }

        if (savehandler_parse_tick_line(buffer, tick) != OK) {
            free(tick);
            continue;
        }

        savehandler_append_tick(list, tick);
    }

    fclose(f);
    return OK;
}

static void savehandler_append_tick(StatList *list, StatsTick *tick) {
    if (list == NULL || tick == NULL) {
        return;
    }

    tick->p_prev = list->p_tick_tail;
    tick->p_next = NULL;

    if (list->p_tick_tail != NULL) {
        list->p_tick_tail->p_next = tick;
    } else {
        list->p_tick_head = tick;
    }

    list->p_tick_tail = tick;
}
static int savehandler_parse_tick_line(const char *line, StatsTick *tick) {
    if (line == NULL || tick == NULL) {
        return ERROR;
    }
    memset(tick, 0, sizeof(*tick));

    char buffer[1024];
    size_t len = strlen(line);
    if (len >= sizeof(buffer)) {
        return ERROR;
    }
    memcpy(buffer, line, len + 1);

    unsigned long values[14];
    int count = 0;

    char *cursor = buffer;
    while (cursor != NULL && *cursor != '\0' && count < 14) {
        char *comma = strchr(cursor, ',');
        if (comma != NULL) {
            *comma = '\0';
        }

        char *end_ptr = NULL;
        unsigned long value = strtoul(cursor, &end_ptr, 10);
        if (end_ptr == cursor || *end_ptr != '\0') {
            return ERROR;
        }

        values[count++] = value;

        if (comma == NULL) {
            break;
        }
        cursor = comma + 1;
    }

    if (count == 14) {
        tick->current_tick = (uint32_t)values[0];
        tick->capacity_total = (uint16_t)values[1];
        tick->capacity_taken = (uint16_t)values[2];
        tick->capacity_free = (uint16_t)values[3];
        tick->arrivals_generated = (uint16_t)values[4];
        tick->enqueued = (uint16_t)values[5];
        tick->entered = (uint16_t)values[6];
        tick->departed = (uint16_t)values[7];
        tick->queue_length_end = (uint8_t)values[8];
        tick->queue_rejections = (uint32_t)values[9];
        tick->queue_wait_max_ticks_tick = (uint32_t)values[11];
        tick->blocker_full_active = (uint16_t)values[12];
        tick->bad_parking_cases = (uint16_t)values[13];
        /* WaitTimeAvg is stored in column 10.
         * We cannot reconstruct original sum/count exactly from an average,
         * so we store a minimal placeholder representation.
         */
        if (values[10] > 0) {
            tick->queue_wait_entered_sum_ticks = (uint64_t)values[10];
            tick->queue_wait_entered_count = 1;
        }

        return OK;
    }

    if (count == 7) {
        tick->current_tick = (uint32_t)values[0];
        tick->capacity_total = (uint16_t)values[1];
        tick->capacity_taken = (uint16_t)values[2];
        tick->capacity_free = (values[1] >= values[2]) ? (uint16_t)(values[1] - values[2]) : 0;
        tick->enqueued = (uint16_t)values[3];
        tick->entered = (uint16_t)values[4];
        tick->departed = (uint16_t)values[5];
        tick->queue_length_end = (uint8_t)values[6];
        return OK;
    }

    return ERROR;
}

static void savehandler_parse_summary_line(const char *line, StatsSummary *summary) {
    if (line == NULL || summary == NULL) {
        return;
    }

    const char *comma = strchr(line, ',');
    if (comma == NULL) {
        return;
    }

    size_t key_len = (size_t)(comma - line);
    if (key_len == 0 || key_len >= 128) {
        return;
    }

    char key[128];
    memcpy(key, line, key_len);
    key[key_len] = '\0';

    const char *value = comma + 1;
    if (*value == '\0') {
        return;
    }

    if (strcmp(key, "Total Ticks") == 0) {
        summary->total_ticks = (uint32_t)strtoul(value, NULL, 10);
    } else if (strcmp(key, "Total Arrivals") == 0) {
        summary->arrivals_total = (uint64_t)strtoull(value, NULL, 10);
    } else if (strcmp(key, "Total Entered") == 0) {
        summary->entered_total = (uint64_t)strtoull(value, NULL, 10);
    } else if (strcmp(key, "Total Departed") == 0) {
        summary->departed_total = (uint64_t)strtoull(value, NULL, 10);
    } else if (strcmp(key, "Avg Utilization (%)") == 0) {
        summary->capacity_taken_percent_avg = strtof(value, NULL);
    } else if (strcmp(key, "Peak Utilization (%)") == 0) {
        summary->capacity_taken_percent_peak = strtof(value, NULL);
    } else if (strcmp(key, "Avg Queue Length") == 0) {
        summary->queue_length_avg = strtof(value, NULL);
    } else if (strcmp(key, "Avg Wait Time (Ticks)") == 0) {
        summary->queue_wait_avg_ticks = (uint32_t)strtoul(value, NULL, 10);
    } else if (strcmp(key, "Max Wait Time (Ticks)") == 0) {
        summary->queue_wait_max_ticks = (uint32_t)strtoul(value, NULL, 10);
    } else if (strcmp(key, "Bad Parking Share (%)") == 0) {
        summary->bad_parking_share_percent = strtof(value, NULL);
    }
}