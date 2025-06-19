#include <cutil/io/log.h>
#include <cutil/posix/getopt.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/string/builder.h>
#include <cutil/util/macro.h>

#include <app/app.h>
#include <app/settings.h>
#include <util/util.h>

#ifndef __STDC_IEC_559__
    #error "System does not support platform-independent floats"
#endif

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
    VIEW_FILE_IDX,
    ADDRESS_IDX,
    PORT_IDX,
    LONGOPTS_ONLY_END_IDX,
};

static const cutil_Option LONGOPTS[] = {
  {"help", CUTIL_OPTION_NO_ARGUMENT, NULL, 'h'},
  {"defaults", CUTIL_OPTION_NO_ARGUMENT, NULL, 'd'},
  {"env_path", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, 'e'},
  {"load", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, 'l'},
  {"save", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, 's'},
  {"width", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, WIDTH_IDX},
  {"height", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, HEIGHT_IDX},
  {"max_re", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, MAX_RE_IDX},
  {"min_re", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, MIN_RE_IDX},
  {"cntr_im", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, CNTR_IM_IDX},
  {"max_itrs", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, MAX_ITRS_IDX},
  {"num_chnks_re", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, NUM_CHNKS_RE_IDX},
  {"num_chnks_im", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, NUM_CHNKS_IM_IDX},
  {"zoom_fac", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, ZOOM_FAC_IDX},
  {"fps", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, FPS_IDX},
  {"palette_idx", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, PALETTE_IDX_IDX},
  {"trip_mode", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, TRIP_MODE_IDX},
  {"view_file", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, VIEW_FILE_IDX},
  {"address", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, ADDRESS_IDX},
  {"port", CUTIL_OPTION_REQUIRED_ARGUMENT, NULL, PORT_IDX},
  {0, 0, 0, 0},
};

static const char *const SHORTOPTS = "hde:l:s:";

static const char *const USAGE
  = "Usage: mandelbrot [OPTION]...\n"
    "Program to visualize the Mandelbrot set.\n"
    "\n"
    "Options:\n"
    "  -h, --help      Show this help message and quit.\n"
    "  -d, --defaults  Show default settings as JSON and quit.\n"
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
    "      --trip_mode     Sets \"trip mode\" type\n"
    "      --view_file     Sets name of file to save view to (relative to "
    "env)\n"
    "      --address       Sets address of server\n"
    "      --port          Sets port of server\n";

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
    cutil_optind = 0;
    for (;;) {
        int optidx = 0;
        const int c
          = cutil_getopt_long(argc, argv, SHORTOPTS, LONGOPTS, &optidx);
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'h': /* help */
            printf("%s", USAGE);
            exit(EXIT_SUCCESS);
        case 'd': /* defaults */
            printf("mandelbrot default settings:\n");
            jsonutil_fwrite(DEFAULT_SETTINGS, stdout, &Settings_to_Json_void);
            exit(EXIT_SUCCESS);
        case 'e': /* env_path */
            env->path = cutil_strdup(cutil_optarg);
            break;
        case 'l': /* load */
            env->load = cutil_strdup(cutil_optarg);
            break;
        case 's': /* save */
            env->save = cutil_strdup(cutil_optarg);
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
    CUTIL_RETURN_IF_NULL(env);

    free(env->path);
    free(env->load);
    free(env->save);

    free(env);
}

static Settings *
_get_settings(const struct _env *env, int argc, char **argv)
{
    Settings *const settings = Settings_duplicate(DEFAULT_SETTINGS);

    if (env->path != NULL) {
        const char *const fname
          = (env->load != NULL) ? env->load : DEFAULT_SETTINGS_FILENAME;
        char *const fname_full = util_concat_paths(env->path, fname);
        jsonutil_read(settings, fname_full, &Settings_fill_from_Json_void);
        free(fname_full);
    }

    /* Parse command line input */
    cutil_optind = 0;
    for (;;) {
        int optidx = 0;
        const int c
          = cutil_getopt_long(argc, argv, SHORTOPTS, LONGOPTS, &optidx);
        if (c == -1) {
            break;
        }
        switch (c) {
        case WIDTH_IDX: /* width*/
            settings->width = atoi(cutil_optarg);
            break;
        case HEIGHT_IDX: /* height*/
            settings->height = atoi(cutil_optarg);
            break;
        case MAX_RE_IDX: /* max_re */
            settings->max_re = atof(cutil_optarg);
            break;
        case MIN_RE_IDX: /* min_re */
            settings->min_re = atof(cutil_optarg);
            break;
        case CNTR_IM_IDX: /* cntr_im */
            settings->cntr_im = atof(cutil_optarg);
            break;
        case MAX_ITRS_IDX: /* max_itrs */
            settings->max_itrs = atoi(cutil_optarg);
            break;
        case NUM_CHNKS_RE_IDX: /* num_chnks_re */
            settings->num_chnks_re = atoi(cutil_optarg);
            break;
        case NUM_CHNKS_IM_IDX: /* num_chnks_im */
            settings->num_chnks_im = atoi(cutil_optarg);
            break;
        case ZOOM_FAC_IDX: /* zoom_fac */
            settings->zoom_fac = atof(cutil_optarg);
            break;
        case FPS_IDX: /* fps */
            settings->fps = atoi(cutil_optarg);
            break;
        case PALETTE_IDX_IDX: /* palette_idx */
            settings->palette_idx = atoi(cutil_optarg);
            break;
        case TRIP_MODE_IDX: /* trip_mode */
            settings->trip_mode = atoi(cutil_optarg);
            break;
        case VIEW_FILE_IDX: /* view_file */
            free(settings->view_file);
            settings->view_file = cutil_strdup(cutil_optarg);
            break;
        case ADDRESS_IDX: /* address */
            free(settings->address);
            settings->address = cutil_strdup(cutil_optarg);
            break;
        case PORT_IDX: /* view_file */
            settings->port = atoi(cutil_optarg);
            break;
        default: /* anything else has been handled before */
            break;
        }
    }

    if (env->path != NULL && env->save != NULL) {
        char *const fname_full = util_concat_paths(env->path, env->save);
        jsonutil_write(settings, fname_full, &Settings_to_Json_void);
        free(fname_full);
    }

    return settings;
}

int
main(int argc, char **argv)
{
    cutil_Logger *const log = cutil_Logger_create(CUTIL_LOG_TRACE);
    cutil_Logger_add_handler(log, stdout, CUTIL_LOG_TRACE);
    cutil_set_global_logger(log);

    struct _env *const env = _get_env(argc, argv);
    Settings *const settings = _get_settings(env, argc, argv);

    const int status = App_run(env->path, settings);

    Settings_free(settings);
    _env_free(env);

    return status;
}
