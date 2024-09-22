#include "video.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "color.h"
#include "data.h"
#include "log.h"
#include "settings.h"
#include "sys.h"

#define FPS UINT8_C(30)

typedef struct {
    const Settings *settings;
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Surface *image;
    Palette_fnc *palette;
} _videoData;

static void
_videoData_free(_videoData *video)
{
    if (video == NULL) {
        return;
    }

    SDL_FreeSurface(video->image);
    SDL_FreeSurface(video->surface);
    SDL_DestroyWindow(video->window);

    free(video);
}

static _videoData *
_videoData_alloc(const Settings *settings)
{
    _videoData *const video = malloc(sizeof *video);

    video->settings = settings;
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
        _videoData_free(video);
        return NULL;
    }
    video->surface = SDL_GetWindowSurface(video->window);
    video->image = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    return video;
}

static void
_videoData_draw_image(_videoData *video)
{
    SDL_BlitSurface(video->image, NULL, video->surface, NULL);
}

static void
_videoData_write_framebuffer(_videoData *video)
{
    SDL_LockSurface(video->image);

    const Settings *const settings = video->settings;
    const int width = settings->width;
    const int height = settings->height;

    const float *const pxdata = ImageData_get_pixel_data();
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
    default:
        return KEY_INVALID;
    }
}

static void
_videoData_loop(_videoData *video)
{
    _videoData_write_framebuffer(video);
    SDL_Event event;
    for (;;) {
        _videoData_draw_image(video);
        SDL_UpdateWindowSurface(video->window);
        const int has_events = SDL_PollEvent(&event);
        if (has_events == 0) {
            msleep(1000 / FPS);
            continue;
        }
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        if (event.type == SDL_QUIT) {
            return;
        } else if (event.type == SDL_KEYDOWN) {
            const SDL_Keycode keycode = event.key.keysym.sym;
            switch (keycode) {
            case SDLK_ESCAPE:
            case SDLK_q:
                return;
            default: {
                const enum Key key = _keymap(keycode);
                const int rewrite_framebuffer = ImageData_action(key);
                if (rewrite_framebuffer != 0) {
                    _videoData_write_framebuffer(video);
                }
            } break;
            }
        }
    }
}

/**
 * Global Video object
 */
static _videoData *_video = NULL;

void
Video_init(void)
{
    if (_video != NULL) {
        return;
    }

    ImageData_init();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log_err("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        Video_quit();
        return;
    }

    _video = _videoData_alloc(GLOBAL_SETTINGS);
}

void
Video_quit(void)
{
    _videoData_free(_video);
    _video = NULL;
    SDL_Quit();
    ImageData_free();
}

void
Video_loop(void)
{
    _videoData_loop(_video);
}
