#include "settings.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

#define DEFAULT_MAXIMUM_REAL 1.0
#define DEFAULT_MINIMUM_REAL -2.0
#define DEFAULT_CENTRE_REAL -0.5
#define DEFAULT_CENTRE_IMAG 0.0

#define DEFAULT_MAXIMUM_ITERATIONS UINT16_C(500)
#define DEFAULT_NUMBER_CHUNKS_REAL 20
#define DEFAULT_NUMBER_CHUNKS_IMAG 20
#define DEFAULT_ZOOM_FACTOR 0.5

#define DEFAULT_FPS UINT8_C(30)

#define DEFAULT_PALETTE_INDEX 4
#define DEFAULT_TRIP_MODE 0

static const Settings DEFAULT_SETTINGS_OBJECT = {
  .width = DEFAULT_WIDTH,
  .height = DEFAULT_HEIGHT,
  .max_re = DEFAULT_MAXIMUM_REAL,
  .min_re = DEFAULT_MINIMUM_REAL,
  .cntr_im = DEFAULT_CENTRE_IMAG,
  .max_itrs = DEFAULT_MAXIMUM_ITERATIONS,
  .num_chnks_re = DEFAULT_NUMBER_CHUNKS_REAL,
  .num_chnks_im = DEFAULT_NUMBER_CHUNKS_IMAG,
  .zoom_fac = DEFAULT_ZOOM_FACTOR,
  .fps = DEFAULT_FPS,
  .palette_idx = DEFAULT_PALETTE_INDEX,
  .trip_mode = DEFAULT_TRIP_MODE,
};

const Settings *const DEFAULT_SETTINGS = &DEFAULT_SETTINGS_OBJECT;

Settings *
Settings_create(void)
{
    Settings *const settings = malloc(sizeof *settings);

    return settings;
}

void
Settings_free(Settings *settings)
{
    if (settings == NULL) {
        return;
    }

    free(settings);
}

Settings *
Settings_duplicate(const Settings *settings)
{
    Settings *const dup = malloc(sizeof *dup);

    return memcpy(dup, settings, sizeof *settings);
}

double
Settings_get_center_real(const Settings *settings)
{
    return (settings->min_re + settings->max_re) / 2;
}

double
Settings_get_center_imag(const Settings *settings)
{
    return settings->cntr_im;
}

double
Settings_get_units_per_pixel(const Settings *settings)
{
    const int width = settings->width;
    const double max_re = settings->max_re;
    const double min_re = settings->min_re;
    return (max_re - min_re) / width;
}

void
Settings_fill_from_Json(Settings *settings, const Json *json)
{
#define JSON_TO_MEMBER(TYPE, MEMBER)                                           \
    Json_elem_to_##TYPE(json, #MEMBER, &settings->MEMBER)

    JSON_TO_MEMBER(int, width);
    JSON_TO_MEMBER(int, height);

    JSON_TO_MEMBER(double, max_re);
    JSON_TO_MEMBER(double, min_re);
    JSON_TO_MEMBER(double, cntr_im);

    JSON_TO_MEMBER(uint16_t, max_itrs);
    JSON_TO_MEMBER(int, num_chnks_re);
    JSON_TO_MEMBER(int, num_chnks_im);
    JSON_TO_MEMBER(double, zoom_fac);

    JSON_TO_MEMBER(uint16_t, fps);

    JSON_TO_MEMBER(int, palette_idx);
    JSON_TO_MEMBER(int, trip_mode);

#undef JSON_TO_MEMBER
}

void
Settings_fill_from_Json_void(void *vsettings, const Json *json)
{
    Settings_fill_from_Json(vsettings, json);
}

Json *
Settings_to_Json(const Settings *settings)
{
    Json *const json = Json_create();

#define MEMBER_TO_JSON(TYPE, MEMBER)                                           \
    do {                                                                       \
        const int status = Json_add_##TYPE(json, #MEMBER, settings->MEMBER);   \
        if (status != EXIT_SUCCESS) {                                          \
            log_err(                                                           \
              "Error while creating JSON from Settings member '%s'!\n",        \
              #MEMBER                                                          \
            );                                                                 \
            Json_delete(json);                                                 \
            return NULL;                                                       \
        }                                                                      \
    } while (0)

    MEMBER_TO_JSON(int, width);
    MEMBER_TO_JSON(int, height);

    MEMBER_TO_JSON(double, max_re);
    MEMBER_TO_JSON(double, min_re);
    MEMBER_TO_JSON(double, cntr_im);

    MEMBER_TO_JSON(uint16_t, max_itrs);
    MEMBER_TO_JSON(int, num_chnks_re);
    MEMBER_TO_JSON(int, num_chnks_im);
    MEMBER_TO_JSON(double, zoom_fac);

    MEMBER_TO_JSON(uint16_t, fps);

    MEMBER_TO_JSON(int, palette_idx);
    MEMBER_TO_JSON(int, trip_mode);

#undef MEMBER_TO_JSON

    return json;
}

Json *
Settings_to_Json_void(const void *vsettings)
{
    return Settings_to_Json(vsettings);
}
