#ifndef AEROTASK_QUEUE_H
#define AEROTASK_QUEUE_H

#include "common.h"
#include <pthread.h>

typedef struct task_node {
    task_t task;
    struct task_node *next;
} task_node_t;

typedef struct {
    task_node_t *head;
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int stopping;
} task_queue_t;

int queue_init(task_queue_t *q);
void queue_destroy(task_queue_t *q);
int queue_push(task_queue_t *q, const task_t *task);
int queue_pop(task_queue_t *q, task_t *out);
void queue_stop(task_queue_t *q);
size_t queue_size(task_queue_t *q);
#endif
