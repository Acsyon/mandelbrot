#include <app/app.h>

#include <SDL2/SDL.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#include <app/video.h>

static const char *const DEFAULT_ENVIRONMENT_PATH = "../env/";

static struct {
    char *env_path;
    Settings *settings;
    GraphicsData *gfxdata;
    Video *video;
} *_app = NULL;

static void
_app_create(const char *env_path, const Settings *settings)
{
    if (_app != NULL) {
        cutil_log_warn("App is already running...");
        return;
    }

    _app = calloc(1, sizeof *_app);

    _app->env_path = cutil_strdup(env_path);
    _app->settings = Settings_duplicate(settings);
    _app->gfxdata = GraphicsData_app_init();
    _app->video = Video_app_init();
}

static void
_app_free(void)
{
    CUTIL_RETURN_IF_NULL(_app);

    free(_app->env_path);
    Settings_free(_app->settings);
    GraphicsData_free(_app->gfxdata);
    Video_free(_app->video);

    free(_app);
}

int
App_run(const char *env_path, const Settings *settings)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cutil_log_error(
          "SDL could not initialize, SDL_Error: %s", SDL_GetError()
        );
        SDL_Quit();
        return EXIT_FAILURE;
    }

    env_path = (env_path != NULL) ? env_path : DEFAULT_ENVIRONMENT_PATH;
    settings = (settings != NULL) ? settings : DEFAULT_SETTINGS;
    _app_create(env_path, settings);
    Video_loop(_app->video);
    _app_free();

    SDL_Quit();

    return EXIT_SUCCESS;
}

const char *
App_get_env_path(void)
{
    CUTIL_RETURN_NULL_IF_NULL(_app);
    return _app->env_path;
}

const Settings *
App_get_settings(void)
{
    CUTIL_RETURN_NULL_IF_NULL(_app);
    return _app->settings;
}

GraphicsData *
App_get_graphics_data(void)
{
    CUTIL_RETURN_NULL_IF_NULL(_app);
    return _app->gfxdata;
}

Video *
App_get_video(void)
{
    CUTIL_RETURN_NULL_IF_NULL(_app);
    return _app->video;
}
