#include "video.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "app.h"
#include "color.h"
#include "data.h"
#include "log.h"
#include "settings.h"
#include "sys.h"

struct Video {
    Settings *settings;
    ImageData *imgdata;
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Surface *image;
    Palette_fnc *palette;
};

static void
_video_free(Video *video)
{
    if (video == NULL) {
        return;
    }

    SDL_FreeSurface(video->image);
    SDL_FreeSurface(video->surface);
    SDL_DestroyWindow(video->window);

    Settings_free(video->settings);

    free(video);
}

static Video *
_video_alloc(const Settings *settings, ImageData *imgdata)
{
    Video *const video = malloc(sizeof *video);

    video->settings = Settings_duplicate(settings);
    video->imgdata = imgdata;
    video->window = NULL;
    video->surface = NULL;
    video->image = NULL;
    video->palette = &Palette_exp_hsv;

    const int width = settings->width;
    const int height = settings->height;

    video->window = SDL_CreateWindow(
      "mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,
      height, SDL_WINDOW_SHOWN
    );
    if (video->window == NULL) {
        log_err("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        _video_free(video);
        return NULL;
    }
    video->surface = SDL_GetWindowSurface(video->window);
    video->image = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    return video;
}

static void
_video_draw_image(Video *video)
{
    SDL_BlitSurface(video->image, NULL, video->surface, NULL);
}

static void
_video_write_framebuffer(Video *video)
{
    SDL_LockSurface(video->image);

    const Settings *const settings = video->settings;
    const ImageData *const imgdata = video->imgdata;
    const int width = settings->width;
    const int height = settings->height;

    const float *const pxdata = ImageData_get_pixel_data(imgdata);
    uint32_t *const buf = video->image->pixels;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            buf[j * width + i] = video->palette(pxdata[i * height + j]);
        }
    }

    SDL_UnlockSurface(video->image);
}

static inline enum Key
_keymap(SDL_Keycode sdl_key)
{
    switch (sdl_key) {
    case SDLK_KP_PLUS:
    case SDLK_PLUS:
        return KEY_ZOOM_IN;
    case SDLK_KP_MINUS:
    case SDLK_MINUS:
        return KEY_ZOOM_OUT;
    case SDLK_w:
    case SDLK_UP:
        return KEY_UP;
    case SDLK_s:
    case SDLK_DOWN:
        return KEY_DOWN;
    case SDLK_a:
    case SDLK_LEFT:
        return KEY_LEFT;
    case SDLK_d:
    case SDLK_RIGHT:
        return KEY_RIGHT;
    case SDLK_r:
    case SDLK_KP_0:
    case SDLK_0:
        return KEY_RESET;
    case SDLK_F5:
        return KEY_VIEW_SAVE;
    case SDLK_F9:
        return KEY_VIEW_LOAD;
    default:
        return KEY_INVALID;
    }
}

static void
_video_loop(Video *video)
{
    ImageData *const imgdata = video->imgdata;
    const Settings *const settings = video->settings;
    const unsigned int MSECONDS_PER_FRAME = 1000 / settings->fps;
    SDL_Event event;
    for (;;) {
        if (ImageData_perform_action(imgdata, MSECONDS_PER_FRAME)) {
            _video_write_framebuffer(video);
        }
        _video_draw_image(video);
        SDL_UpdateWindowSurface(video->window);

        SDL_PollEvent(&event);
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        if (event.type == SDL_QUIT) {
            return;
        }
        if (event.type == SDL_KEYDOWN) {
            const SDL_Keycode keycode = event.key.keysym.sym;
            switch (keycode) {
            case SDLK_ESCAPE:
            case SDLK_q:
                return;
            default: {
                const enum Key key = _keymap(keycode);
                ImageData_register_action(imgdata, key);
            } break;
            }
        }
    }
}

Video *
Video_app_init(void)
{
    Video *const video = App_get_video();
    if (video != NULL) {
        return video;
    }
    const Settings *const settings = App_get_settings();
    ImageData *const imgdata = App_get_image_data();
    return _video_alloc(settings, imgdata);
}

void
Video_free(Video *video)
{
    _video_free(video);
}

void
Video_loop(Video *video)
{
    _video_loop(video);
}
