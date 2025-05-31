/* net/package/settings.h
 *
 * Header for packages containing settings (initial handshake)
 *
 */

#ifndef MANDELBROT_NET_PACKAGE_SETTINGS_H_INCLUDED
#define MANDELBROT_NET_PACKAGE_SETTINGS_H_INCLUDED

#include <app/settings.h>
#include <net/package.h>

/**
 * Creates a new Package object for sending and receiving Settings (which is
 * taken as the initial handshake).
 *
 * @param[in] settings Settings object to initialize Package with
 *
 * @return new Package object for sending and receiving Settings
 */
Package *
Package_from_settings(const Settings *settings);

/**
 * Reads content of Package, interprets it as a JSON string and writes content
 * to Settings object.
 *
 * @param[in] pkg Package object to read data from
 * @param[out] settings Settings object to write data into
 */
void
Package_to_settings(const Package *pkg, Settings *settings);

#endif /* MANDELBROT_NET_PACKAGE_SETTINGS_H_INCLUDED */
