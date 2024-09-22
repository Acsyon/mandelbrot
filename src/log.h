#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param[in] level Minimum log level to display
 *
 * @return current minimum log level
 */
enum LogLevel
get_log_level(void);

/**
 * Sets the minimum log level to display.
 *
 * @param[in] level Minimum log level to display
 *
 * @return old log level
 */
enum LogLevel
set_log_level(enum LogLevel level);

/**
 * Prints formatted message `format` to log stream prepended with level token if
 * `level` is greater than the minimum log level.
 *
 * @param[in] level log level of the message
 * @param[in] format printf-like format string
 */
void
log_message(enum LogLevel level, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_INCLUDED */
