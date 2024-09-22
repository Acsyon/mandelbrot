#include "log.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

static enum LogLevel _minimum_log_level = LOG_WARNING;

static inline const char *
_get_level_string(enum LogLevel level)
{
    switch (level) {
    case LOG_DEBUG:
        return "DBG";
    case LOG_WARNING:
        return "WRN";
    case LOG_ERROR:
        return "ERR";
    default:
        return "";
    }
}

static inline bool
_is_level_valid(enum LogLevel level)
{
    return !(level < LOG_DEBUG || level > LOG_ERROR);
}

enum LogLevel
get_log_level(void)
{
    return _minimum_log_level;
}

enum LogLevel
set_log_level(enum LogLevel level)
{
    if (!_is_level_valid(level)) {
        log_message(LOG_WARNING, "Cannot change log level: invalid value!\n");
        return _minimum_log_level;
    }
    enum LogLevel old_level = _minimum_log_level;
    _minimum_log_level = level;
    return old_level;
}

void
log_message(enum LogLevel level, const char *format, ...)
{
    FILE *const log = stderr;

    if (!_is_level_valid(level)) {
        return;
    }
    if (level < _minimum_log_level) {
        return;
    }

    const char *const lvlstr = _get_level_string(level);
    fprintf(log, "[%s] ", lvlstr);

    va_list args;
    va_start(args, format);
    vfprintf(log, format, args);
    va_end(args);
}
