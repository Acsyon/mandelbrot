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
    int width;         /* Width of window in pixels */
    int height;        /* Height of window in pixels */
    double max_absval; /* Maximum absolute value to cut Mandelbrot iteration */
    uint16_t max_itrs; /* Maximum number of Mandelbrot iterations to make*/
    double max_re;     /* Maximum value of real part */
    double min_re;     /* Maximum value of imaginary part */
    double cntr_re;    /* Real part of centre point of window */
    double cntr_im;    /* Imaginary part of centre point of window */
    double zoom_fac;   /* Factor for one zoom stage */
    int num_chnks_re;  /* Number of chunks in real direction */
    int num_chnks_im;  /* Number of chunks in imaginary direction */
    unsigned long int prec; /* Precision for GMP */
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
 * Returns number of mathematical length units per pixel according to parameters
 * in `settings`
 *
 * @param[in] settings Settings object to get units per pixel from
 *
 * @return units per pixel according to parameters in `settings`
 */
double
Settings_get_units_per_pixel(const Settings *settings);

#endif /* SETTINGS_H_INCLUDED */
