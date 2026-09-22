#ifndef AEROTASK_STORAGE_H
#define AEROTASK_STORAGE_H
#include "common.h"
int storage_open(const char *path);
void storage_close(void);
int storage_insert_task(task_t *task);
int storage_update_task(const task_t *task);
int storage_get_task(int id, task_t *out);
int storage_list_tasks(void);
void storage_print_stats(void);
#endif
