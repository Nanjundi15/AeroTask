#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

static void write_log(const char *level, const char *fmt, va_list ap) {
    time_t now = time(NULL);
    struct tm tmv;
#ifdef _WIN32
    localtime_s(&tmv, &now);
#else
    struct tm *tmp = localtime(&now); if (tmp) tmv = *tmp; else memset(&tmv, 0, sizeof(tmv));
#endif
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tmv);
    fprintf(stderr, "[%s] [%s] ", ts, level);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}

void log_info(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt); write_log("INFO", fmt, ap); va_end(ap);
}
void log_warn(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt); write_log("WARN", fmt, ap); va_end(ap);
}
void log_error(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt); write_log("ERROR", fmt, ap); va_end(ap);
}
