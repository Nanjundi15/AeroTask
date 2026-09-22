#ifndef AEROTASK_COMMON_H
#define AEROTASK_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#define AT_MAX_NAME 128
#define AT_MAX_ERROR 256
#define AT_MAX_PAYLOAD 512
#define AT_MAX_WORKERS 32
#define AT_DEFAULT_RETRIES 3

typedef enum {
    TASK_PENDING = 0,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_RETRYING,
    TASK_DEAD
} task_status_t;

typedef struct {
    int id;
    char name[AT_MAX_NAME];
    int priority;
    task_status_t status;
    int retry_count;
    int max_retries;
    int worker_id;
    time_t created_at;
    time_t started_at;
    time_t completed_at;
    char error[AT_MAX_ERROR];
} task_t;

const char *task_status_name(task_status_t status);
long long now_ms(void);
#endif
