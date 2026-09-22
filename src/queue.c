#include "queue.h"
#include <stdlib.h>

int queue_init(task_queue_t *q) {
    q->head = NULL; q->size = 0; q->stopping = 0;
    if (pthread_mutex_init(&q->mutex, NULL) != 0) return -1;
    if (pthread_cond_init(&q->cond, NULL) != 0) return -1;
    return 0;
}

void queue_destroy(task_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    task_node_t *n = q->head;
    while (n) { task_node_t *next = n->next; free(n); n = next; }
    q->head = NULL; q->size = 0;
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

static void insert_priority(task_queue_t *q, task_node_t *node) {
    if (!q->head || node->task.priority > q->head->task.priority) {
        node->next = q->head; q->head = node; return;
    }
    task_node_t *cur = q->head;
    while (cur->next && cur->next->task.priority >= node->task.priority)
        cur = cur->next;
    node->next = cur->next; cur->next = node;
}

int queue_push(task_queue_t *q, const task_t *task) {
    task_node_t *node = (task_node_t*)malloc(sizeof(*node));
    if (!node) return -1;
    node->task = *task; node->next = NULL;
    pthread_mutex_lock(&q->mutex);
    if (q->stopping) { pthread_mutex_unlock(&q->mutex); free(node); return -1; }
    insert_priority(q, node);
    q->size++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int queue_pop(task_queue_t *q, task_t *out) {
    pthread_mutex_lock(&q->mutex);
    while (!q->head && !q->stopping)
        pthread_cond_wait(&q->cond, &q->mutex);
    if (!q->head && q->stopping) {
        pthread_mutex_unlock(&q->mutex); return 0;
    }
    task_node_t *node = q->head;
    q->head = node->next; q->size--;
    *out = node->task;
    free(node);
    pthread_mutex_unlock(&q->mutex);
    return 1;
}

void queue_stop(task_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    q->stopping = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

size_t queue_size(task_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    size_t n = q->size;
    pthread_mutex_unlock(&q->mutex);
    return n;
}
