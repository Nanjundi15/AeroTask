#include "common.h"
#include <sys/time.h>
#include <string.h>

const char *task_status_name(task_status_t status) {
    switch (status) {
        case TASK_PENDING: return "PENDING";
        case TASK_RUNNING: return "RUNNING";
        case TASK_COMPLETED: return "COMPLETED";
        case TASK_FAILED: return "FAILED";
        case TASK_RETRYING: return "RETRYING";
        case TASK_DEAD: return "DEAD";
        default: return "UNKNOWN";
    }
}

long long now_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}
