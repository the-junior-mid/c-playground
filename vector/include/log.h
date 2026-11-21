#include <stdarg.h>
#include <stdio.h>


#ifndef MID_LOG_H
#define MID_LOG_H

#define LOG(lvl, fmt, ...) \
    log_write((lvl), __FILE__, __LINE__, fmt "\n", ##__VA_ARGS__)

static const char *log_lvl[] = {"DEBUG", "INFO", "ERROR", "WARN"};
typedef enum {DEBUG, INFO, ERROR, WARN} log_level;

void log_write(log_level lvl, const char *file, int line, const char *fmt, ...) {
    fprintf(stderr, "[%s] %s:%d: ", log_lvl[lvl], file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

#endif