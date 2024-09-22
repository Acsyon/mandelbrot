#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_LOG_STREAM stderr

static FILE *_log_stream = NULL;
static enum LogLevel _log_level = LOG_WARNING;

static const char *const LEVEL_STRING[] = {
  [LOG_DEBUG] = "DBG",
  [LOG_WARNING] = "WRN",
  [LOG_ERROR] = "ERR",
};

static inline bool
_is_level_valid(enum LogLevel level)
{
    return !(level < LOG_DEBUG || level > LOG_ERROR);
}

enum LogLevel
log_get_level(void)
{
    return _log_level;
}

enum LogLevel
log_set_level(enum LogLevel level)
{
    if (!_is_level_valid(level)) {
        log_wrn("Cannot change log level: invalid value!\n");
        return _log_level;
    }
    enum LogLevel old_level = _log_level;
    _log_level = level;
    return old_level;
}

FILE *
log_get_stream(void)
{
    return _log_stream;
}

FILE *
log_set_stream(FILE *stream)
{
    FILE *old_stream = _log_stream;
    _log_stream = stream;
    return old_stream;
}

static void
_log_vmsg(enum LogLevel level, const char *format, va_list args)
{
    if (_log_stream == NULL) {
        _log_stream = DEFAULT_LOG_STREAM;
    }
    if (!_is_level_valid(level)) {
        return;
    }
    if (level < _log_level) {
        return;
    }

    const char *const lvlstr = LEVEL_STRING[level];
    fprintf(_log_stream, "[%s] ", lvlstr);
    vfprintf(_log_stream, format, args);
}

void
log_msg(enum LogLevel level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _log_vmsg(level, format, args);
    va_end(args);
}

void
log_dbg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _log_vmsg(LOG_DEBUG, format, args);
    va_end(args);
}

void
log_wrn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _log_vmsg(LOG_WARNING, format, args);
    va_end(args);
}

void
log_err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _log_vmsg(LOG_ERROR, format, args);
    va_end(args);
}
