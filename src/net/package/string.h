/* net/package/string.h
 *
 * Header for packages containing strings
 *
 */

#ifndef MANDELBROT_NET_PACKAGE_STRING_H_INCLUDED
#define MANDELBROT_NET_PACKAGE_STRING_H_INCLUDED

#include <net/package.h>

/**
 * vtable for Settings Package object
 */
extern const PackageType *const PACKAGE_TYPE_STRING;

/**
 * Creates a new Package object for sending and receiving strings.
 *
 * @param[in] str string to initialize Package with
 *
 * @return new Package object for sending and receiving strings
 *
 * @note Takes ownership of `str` so it is freed together with Package.
 */
Package *
Package_from_string(char *str);

/**
 * Returns a pointer to the string inside the Package.
 *
 * @param[in] pkg Package to return string of
 *
 * @return pointer to the string inside the Package
 */
const char *
Package_to_string(const Package *pkg);

#endif /* MANDELBROT_NET_PACKAGE_STRING_H_INCLUDED */
