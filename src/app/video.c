#include <app/video.h>

#include <SDL2/SDL.h>
#include <omp.h>

#include <cutil/io/log.h>
#include <cutil/std/stdbool.h>
#include <cutil/util/macro.h>

#include <app/app.h>
#include <app/data.h>
#include <app/key.h>
#include <app/settings.h>
#include <util/num.h>
#include <util/sys.h>
#include <visuals/palette.h>

#define MSECONDS_PER_SEC 1000U

struct _video {
    Settings *settings;
    GraphicsData *gfxdata;

    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Surface *image;

    KeyBuffer *keybuf;
    bool should_close;
    bool has_changed;

    unsigned int msecs_per_frame;
    uint64_t last_frame;

    Palette_fnc *palette;
    const void *palette_params;
    PaletteCycler *cycler;
    TripModeGenerator *tripgen;
};

static void
_video_free(Video *video)
{
    CUTIL_RETURN_IF_NULL(video);

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
_video_alloc(const Settings *settings, GraphicsData *gfxdata)
{
    Video *const video = malloc(sizeof *video);

    video->settings = Settings_duplicate(settings);
    video->gfxdata = gfxdata;

    video->window = NULL;
    video->surface = NULL;
    video->image = NULL;

    video->keybuf = KeyBuffer_alloc();
    video->should_close = false;
    video->has_changed = true;

    video->msecs_per_frame = MSECONDS_PER_SEC / settings->fps;
    video->last_frame = SDL_GetTicks64();

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
        cutil_log_error(
          "Window could not be created, SDL_Error: %s", SDL_GetError()
        );
        _video_free(video);
        return NULL;
    }
    video->surface = SDL_GetWindowSurface(video->window);
    video->image = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    return video;
}

static void
_video_write_framebuffer(Video *video)
{
    SDL_LockSurface(video->image);

    const Settings *const settings = video->settings;
    GraphicsData *const gfxdata = video->gfxdata;
    const uint16_t width = settings->width;
    const uint16_t height = settings->height;

    const float *const pxdata = GraphicsData_get_pixel_data(gfxdata);
    uint32_t *const buf = video->image->pixels;
    for (uint16_t i = 0; i < width; ++i) {
        for (uint16_t j = 0; j < height; ++j) {
            const float pxval = pxdata[i * height + j];
            buf[j * width + i] = video->palette(pxval, video->palette_params);
        }
    }

    SDL_UnlockSurface(video->image);
}

static unsigned int
_video_get_frame_delta(const Video *video)
{
    const uint64_t next_frame = video->last_frame + video->msecs_per_frame;
    const uint64_t curr_frame = SDL_GetTicks64();
    return (next_frame > curr_frame) ? (next_frame - curr_frame) : 0U;
}

static void
_video_draw_image(Video *video)
{
    if (video->has_changed || GraphicsData_has_changed(video->gfxdata)) {
        _video_write_framebuffer(video);
        video->has_changed = false;
    }
    SDL_BlitSurface(video->image, NULL, video->surface, NULL);
    SDL_UpdateWindowSurface(video->window);

    const unsigned int delta = _video_get_frame_delta(video);
    msleep(delta);
    video->last_frame = SDL_GetTicks64();
}

static void
_video_cycle_palette(Video *video)
{
    video->palette = PaletteCycler_cycle_palette(video->cycler);
    video->palette_params = NULL;
    video->has_changed = true;
}

static void
_video_advance_trip_mode(Video *video)
{
    TripModeGenerator *const tripgen = video->tripgen;
    TripModeGenerator_advance(tripgen);
    video->palette = TripModeGenerator_get_palette(tripgen);
    video->palette_params = TripModeGenerator_get_params(tripgen);
    video->has_changed = true;
}

static inline void
_video_register_input_events(Video *video)
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
 */
static inline void
_video_process_video_actions(Video *video)
{
    enum Key key;
    while ((key = KeyBuffer_pop_key(video->keybuf, KEYCATEGORY_VIDEO))
           != KEY_INVALID)
    {
        switch (key) {
        case KEY_QUIT:
            video->should_close = true;
            GraphicsData_register_action(video->gfxdata, key);
            return;
        case KEY_CHANGE_PALETTE: {
            _video_cycle_palette(video);
        } break;
        case KEY_TRIP_MODE: {
            _video_advance_trip_mode(video);
        } break;
        default:
            break;
        }
    }
}

static inline void
_video_process_data_actions(Video *video)
{
    enum Key key;
    while ((key = KeyBuffer_pop_key(video->keybuf, KEYCATEGORY_DATA))
           != KEY_INVALID)
    {
        GraphicsData_register_action(video->gfxdata, key);
    }
}

static inline void
_video_perform_client_actions(Video *video)
{
    _video_draw_image(video);
    _video_register_input_events(video);
    _video_process_video_actions(video);
    _video_process_data_actions(video);
}

static void
_video_loop_sequential(Video *video)
{
    while (!video->should_close) {
        _video_perform_client_actions(video);

        const unsigned int delta = _video_get_frame_delta(video);
        GraphicsData_resume_action(video->gfxdata, delta);
    }
}

static void
_video_loop_concurrent(Video *video)
{
    omp_set_nested(1);
#pragma omp parallel
    {
#pragma omp master
        {
            while (!video->should_close) {
                _video_perform_client_actions(video);
            }
        }
#pragma omp single
        {
            while (!video->should_close) {
                GraphicsData_perform_action(video->gfxdata);
            }
        }
    }
}

static void
_video_loop(Video *video)
{
    const int tnum = omp_get_max_threads();
    if (tnum == 1) {
        _video_loop_sequential(video);
    } else {
        _video_loop_concurrent(video);
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
    GraphicsData *const gfxdata = App_get_graphics_data();
    return _video_alloc(settings, gfxdata);
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
