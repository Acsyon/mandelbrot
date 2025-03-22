/* util/sys.h
 *
 * Header for system-dependent functionality
 *
 */

#ifndef MANDELBROT_UTIL_SYS_H_INCLUDED
#define MANDELBROT_UTIL_SYS_H_INCLUDED

/**
 * Sleep for `mseconds` milliseconds.
 *
 * @param[in] mseconds number of milliseconds to sleep
 */
void
msleep(unsigned int mseconds);

#endif /* MANDELBROT_UTIL_SYS_H_INCLUDED */
