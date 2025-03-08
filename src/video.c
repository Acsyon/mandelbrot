#include "video.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "app.h"
#include "data.h"
#include "key.h"
#include "log.h"
#include "num.h"
#include "palette.h"
#include "settings.h"
#include "sys.h"

struct Video {
    Settings *settings;
    ImageData *imgdata;
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Surface *image;
    KeyBuffer *keybuf;
    Palette_fnc *palette;
    const void *palette_params;
    PaletteCycler *cycler;
    TripModeGenerator *tripgen;
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
    KeyBuffer_free(video->keybuf);
    PaletteCycler_free(video->cycler);
    TripModeGenerator_free(video->tripgen);

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
    video->keybuf = KeyBuffer_alloc();
    video->palette_params = NULL;
    video->cycler = PaletteCycler_create(
        PALETTE_FUNCTIONS, PALETTE_FUNCTION_COUNT, settings->palette_idx
    );
    video->palette = PaletteCycler_cycle_palette(video->cycler);
    video->tripgen = TripModeGenerator_create(settings->trip_mode);

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
            buf[j * width + i]
              = video->palette(pxdata[i * height + j], video->palette_params);
        }
    }

    SDL_UnlockSurface(video->image);
}

static void
_video_cycle_palette(Video *video)
{
    video->palette = PaletteCycler_cycle_palette(video->cycler);
    video->palette_params = NULL;
    _video_write_framebuffer(video);
}

static void
_video_advance_trip_mode(Video *video)
{
    TripModeGenerator *const tripgen = video->tripgen;
    TripModeGenerator_advance(tripgen);
    video->palette = TripModeGenerator_get_palette(tripgen);
    video->palette_params = TripModeGenerator_get_params(tripgen);
    _video_write_framebuffer(video);
}

static inline void
_video_register_events(Video *video)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        const int raw_key = event.key.keysym.sym;
        const enum Key key = KeyMap_map(DEFAULT_KEYMAP, raw_key);
        switch (event.type) {
        case SDL_QUIT:
            return;
        case SDL_KEYDOWN: {
            KeyBuffer_register_key_down(video->keybuf, key);
        } break;
        case SDL_KEYUP: {
            KeyBuffer_register_key_up(video->keybuf, key);
        } break;
        default:
            break;
        }
    }
}

/**
 * Auxiliary function to process video actions. Since quitting is considered a
 * video action, its return value indicates whether the application should
 * close. Needs `msecs_per_frame` to pause an appropriate time to make the "trip
 * mode" smooth.
 *
 * @param[in] video Video object to process video actions of
 * @param[in] msecs_per_frame milliseconds per frame
 *
 * @return should the application close?
 */
static inline bool
_video_process_video_actions(Video *video, int msecs_per_frame)
{
    enum Key key;
    while ((key = KeyBuffer_pop_key(video->keybuf, KEYCATEGORY_VIDEO))
           != KEY_INVALID)
    {
        switch (key) {
        case KEY_QUIT:
            return true;
        case KEY_CHANGE_PALETTE: {
            _video_cycle_palette(video);
        } break;
        case KEY_TRIP_MODE: {
            _video_advance_trip_mode(video);
            msleep(msecs_per_frame);
        } break;
        default:
            break;
        }
    }
    return false;
}

static inline void
_video_process_data_actions(Video *video, ImageData *imgdata)
{
    enum Key key;
    while ((key = KeyBuffer_pop_key(video->keybuf, KEYCATEGORY_DATA))
           != KEY_INVALID)
    {
        ImageData_register_action(imgdata, key);
    }
}

static void
_video_loop(Video *video)
{
    ImageData *const imgdata = video->imgdata;
    const Settings *const settings = video->settings;
    const unsigned int msecs_per_frame = 1000 / settings->fps;
    for (;;) {
        if (ImageData_perform_action(imgdata, msecs_per_frame)) {
            _video_write_framebuffer(video);
        }
        _video_draw_image(video);
        SDL_UpdateWindowSurface(video->window);
        _video_register_events(video);
        const bool close = _video_process_video_actions(video, msecs_per_frame);
        if (close) {
            return;
        }
        _video_process_data_actions(video, imgdata);
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
