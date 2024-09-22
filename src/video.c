#include "video.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "data.h"
#include "settings.h"

#define FPS UINT64_C(30)

/**
 * Global video object
 */
static struct {
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Surface *image;
    bool do_redraw;
} *_video = NULL;

void
Video_init(void)
{
    if (_video != NULL) {
        return;
    }

    _video = malloc(sizeof *_video);

    _video->window = NULL;
    _video->surface = NULL;
    _video->image = NULL;
    _video->do_redraw = true;

    ImageData_init();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        Video_quit();
        return;
    }

    const int width = GLOBAL_SETTINGS->width;
    const int height = GLOBAL_SETTINGS->height;

    _video->window = SDL_CreateWindow(
      "mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,
      height, SDL_WINDOW_SHOWN
    );

    if (_video->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        Video_quit();
        return;
    }
    _video->surface = SDL_GetWindowSurface(_video->window);

    _video->image = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
}

void
Video_quit(void)
{
    if (_video == NULL) {
        return;
    }

    SDL_FreeSurface(_video->image);
    SDL_FreeSurface(_video->surface);
    SDL_DestroyWindow(_video->window);

    SDL_Quit();

    ImageData_free();

    free(_video);
    _video = NULL;
}

static inline uint32_t
_palette(float pos)
{
    return powf(pos, 1.0 / 1.5) * 255 * 0x00000001;
}

static void
_draw_image(void)
{
    SDL_LockSurface(_video->image);

    const int width = GLOBAL_SETTINGS->width;
    const int height = GLOBAL_SETTINGS->height;

    const float *const pxdata = get_pixel_data();
    uint32_t *const buf = _video->image->pixels;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            buf[j * width + i] = _palette(pxdata[i * height + j]);
        }
    }

    SDL_UnlockSurface(_video->image);
    SDL_BlitSurface(_video->image, NULL, _video->surface, NULL);
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
    case SDLK_KP_0:
    case SDLK_0:
        return KEY_RESET;
    default:
        return KEY_INVALID;
    }
}

void
Video_loop(void)
{
    SDL_Event e;
    for (;;) {
        if (_video->do_redraw) {
            _draw_image();
            _video->do_redraw = false;
        }
        SDL_UpdateWindowSurface(_video->window);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    return;
                default: {
                    const enum Key key = _keymap(e.key.keysym.sym);
                    ImageData_action(key);
                    _video->do_redraw = true;
                } break;
                }
            }
        }
    }
}
