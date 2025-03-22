#include "app.h"

#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <app/video.h>
#include <util/log.h>
#include <util/util.h>

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
        log_wrn("App is already running.");
        return;
    }

    _app = calloc(1, sizeof *_app);

    _app->env_path = Util_strdup(env_path);
    _app->settings = Settings_duplicate(settings);
    _app->gfxdata = GraphicsData_app_init();
    _app->video = Video_app_init();
}

static void
_app_free(void)
{
    if (_app == NULL) {
        return;
    }

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
        log_err("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
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
    if (_app == NULL) {
        return NULL;
    }
    return _app->env_path;
}

const Settings *
App_get_settings(void)
{
    if (_app == NULL) {
        return NULL;
    }
    return _app->settings;
}

GraphicsData *
App_get_graphics_data(void)
{
    if (_app == NULL) {
        return NULL;
    }
    return _app->gfxdata;
}

Video *
App_get_video(void)
{
    if (_app == NULL) {
        return NULL;
    }
    return _app->video;
}
