#include "settings.h"

#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "log.h"

#define DEFAULT_MAXIMUM_ITERATIONS UINT16_C(500)
#define DEFAULT_NUMBER_CHUNKS_REAL 10
#define DEFAULT_NUMBER_CHUNKS_IMAG 10
#define DEFAULT_ZOOM_FACTOR 0.5

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_MAXIMUM_REAL 1.0
#define DEFAULT_MINIMUM_REAL -2.0
#define DEFAULT_CENTRE_REAL -0.5
#define DEFAULT_CENTRE_IMAG 0.0

static Settings _global_settings = {
  .max_itrs = DEFAULT_MAXIMUM_ITERATIONS,
  .num_chnks_re = DEFAULT_NUMBER_CHUNKS_REAL,
  .num_chnks_im = DEFAULT_NUMBER_CHUNKS_IMAG,
  .zoom_fac = DEFAULT_ZOOM_FACTOR,
  .width = DEFAULT_WIDTH,
  .height = DEFAULT_HEIGHT,
  .max_re = DEFAULT_MAXIMUM_REAL,
  .min_re = DEFAULT_MINIMUM_REAL,
  .cntr_re = DEFAULT_CENTRE_REAL,
  .cntr_im = DEFAULT_CENTRE_IMAG,
};

const Settings *const GLOBAL_SETTINGS = &_global_settings;

static cJSON *
_cJSON_read(const char *str)
{
    cJSON *const json = cJSON_Parse(str);
    if (json == NULL) {
        const char *errptr = cJSON_GetErrorPtr();
        log_err("Error while reading JSON before '%s'!\n", errptr);
    }
    return json;
}

static char *
_file_to_str(FILE *in)
{
    fseek(in, 0L, SEEK_END);
    const long fsize = ftell(in) + 1;
    rewind(in);

    char *const fstr = malloc(fsize * sizeof *fstr);
    if (fstr == NULL) {
        log_err("Cannot allocate memory for copy of file!\n");
        return NULL;
    }
    const size_t size = fread(fstr, 1UL, fsize, in);
    if (size != (size_t) fsize - 1) {
        log_err("Cannot copy contents of file!\n");
        free(fstr);
        return NULL;
    }
    return fstr;
}

static void
_settings_from_cJSON(Settings *settings, cJSON *json)
{
    const cJSON *item = NULL;

#define JSON_TO_MEMBER(member)                                                 \
    do {                                                                       \
        item = cJSON_GetObjectItemCaseSensitive(json, #member);                \
        if (item != NULL) {                                                    \
            settings->member = cJSON_GetNumberValue(item);                     \
        }                                                                      \
    } while (0)

    JSON_TO_MEMBER(max_itrs);
    JSON_TO_MEMBER(num_chnks_re);
    JSON_TO_MEMBER(num_chnks_im);
    JSON_TO_MEMBER(zoom_fac);

    JSON_TO_MEMBER(width);
    JSON_TO_MEMBER(height);
    JSON_TO_MEMBER(max_re);
    JSON_TO_MEMBER(min_re);
    JSON_TO_MEMBER(cntr_re);
    JSON_TO_MEMBER(cntr_im);

#undef JSON_TO_MEMBER
}

Settings *
Settings_create_from_file(const char *fname)
{
    Settings *const settings = malloc(sizeof *settings);

    Settings_read(settings, fname);

    return settings;
}

void
Settings_fill_from_string(Settings *settings, const char *str)
{
    cJSON *const json = _cJSON_read(str);
    if (json == NULL) {
        return;
    }
    _settings_from_cJSON(settings, json);
}

void
Settings_fread(Settings *settings, FILE *in)
{
    char *const str = _file_to_str(in);
    Settings_fill_from_string(settings, str);
    free(str);
}

void
Settings_read(Settings *settings, const char *fname)
{
    FILE *const in = fopen(fname, "r");
    if (in == NULL) {
        log_err("Cannot open file '%s'!\n", fname);
        return;
    }
    Settings_fread(settings, in);
    fclose(in);
}

static cJSON *
_cJSON_from_settings(const Settings *settings)
{
    cJSON *const json = cJSON_CreateObject();

    cJSON *item = NULL;

    /* This is evil and I love it */
#define MEMBER_TO_JSON(member)                                                 \
    do {                                                                       \
        item = cJSON_CreateNumber((double) settings->member);                  \
        if (item == NULL) {                                                    \
            goto err;                                                          \
        }                                                                      \
        cJSON_AddItemToObject(json, #member, item);                            \
    } while (0)

    MEMBER_TO_JSON(max_itrs);
    MEMBER_TO_JSON(num_chnks_re);
    MEMBER_TO_JSON(num_chnks_im);
    MEMBER_TO_JSON(zoom_fac);

    MEMBER_TO_JSON(width);
    MEMBER_TO_JSON(height);
    MEMBER_TO_JSON(max_re);
    MEMBER_TO_JSON(min_re);
    MEMBER_TO_JSON(cntr_re);
    MEMBER_TO_JSON(cntr_im);

#undef MEMBER_TO_JSON

    return json;
err:
    log_err("Error while creating JSON!\n");
    cJSON_Delete(json);
    return NULL;
}

char *
Settings_to_string(const Settings *settings)
{
    cJSON *const json = _cJSON_from_settings(settings);
    if (json == NULL) {
        return NULL;
    }
    char *const str = cJSON_Print(json);
    cJSON_Delete(json);
    return str;
}

void
Settings_fwrite(const Settings *settings, FILE *out)
{
    char *const str = Settings_to_string(settings);
    if (str == NULL) {
        return;
    }
    fprintf(out, "%s\n", str);
    free(str);
}

void
Settings_write(const Settings *settings, const char *fname)
{
    FILE *const out = fopen(fname, "w");
    if (out == NULL) {
        log_err("Cannot open file '%s'!\n", fname);
        return;
    }
    Settings_fwrite(settings, out);
    fclose(out);
}

void
Settings_free(Settings *settings)
{
    if (settings == NULL) {
        return;
    }

    free(settings);
}

double
Settings_get_units_per_pixel(const Settings *settings)
{
    const int width = settings->width;
    const double max_re = settings->max_re;
    const double min_re = settings->min_re;
    return (max_re - min_re) / width;
}
