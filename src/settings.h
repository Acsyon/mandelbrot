/* settings.h
 *
 * Header for (global) settings
 *
 */

#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/**
 * Struct for settings
 */
typedef struct {
    int width;
    int height;
    double max_absval;
    uint16_t max_itrs;
    double max_re;
    double min_re;
    double cntr_re;
    double cntr_im;
    double zoom_spd;
    double scrl_spd;
    unsigned long int prec;
} Settings;

/**
 * Pointer to global settings
 */
extern const Settings *const GLOBAL_SETTINGS;

/**
 * Creates newly malloc'd Settings object filled with the values in the JSON
 * config file in `fname`.
 *
 * @param[in] fname file name of config file to read values from
 *
 * @return newly malloc'd Settings objects
 */
Settings *
Settings_create_from_file(const char *fname);

/**
 * Fills Settings object `settings` with the values in the JSON config file
 * whose contents are stored in string `str`.
 *
 * @param[out] settings Settings object to write values into
 * @param[in] str contents of config file to read values from
 */
void
Settings_fill_from_string(Settings *settings, const char *str);

/**
 * Fills Settings object `settings` with the values in the JSON config file
 * opened in file stream `in`.
 *
 * @param[out] settings Settings object to write values into
 * @param[in] in stream of config file to read values from
 */
void
Settings_fread(Settings *settings, FILE *in);

/**
 * Fills Settings object `settings` with the values in the JSON config file in
 * `fname`.
 *
 * @param[out] settings Settings object to write values into
 * @param[in] fname file name of config file to read values from
 */
void
Settings_read(Settings *settings, const char *fname);

/**
 * Creates newly malloc'd string containing `settings` as a JSON object.
 *
 * @param[out] settings Settings object to convert to string
 *
 * @return newly malloc'd string containing `settings` as a JSON object
 */
char *
Settings_to_string(const Settings *settings);

/**
 * Writes `settings` as a JSON object to output stream `out`.
 *
 * @param[in] settings Settings object to write
 * @param[out] out output stream to write into
 */
void
Settings_fwrite(const Settings *settings, FILE *out);

/**
 * Writes `settings` as a JSON object to file `fname`.
 *
 * @param[in] settings Settings object to write
 * @param[in] out name of file to write into
 */
void
Settings_write(const Settings *settings, const char *fname);

/**
 * Frees memory pointed to by `settings`.
 *
 * @param[in] settings pointer to Settings object to be freed
 */
void
Settings_free(Settings *settings);

/**
 * Returns resolution according to parameters in `settings`
 * 
 * @param[in] settings Settings object to get resolution from
 * 
 * @return resolution according to parameters in `settings`
 */
double
Settings_get_resolution(const Settings *settings);

#endif /* SETTINGS_H_INCLUDED */
