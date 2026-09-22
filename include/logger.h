#ifndef AEROTASK_LOGGER_H
#define AEROTASK_LOGGER_H
#include <stdarg.h>
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);
#endif
