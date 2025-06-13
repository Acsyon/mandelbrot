/* util/util.h
 *
 * Header for miscellaneous utility functions
 *
 */

#ifndef MANDELBROT_UTIL_UTIL_H_INCLUDED
#define MANDELBROT_UTIL_UTIL_H_INCLUDED

#include <gmp.h>

#include <cutil/std/stdio.h>

/**
 * Reads contents of file into newly malloc'd string.
 *
 * @param[in] in FILE stream to read data from
 *
 * @return newly malloc'd string with contents of `in` (NULL for error)
 */
char *
Util_file_to_str(FILE *in);

/**
 * Converts GMP floating point number `mpf` (in base 10) to a newly malloc'd
 * string that can be converted back with 'mpf_set_str'.
 *
 * @param[in] mpf GMP floating point number to be converted to string
 *
 * @return newly malloc'd string of GMP floating point number
 */
char *
Util_mpf_to_str_base10(mpf_srcptr mpf);

/**
 * Calculates new precision based on `upp`.
 *
 * @param[in] upp units per pixel to calculate new precision from
 *
 * @return new precision
 */
mp_bitcnt_t
Util_calculate_new_prec(mpf_srcptr upp);

#endif /* MANDELBROT_UTIL_UTIL_H_INCLUDED */
