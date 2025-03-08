#include <stdlib.h>

#include "app.h"
#include "getopt.h"
#include "log.h"
#include "settings.h"
#include "util.h"

#define DEFAULT_SETTINGS_FILENAME "settings.json"

enum {
    LONGOPTS_ONLY_START_IDX = 1000,
    WIDTH_IDX,
    HEIGHT_IDX,
    MAX_RE_IDX,
    MIN_RE_IDX,
    CNTR_IM_IDX,
    MAX_ITRS_IDX,
    NUM_CHNKS_RE_IDX,
    NUM_CHNKS_IM_IDX,
    ZOOM_FAC_IDX,
    FPS_IDX,
    PALETTE_IDX_IDX,
    TRIP_MODE_IDX,
    LONGOPTS_ONLY_END_IDX,
};

static const struct option LONGOPTS[] = {
  {"help", NO_ARGUMENT, NULL, 'h'},
  {"env_path", REQUIRED_ARGUMENT, NULL, 'e'},
  {"load", REQUIRED_ARGUMENT, NULL, 'l'},
  {"save", REQUIRED_ARGUMENT, NULL, 's'},
  {"width", REQUIRED_ARGUMENT, NULL, WIDTH_IDX},
  {"height", REQUIRED_ARGUMENT, NULL, HEIGHT_IDX},
  {"max_re", REQUIRED_ARGUMENT, NULL, MAX_RE_IDX},
  {"min_re", REQUIRED_ARGUMENT, NULL, MIN_RE_IDX},
  {"cntr_im", REQUIRED_ARGUMENT, NULL, CNTR_IM_IDX},
  {"max_itrs", REQUIRED_ARGUMENT, NULL, MAX_ITRS_IDX},
  {"num_chnks_re", REQUIRED_ARGUMENT, NULL, NUM_CHNKS_RE_IDX},
  {"num_chnks_im", REQUIRED_ARGUMENT, NULL, NUM_CHNKS_IM_IDX},
  {"zoom_fac", REQUIRED_ARGUMENT, NULL, ZOOM_FAC_IDX},
  {"fps", REQUIRED_ARGUMENT, NULL, FPS_IDX},
  {"palette_idx", REQUIRED_ARGUMENT, NULL, PALETTE_IDX_IDX},
  {"trip_mode", REQUIRED_ARGUMENT, NULL, TRIP_MODE_IDX},
  {0, 0, 0, 0},
};

static const char *const SHORTOPTS = "he:l:s:";

static const char *const USAGE
  = "Usage: mandelbrot [OPTION]...\n"
    "Program to visualize the Mandelbrot set.\n"
    "\n"
    "Options:\n"
    "  -h, --help      Show this help message and quit.\n"
    "  -e, --env_path  Sets path of environment (for saving and loading) "
    "to work with\n"
    "\n"
    "The following two options are relative to \"-e\" and have no effect "
    "when \"-e\" is not set:\n"
    "  -l, --load      Sets name of file to load settings from, defaults to "
    "\"" DEFAULT_SETTINGS_FILENAME
    "\"\n"
    "  -s, --save      Sets name of file to save settings to\n"
    "\n"
    "Settings:\n"
    "      --width         Sets width of window in pixels.\n"
    "      --height        Sets height of window in pixels.\n"
    "      --max_re        Sets maximum value of real part\n"
    "      --min_re        Sets maximum value of imaginary part\n"
    "      --cntr_im       Sets imaginary part of centre point of window\n"
    "      --max_itrs      Sets maximum number of iterations to make\n"
    "      --num_chnks_re  Sets number of chunks in real direction\n"
    "      --num_chnks_im  Sets number of chunks in imaginary direction\n"
    "      --zoom_fac      Sets factor for one zoom stage\n"
    "      --fps           Sets frames per second (for intermediary updates)\n"
    "      --palette_idx   Sets start index for colour palette\n"
    "      --trip_mode     Sets \"trip mode\" type\n";

/**
 * Auxiliary struct for environment strings (path and file names)
 */
struct _env {
    char *path;
    char *load;
    char *save;
};

static struct _env *
_get_env(int argc, char **argv)
{
    struct _env *const env = malloc(sizeof *env);

    env->path = NULL;
    env->load = NULL;
    env->save = NULL;

    /* Parse command line input */
    optind = 0;
    for (;;) {
        int optidx = 0;
        const int c = getopt_long(argc, argv, SHORTOPTS, LONGOPTS, &optidx);
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'h': /* help */
            printf("%s", USAGE);
            exit(EXIT_SUCCESS);
        case 'e': /* env_path */
            env->path = Util_strdup(optarg);
            break;
        case 'l': /* load */
            env->load = Util_strdup(optarg);
            break;
        case 's': /* save */
            env->save = Util_strdup(optarg);
            break;
        default: /* bullshit or longopts only */
            if (c >= LONGOPTS_ONLY_START_IDX && c <= LONGOPTS_ONLY_END_IDX) {
                break;
            }
            printf("%s", USAGE);
            exit(EXIT_FAILURE);
        }
    }

    return env;
}

static void
_env_free(struct _env *env)
{
    if (env == NULL) {
        return;
    }

    free(env->path);
    free(env->load);
    free(env->save);

    free(env);
}

static char *
_concat_paths(const char *path1, const char *path2)
{
    const size_t bufsiz = snprintf(NULL, 0, "%s/%s", path1, path2) + 1;
    char *const buf = malloc(bufsiz * sizeof *buf);
    sprintf(buf, "%s/%s", path1, path2);
    return buf;
}

static Settings *
_get_settings(const struct _env *env, int argc, char **argv)
{
    Settings *const settings = Settings_duplicate(DEFAULT_SETTINGS);

    if (env->path != NULL) {
        const char *const fname
          = (env->load != NULL) ? env->load : DEFAULT_SETTINGS_FILENAME;
        char *const fname_full = _concat_paths(env->path, fname);
        JsonUtil_read(settings, fname_full, &Settings_fill_from_Json_void);
        free(fname_full);
    }

    /* Parse command line input */
    optind = 0;
    for (;;) {
        int optidx = 0;
        const int c = getopt_long(argc, argv, SHORTOPTS, LONGOPTS, &optidx);
        if (c == -1) {
            break;
        }
        switch (c) {
        case WIDTH_IDX: /* width*/
            settings->width = atoi(optarg);
            break;
        case HEIGHT_IDX: /* height*/
            settings->height = atoi(optarg);
            break;
        case MAX_RE_IDX: /* max_re */
            settings->max_re = atof(optarg);
            break;
        case MIN_RE_IDX: /* min_re */
            settings->min_re = atof(optarg);
            break;
        case CNTR_IM_IDX: /* cntr_im */
            settings->cntr_im = atof(optarg);
            break;
        case MAX_ITRS_IDX: /* max_itrs */
            settings->max_itrs = atoi(optarg);
            break;
        case NUM_CHNKS_RE_IDX: /* num_chnks_re */
            settings->num_chnks_re = atoi(optarg);
            break;
        case NUM_CHNKS_IM_IDX: /* num_chnks_im */
            settings->num_chnks_im = atoi(optarg);
            break;
        case ZOOM_FAC_IDX: /* zoom_fac */
            settings->zoom_fac = atof(optarg);
            break;
        case FPS_IDX: /* fps */
            settings->fps = atoi(optarg);
            break;
        case PALETTE_IDX_IDX: /* palette_idx */
            settings->palette_idx = atoi(optarg);
            break;
        case TRIP_MODE_IDX: /* trip_mode */
            settings->trip_mode = atoi(optarg);
            break;
        default: /* anything else has been handled before */
            break;
        }
    }

    if (env->path != NULL && env->save != NULL) {
        char *const fname_full = _concat_paths(env->path, env->save);
        JsonUtil_write(settings, fname_full, &Settings_to_Json_void);
        free(fname_full);
    }

    return settings;
}

int
main(int argc, char **argv)
{
    log_set_stream(stdout);

    struct _env *const env = _get_env(argc, argv);
    Settings *const settings = _get_settings(env, argc, argv);

    const int status = App_run(env->path, settings);

    Settings_free(settings);
    _env_free(env);

    return status;
}
