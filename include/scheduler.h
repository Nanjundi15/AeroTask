#ifndef AEROTASK_SCHEDULER_H
#define AEROTASK_SCHEDULER_H

#include "common.h"
#include "queue.h"

typedef struct {
    task_queue_t queue;
    pthread_t workers[AT_MAX_WORKERS];
    int worker_count;
    volatile int running;
} scheduler_t;

int scheduler_init(scheduler_t *s, int worker_count);
void scheduler_start(scheduler_t *s);
int scheduler_submit(scheduler_t *s, task_t *task);
void scheduler_stop(scheduler_t *s);
#endif
