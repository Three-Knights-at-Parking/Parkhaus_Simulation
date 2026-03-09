#include "io/SaveHandler.h"

#include <inttypes.h>

#include "utils/SafteyUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

const char *savehandler_resolve_stats_path(const char *dest_path) {
    // Using a static buffer so the pointer remains valid after the function returns,
    // saving the caller from having to manually free() the memory.
    static char resolved_path[MAX_PATH_LENGTH];
    memset(resolved_path, 0, MAX_PATH_LENGTH);

    // We ignore the return value here. If the directory already exists,
    // MAKE_DIR will return an error (-1), which is exactly the behavior we expect and can safely ignore.
    // https://linux.die.net/man/2/mkdir
    MAKE_DIR(STATS_BASE_DIR);
    if (dest_path == NULL || dest_path[0] == '\0') {strncpy(resolved_path, DEFAULT_STATS_PATH, MAX_PATH_LENGTH - 1);
    }
    else if (strstr(dest_path, "..") != NULL) {
        // Path contains "..", we do not allow upwards traversal, since it allows writing to abitrary paths! Force safe fallback.
        print_warning_s("savehandler_resolve_stats_path: Path contained '..'. Falling back to safe default.");
        strncpy(resolved_path, DEFAULT_STATS_PATH, MAX_PATH_LENGTH - 1);
    }
    else {
        snprintf(resolved_path, MAX_PATH_LENGTH, "%s%s", STATS_BASE_DIR, dest_path);
    }

    return resolved_path;
}

void savehandler_write_header_if_new(void *file, const Simulation *p_sim, enum OutputMode mode) {
    if (file == NULL || mode == NONE) return;

    FILE *f = (FILE *)file;
    if (mode == NORMAL) {
        fprintf(f, "Tick,CapTotal,CapTaken,Enqueued,Entered,Departed,QueueLen\n");
    } else if (mode == VERBOSE) {
        fprintf(f, "Tick,CapTotal,CapTaken,CapFree,Arrivals,Enqueued,Entered,Departed,"
                   "QueueLenEnd,QueueRejections,WaitTimeAvg,MaxWaitTime,BlockerFull,BadParking\n");
    }
}

int savehandler_save_tick(const Simulation *p_sim, const StatsTick *p_tickstats, const char *dest_path) {
    if (p_sim == NULL || p_tickstats == NULL) return ERROR;

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
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0) {
        savehandler_write_header_if_new(f, p_sim, mode);
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

    // TODO Waiting on UI side function to pass values to

    return OK;
}

int savehandler_save_summary(const Simulation *p_sim, const StatsSummary *p_summary, const char *dest_path) {
    if (p_sim == NULL || p_summary == NULL) return ERROR;

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

    // TODO Waiting on UI function

    return OK;
}

/*
 * FIXME
 * We need to aggregate the different lines in a StatsList that we pass to the UI
 * as determined in Part 1 of the project. This is a stub for now.
 */
int savehandler_load_and_print(const char *src_path) {
    const char *resolved_path = savehandler_resolve_stats_path(src_path);
    if (resolved_path[0] == '\0') return ERROR;

    FILE *f = fopen(resolved_path, "r");
    if (f == NULL) {
        print_warning_s("No existing stats file found to load.");
        return ERROR;
    }

    char buffer[1024];
    printf("\n=== LOADED STATS FILE: %s ===\n", resolved_path);

    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        printf("%s", buffer);
    }
    printf("======================================\n");

    fclose(f);
    return OK;
}