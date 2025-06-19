/* app/settings.h
 *
 * Header for (global) settings
 *
 */

#ifndef MANDELBROT_APP_SETTINGS_H_INCLUDED
#define MANDELBROT_APP_SETTINGS_H_INCLUDED

#include <util/json.h>

/**
 * Struct for settings
 */
typedef struct {
    uint16_t width;       /* Width of window in pixels */
    uint16_t height;      /* Height of window in pixels */
    uint16_t max_itrs;    /* Maximum number of Mandelbrot iterations to make */
    uint8_t num_chnks_re; /* Number of chunks in real direction */
    uint8_t num_chnks_im; /* Number of chunks in imaginary direction */
    double zoom_fac;      /* Factor for one zoom stage */
    uint16_t fps;         /* Frames per second (for intermediary updates) */
    double max_re;        /* Maximum value of real part */
    double min_re;        /* Maximum value of imaginary part */
    double cntr_im;       /* Imaginary part of centre point of window */
    uint8_t palette_idx;  /* Default index of colour palette*/
    uint8_t trip_mode;    /* Type of trip mode */
    char *view_file;      /* File to save view to */
    char *address;        /* Address of server */
    uint16_t port;        /* Port of server */
} Settings;

/**
 * Pointer to default settings object
 */
extern const Settings *const DEFAULT_SETTINGS;

/**
 * Creates newly malloc'd (and default-initialized) Settings object.
 *
 * @return pointer to newly malloc'd (and default-initialized) Settings object
 */
Settings *
Settings_create(void);

/**
 * Frees memory pointed to by `settings`.
 *
 * @param[in] settings pointer to Settings object to be freed
 */
void
Settings_free(Settings *settings);

/**
 * Copies Settings object `settings` to newly malloc'd memory.
 *
 * @param[in] settings Settings object to be copied
 *
 * @return pointer to newly malloc'd copy of `settings`
 */
Settings *
Settings_duplicate(const Settings *settings);

/**
 * Returns real component of centre point according to parameters in `settings`.
 *
 * @param[in] settings Settings object to get real component of centre of
 *
 * @return real component of centre point according to parameters in `settings`
 */
double
Settings_get_center_real(const Settings *settings);

/**
 * Returns imaginary component of centre point according to parameters in
 * `settings`.
 *
 * @param[in] settings Settings object to get imaginary component of centre of
 *
 * @return imaginary component of centre point according to parameters in
 * `settings`
 */
double
Settings_get_center_imag(const Settings *settings);

/**
 * Returns number of mathematical length units per pixel according to parameters
 * in `settings`.
 *
 * @param[in] settings Settings object to get units per pixel from
 *
 * @return units per pixel according to parameters in `settings`
 */
double
Settings_get_units_per_pixel(const Settings *settings);

/**
 * Fills members of `settings` according to content of `json`.
 *
 * @param[in, out] settings Settings object to fill
 * @param[in] json Json object to get data from
 */
void
Settings_fill_from_Json(Settings *settings, const Json *json);

/**
 * Fills members of `vsettings` according to content of `json`. Version with
 * void pointer to be used as a 'JsonUtilReadCallback'.
 *
 * @param[in, out] vsettings Settings object (as void pointer) to fill
 * @param[in] json Json object to get data from
 */
void
Settings_fill_from_Json_void(void *vsettings, const Json *json);

/**
 * Returns Json object with content of `settings`.
 *
 * @param[in] settings Settings object to read data from
 *
 * @return Json object with content of `settings`
 */
Json *
Settings_to_Json(const Settings *settings);

/**
 * Returns Json object with content of `settings`. Version with void pointer to
 * be used as a 'JsonUtilWriteCallback'.
 *
 * @param[in] vsettings Settings object (as void pointer) to read data from
 *
 * @return Json object with content of `settings`
 */
Json *
Settings_to_Json_void(const void *vsettings);

/**
 * Creates newly malloc'd Settings object by consolidating the
 * `settings_server`, which are taken to be the upper limit of the ressources
 * the client might request, and the `client_settings`.
 *
 * @param[in] settings_server Settings object of server to be consolidated
 * @param[in] settings_client Settings object of client to be consolidated
 *
 * @return pointer to newly malloc'd, consolidated Settings object
 */
Settings *
Settings_consolidate_server_client(
  const Settings *settings_server, const Settings *settings_client
);

#endif /* MANDELBROT_APP_SETTINGS_H_INCLUDED */
