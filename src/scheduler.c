#include "scheduler.h"
#include "storage.h"
#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct { scheduler_t *scheduler; int worker_id; } worker_arg_t;

static void simulate_work(task_t *task, int worker_id) {
    task->worker_id = worker_id;
    task->status = TASK_RUNNING;
    task->started_at = time(NULL);
    storage_update_task(task);

    /* Safe simulated workload: no arbitrary shell execution. */
    sleep(task->priority >= 8 ? 1 : 2);

    /* Deterministic demonstration failure for names containing "fail". */
    if (strstr(task->name, "fail") != NULL) {
        snprintf(task->error, sizeof(task->error), "Simulated worker failure");
        if (task->retry_count < task->max_retries) {
            task->retry_count++;
            task->status = TASK_RETRYING;
            storage_update_task(task);
            unsigned delay = 1u << (task->retry_count - 1);
            sleep(delay);
            task->status = TASK_RETRYING;
        } else {
            task->status = TASK_DEAD;
            task->completed_at = time(NULL);
            storage_update_task(task);
        }
        return;
    }

    task->status = TASK_COMPLETED;
    task->completed_at = time(NULL);
    task->error[0] = '\0';
    storage_update_task(task);
}

static void *worker_loop(void *arg) {
    worker_arg_t *wa = (worker_arg_t*)arg;
    scheduler_t *s = wa->scheduler;
    int id = wa->worker_id;
    free(wa);
    log_info("Worker %d started", id);

    task_t task;
    while (s->running && queue_pop(&s->queue, &task)) {
        simulate_work(&task, id);
        if (task.status == TASK_RETRYING) {
            if (queue_push(&s->queue, &task) != 0) {
                task.status = TASK_DEAD;
                snprintf(task.error,sizeof(task.error),"Could not requeue retry");
                storage_update_task(&task);
            }
        }
    }
    log_info("Worker %d stopped", id);
    return NULL;
}

int scheduler_init(scheduler_t *s, int worker_count) {
    if (worker_count < 1 || worker_count > AT_MAX_WORKERS) return -1;
    memset(s,0,sizeof(*s));
    s->worker_count=worker_count; s->running=1;
    return queue_init(&s->queue);
}

void scheduler_start(scheduler_t *s) {
    for(int i=0;i<s->worker_count;i++) {
        worker_arg_t *a=malloc(sizeof(*a));
        if(!a) continue;
        a->scheduler=s; a->worker_id=i+1;
        pthread_create(&s->workers[i],NULL,worker_loop,a);
    }
}

int scheduler_submit(scheduler_t *s, task_t *task) {
    task->status=TASK_PENDING;
    task->created_at=time(NULL);
    task->retry_count=0;
    task->worker_id=0;
    task->started_at=0;
    task->completed_at=0;
    task->error[0]='\0';
    if(storage_insert_task(task)!=0) return -1;
    if(queue_push(&s->queue,task)!=0) return -1;
    return 0;
}

void scheduler_stop(scheduler_t *s) {
    if(!s->running) return;
    s->running=0;
    queue_stop(&s->queue);
    for(int i=0;i<s->worker_count;i++) pthread_join(s->workers[i],NULL);
    queue_destroy(&s->queue);
}
