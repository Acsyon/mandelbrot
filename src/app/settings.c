#include <app/settings.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#define DEFAULT_WIDTH UINT16_C(800)
#define DEFAULT_HEIGHT UINT16_C(600)

#define DEFAULT_MAXIMUM_ITERATIONS UINT16_C(500)
#define DEFAULT_NUMBER_CHUNKS_REAL UINT8_C(20)
#define DEFAULT_NUMBER_CHUNKS_IMAG UINT8_C(20)
#define DEFAULT_ZOOM_FACTOR (0.5)

#define DEFAULT_FPS UINT16_C(30)

#define DEFAULT_MAXIMUM_REAL (1.0)
#define DEFAULT_MINIMUM_REAL (-2.0)
#define DEFAULT_CENTRE_REAL (-0.5)
#define DEFAULT_CENTRE_IMAG (0.0)

#define DEFAULT_PALETTE_INDEX UINT8_C(4)
#define DEFAULT_TRIP_MODE UINT8_C(0)

#define DEFAULT_VIEW_FILENAME "view.json"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT UINT16_C(10101)

static const Settings DEFAULT_SETTINGS_OBJECT = {
  .width = DEFAULT_WIDTH,
  .height = DEFAULT_HEIGHT,
  .max_itrs = DEFAULT_MAXIMUM_ITERATIONS,
  .num_chnks_re = DEFAULT_NUMBER_CHUNKS_REAL,
  .num_chnks_im = DEFAULT_NUMBER_CHUNKS_IMAG,
  .zoom_fac = DEFAULT_ZOOM_FACTOR,
  .fps = DEFAULT_FPS,
  .max_re = DEFAULT_MAXIMUM_REAL,
  .min_re = DEFAULT_MINIMUM_REAL,
  .cntr_im = DEFAULT_CENTRE_IMAG,
  .palette_idx = DEFAULT_PALETTE_INDEX,
  .trip_mode = DEFAULT_TRIP_MODE,
  .view_file = DEFAULT_VIEW_FILENAME,
  .address = DEFAULT_ADDRESS,
  .port = DEFAULT_PORT,
};

const Settings *const DEFAULT_SETTINGS = &DEFAULT_SETTINGS_OBJECT;

Settings *
Settings_create(void)
{
    return Settings_duplicate(DEFAULT_SETTINGS);
}

void
Settings_free(Settings *settings)
{
    CUTIL_RETURN_IF_NULL(settings);

    free(settings->view_file);

    free(settings);
}

Settings *
Settings_duplicate(const Settings *settings)
{
    Settings *const dup = malloc(sizeof *dup);

    memcpy(dup, settings, sizeof *settings);
    dup->view_file = cutil_strdup(settings->view_file);
    dup->address = cutil_strdup(settings->address);

    return dup;
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

    JSON_TO_MEMBER(uint16_t, width);
    JSON_TO_MEMBER(uint16_t, height);

    JSON_TO_MEMBER(uint16_t, max_itrs);
    JSON_TO_MEMBER(uint8_t, num_chnks_re);
    JSON_TO_MEMBER(uint8_t, num_chnks_im);
    JSON_TO_MEMBER(double, zoom_fac);

    JSON_TO_MEMBER(uint16_t, fps);

    JSON_TO_MEMBER(double, max_re);
    JSON_TO_MEMBER(double, min_re);
    JSON_TO_MEMBER(double, cntr_im);

    JSON_TO_MEMBER(uint8_t, palette_idx);
    JSON_TO_MEMBER(uint8_t, trip_mode);

    JSON_TO_MEMBER(str, view_file);

    JSON_TO_MEMBER(str, address);
    JSON_TO_MEMBER(uint16_t, port);

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
            cutil_log_warn(                                                    \
              "Failed to create JSON field from Settings member '%s'", #MEMBER \
            );                                                                 \
        }                                                                      \
    } while (0)

    MEMBER_TO_JSON(uint16_t, width);
    MEMBER_TO_JSON(uint16_t, height);

    MEMBER_TO_JSON(uint16_t, max_itrs);
    MEMBER_TO_JSON(uint8_t, num_chnks_re);
    MEMBER_TO_JSON(uint8_t, num_chnks_im);
    MEMBER_TO_JSON(double, zoom_fac);

    MEMBER_TO_JSON(uint16_t, fps);

    MEMBER_TO_JSON(double, max_re);
    MEMBER_TO_JSON(double, min_re);
    MEMBER_TO_JSON(double, cntr_im);

    MEMBER_TO_JSON(uint8_t, palette_idx);
    MEMBER_TO_JSON(uint8_t, trip_mode);

    MEMBER_TO_JSON(str, view_file);

    MEMBER_TO_JSON(str, address);
    MEMBER_TO_JSON(uint16_t, port);

#undef MEMBER_TO_JSON

    return json;
}

Json *
Settings_to_Json_void(const void *vsettings)
{
    return Settings_to_Json(vsettings);
}

Settings *
Settings_consolidate_server_client(
  const Settings *settings_server, const Settings *settings_client
)
{
#define SET_FIELD_MIN(FIELD)                                                   \
    settings->FIELD = CUTIL_MIN(settings_server->FIELD, settings_client->FIELD)

    Settings *const settings = calloc(1UL, sizeof *settings);

    SET_FIELD_MIN(width);
    SET_FIELD_MIN(height);
    SET_FIELD_MIN(max_itrs);
    SET_FIELD_MIN(num_chnks_re);
    SET_FIELD_MIN(num_chnks_im);
    settings->zoom_fac = settings_client->zoom_fac;
    settings->max_re = settings_client->max_re;
    settings->min_re = settings_client->min_re;
    settings->cntr_im = settings_client->cntr_im;

    return settings;

#undef SET_FIELD_MIN
}
