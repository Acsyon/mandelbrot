/* log.h
 *
 * Header for logging stuff
 *
 */

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <stdio.h>

/**
 * Log level enumerator
 */
enum LogLevel {
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR,
};

/**
 * Gets the current minimum log level to display.
 *
 * @return current minimum log level
 */
enum LogLevel
log_get_level(void);

/**
 * Sets the minimum log level to display.
 *
 * @param[in] level new minimum log level
 *
 * @return old minimum log level
 */
enum LogLevel
log_set_level(enum LogLevel level);

/**
 * Gets the current log stream.
 *
 * @return current log stream
 */
FILE *
log_get_stream(void);

/**
 * Sets the log stream.
 *
 * @param[in] stream new log stream
 *
 * @return old log stream
 */
FILE *
log_set_stream(FILE *stream);

/**
 * Prints formatted message `format` to log stream prepended with level token if
 * `level` is greater than the minimum log level.
 *
 * @param[in] level log level of the message
 * @param[in] format printf-like format string
 */
void
log_msg(enum LogLevel level, const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level LOG_DEBUG.
 *
 * @param[in] format printf-like format string
 */
void
log_dbg(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level LOG_WARNING.
 *
 * @param[in] format printf-like format string
 */
void
log_wrn(const char *format, ...);

/**
 * Prints formatted message `format` to log stream for level LOG_ERROR.
 *
 * @param[in] format printf-like format string
 */
void
log_err(const char *format, ...);

#endif /* LOG_H_INCLUDED */
