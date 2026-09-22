#include "storage.h"
#include "logger.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

static sqlite3 *db = NULL;

int storage_open(const char *path) {
    if (sqlite3_open(path, &db) != SQLITE_OK) return -1;
    const char *sql =
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "priority INTEGER NOT NULL,"
        "status INTEGER NOT NULL,"
        "retry_count INTEGER NOT NULL,"
        "max_retries INTEGER NOT NULL,"
        "worker_id INTEGER,"
        "created_at INTEGER,"
        "started_at INTEGER,"
        "completed_at INTEGER,"
        "error TEXT);";
    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        log_error("SQLite schema error: %s", err ? err : "unknown");
        sqlite3_free(err); return -1;
    }
    return 0;
}

void storage_close(void) {
    if (db) sqlite3_close(db);
    db = NULL;
}

int storage_insert_task(task_t *task) {
    const char *sql = "INSERT INTO tasks(name,priority,status,retry_count,max_retries,worker_id,created_at,started_at,completed_at,error) VALUES(?,?,?,?,?,?,?,?,?,?)";
    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(db, sql, -1, &st, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_text(st,1,task->name,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(st,2,task->priority);
    sqlite3_bind_int(st,3,task->status);
    sqlite3_bind_int(st,4,task->retry_count);
    sqlite3_bind_int(st,5,task->max_retries);
    sqlite3_bind_int(st,6,task->worker_id);
    sqlite3_bind_int64(st,7,(sqlite3_int64)task->created_at);
    sqlite3_bind_int64(st,8,(sqlite3_int64)task->started_at);
    sqlite3_bind_int64(st,9,(sqlite3_int64)task->completed_at);
    sqlite3_bind_text(st,10,task->error,-1,SQLITE_TRANSIENT);
    int rc = sqlite3_step(st);
    if (rc == SQLITE_DONE) task->id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(st);
    return rc == SQLITE_DONE ? 0 : -1;
}

int storage_update_task(const task_t *task) {
    const char *sql = "UPDATE tasks SET name=?,priority=?,status=?,retry_count=?,max_retries=?,worker_id=?,created_at=?,started_at=?,completed_at=?,error=? WHERE id=?";
    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(db, sql, -1, &st, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_text(st,1,task->name,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(st,2,task->priority);
    sqlite3_bind_int(st,3,task->status);
    sqlite3_bind_int(st,4,task->retry_count);
    sqlite3_bind_int(st,5,task->max_retries);
    sqlite3_bind_int(st,6,task->worker_id);
    sqlite3_bind_int64(st,7,(sqlite3_int64)task->created_at);
    sqlite3_bind_int64(st,8,(sqlite3_int64)task->started_at);
    sqlite3_bind_int64(st,9,(sqlite3_int64)task->completed_at);
    sqlite3_bind_text(st,10,task->error,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(st,11,task->id);
    int rc = sqlite3_step(st);
    sqlite3_finalize(st);
    return rc == SQLITE_DONE ? 0 : -1;
}

int storage_get_task(int id, task_t *out) {
    const char *sql = "SELECT id,name,priority,status,retry_count,max_retries,worker_id,created_at,started_at,completed_at,error FROM tasks WHERE id=?";
    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(db, sql, -1, &st, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_int(st,1,id);
    int rc = sqlite3_step(st);
    if (rc == SQLITE_ROW) {
        memset(out,0,sizeof(*out));
        out->id=sqlite3_column_int(st,0);
        snprintf(out->name,sizeof(out->name),"%s",(const char*)sqlite3_column_text(st,1));
        out->priority=sqlite3_column_int(st,2);
        out->status=(task_status_t)sqlite3_column_int(st,3);
        out->retry_count=sqlite3_column_int(st,4);
        out->max_retries=sqlite3_column_int(st,5);
        out->worker_id=sqlite3_column_int(st,6);
        out->created_at=(time_t)sqlite3_column_int64(st,7);
        out->started_at=(time_t)sqlite3_column_int64(st,8);
        out->completed_at=(time_t)sqlite3_column_int64(st,9);
        const unsigned char *e=sqlite3_column_text(st,10);
        if(e) snprintf(out->error,sizeof(out->error),"%s",(const char*)e);
        sqlite3_finalize(st); return 0;
    }
    sqlite3_finalize(st); return -1;
}

int storage_list_tasks(void) {
    const char *sql="SELECT id,name,priority,status,retry_count,worker_id FROM tasks ORDER BY id DESC LIMIT 30";
    sqlite3_stmt *st;
    if(sqlite3_prepare_v2(db,sql,-1,&st,NULL)!=SQLITE_OK) return -1;
    printf("ID   PRIORITY STATUS      RETRIES WORKER NAME\n");
    printf("---------------------------------------------------------------\n");
    while(sqlite3_step(st)==SQLITE_ROW) {
        printf("%-4d %-8d %-11s %-7d %-6d %s\n",
            sqlite3_column_int(st,0), sqlite3_column_int(st,2),
            task_status_name((task_status_t)sqlite3_column_int(st,3)),
            sqlite3_column_int(st,4), sqlite3_column_int(st,5),
            sqlite3_column_text(st,1));
    }
    sqlite3_finalize(st); return 0;
}

void storage_print_stats(void) {
    const char *sql="SELECT status, COUNT(*) FROM tasks GROUP BY status";
    sqlite3_stmt *st;
    if(sqlite3_prepare_v2(db,sql,-1,&st,NULL)!=SQLITE_OK) return;
    printf("Task statistics:\n");
    while(sqlite3_step(st)==SQLITE_ROW)
        printf("  %-11s %lld\n", task_status_name((task_status_t)sqlite3_column_int(st,0)), sqlite3_column_int64(st,1));
    sqlite3_finalize(st);
}
