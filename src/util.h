/* util.h
 *
 * Header for miscellaneous utility functions
 *
 */

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>

#include <gmp.h>

/**
 * Copies string `str` to newly malloc'd memory.
 *
 * @param[in] str string to be copied
 *
 * @return pointer to newly malloc'd copy of `str`
 */
char *
Util_strdup(const char *str);

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

#endif /* UTIL_H_INCLUDED */
