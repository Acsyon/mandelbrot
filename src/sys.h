/* sys.h
 *
 * Header for system-dependent functionality
 *
 */

#ifndef SYS_H_INCLUDED
#define SYS_H_INCLUDED

/**
 * Sleep for `mseconds` milliseconds.
 *
 * @param[in] mseconds number of milliseconds to sleep
 */
void
msleep(unsigned int mseconds);

#endif /* SYS_H_INCLUDED */
