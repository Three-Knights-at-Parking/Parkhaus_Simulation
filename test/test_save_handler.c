#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io/SaveHandler.h"
#include "types.h"

static void test_resolve_stats_path_defaults(void) {
    const char *path_null = savehandler_resolve_stats_path(NULL);
    const char *path_empty = savehandler_resolve_stats_path("");
    const char *path_parent = savehandler_resolve_stats_path("../evil.csv");
    const char *path_custom = savehandler_resolve_stats_path("safe.csv");

    assert(path_null != NULL);
    assert(path_empty != NULL);
    assert(path_parent != NULL);
    assert(path_custom != NULL);

    assert(strcmp(path_null, "./stats/stats.csv") == 0);
    assert(strcmp(path_empty, "./stats/stats.csv") == 0);
    assert(strcmp(path_parent, "./stats/stats.csv") == 0);
    assert(strcmp(path_custom, "./stats/safe.csv") == 0);
}