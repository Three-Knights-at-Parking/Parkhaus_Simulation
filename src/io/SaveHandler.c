#include "io/SaveHandler.h"
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

#define STATS_BASE_DIR "../stats/"
#define DEFAULT_STATS_FILE "stats.csv"
#define DEFAULT_STATS_PATH "../stats/stats.csv"
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
    // TODO
}

int savehandler_save_tick(const Simulation *p_sim, const StatsTick *p_tickstats, const char *dest_path) {
    // TODO
    return OK;
}

int savehandler_save_summary(const Simulation *p_sim, const StatsSummary *p_summary, const char *dest_path) {
    // TODO
    return OK;
}

int savehandler_load_and_print(const char *src_path) {
    // TODO
    return OK;
}