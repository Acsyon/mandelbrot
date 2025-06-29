/* util/mpf.h
 *
 * Header for miscellaneous mpf-related functions
 *
 */

#ifndef MANDELBROT_UTIL_MPF_H_INCLUDED
#define MANDELBROT_UTIL_MPF_H_INCLUDED

#include <gmp.h>

/**
 * Opaque MpfStringBuilder type
 */
typedef struct _mpfStringBuilder MpfStringBuilder;

/**
 * Creates newly malloc'd MpfStringBuilder object with default initial
 * capacities.
 *
 * @return newly malloc'd MpfStringBuilder object
 */
MpfStringBuilder *
MpfStringBuilder_create(void);

/**
 * Frees memory pointed to by `mpfsb`.
 *
 * @param[in] mpfsb MpfStringBuilder object to be freed
 */
void
MpfStringBuilder_free(MpfStringBuilder *mpfsb);

/**
 * Uses MpfStringBuilder `mpfsb` to convert GMP floating point number `mpf` (in
 * base `base`) to a newly malloc'd string that can be converted back with
 * 'mpf_set_str'.
 *
 * @param[in] mpfsb MpfStringBuilder to perform conversion with
 * @param[in] mpf GMP floating point number to be converted to string
 * @param[in] base base of the number system `mpf` should be represented in
 *
 * @return newly malloc'd string of GMP floating point number
 */
char *
MpfStringBuilder_to_string(MpfStringBuilder *mpfsb, mpf_srcptr mpf, int base);

/**
 * Uses MpfStringBuilder `mpfsb` to convert GMP floating point number `mpf` (in
 * base `base`) to a string that can be converted back with 'mpf_set_str' and
 * copies it to a buffer `buf` whose size is `buflen`. If the buffer is too
 * small, no action is performed.
 *
 * @param[in] mpfsb MpfStringBuilder to perform conversion with
 * @param[in] mpf GMP floating point number to be converted to string
 * @param[in] base base of the number system `mpf` should be represented in
 * @param[in] buflen size of buffer
 * @param[in, out] buf buffer to write string to
 *
 * @return length of copied string (without NUL character) or 0 if buffer is too
 * small
 */
size_t
MpfStringBuilder_copy_string_to_buf(
  MpfStringBuilder *mpfsb, mpf_srcptr mpf, int base, size_t buflen, char *buf
);

/**
 * Uses MpfStringBuilder `mpfsb` to convert GMP floating point number `mpf` (in
 * base `base`) to a string that can be converted back with 'mpf_set_str' and
 * copies it to a buffer pointed to by `p_buf` whose size is pointed to by
 * `p_buflen` and performs potentially necessary (re)allocations. If any
 * (re)allocation happens and `p_buflen` is not NULL, the new size of the buffer
 * (including the NUL character) is written to *`p_buflen`. *`p_buf` needs to be
 * freed.
 *
 * @param[in] mpfsb MpfStringBuilder to perform conversion with
 * @param[in] mpf GMP floating point number to be converted to string
 * @param[in] base base of the number system `mpf` should be represented in
 * @param[in, out] p_buflen pointer to length of buffer to write new length to
 * (if it is not NULL)
 * @param[in, out] p_buf pointer to buffer to write string to
 *
 * @return length of copied string (without NUL character)
 */
size_t
MpfStringBuilder_copy_string(
  MpfStringBuilder *mpfsb,
  mpf_srcptr mpf,
  int base,
  size_t *p_buflen,
  char **p_buf
);

#endif /* MANDELBROT_UTIL_MPF_H_INCLUDED */
