#include "image.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_timer.h>
#include <gmp.h>
#include <omp.h>

#include <cutil/log.h>

#include <app/app.h>
#include <data/pixel.h>
#include <util/sys.h>
#include <util/util.h>
#include <visuals/palette.h>
#include <visuals/view.h>

#define INITIAL_PRECISION GMP_LIMB_BITS
#define ITERATION_CUTOFF_ABSOLUTE_VALUE 2.0

/**
 * Possible data states
 */
enum DataState {
    DATA_STATE_WORKING = -1,
    DATA_STATE_IDLE = 0,
};

/**
 * ImageData container struct
 */
struct ImageData {
    Settings *settings;
    mp_bitcnt_t prec;
    View *view;
    mpf_t action_buf;
    PixelData *data;
    int tnum;
    PixelDataBuffer *tbuf;
    ChunkData chunks;
    float *framebuf;
    enum DataState state;
    uint64_t target_ticks;
    char *view_fname;
};

static void
_imageData_init_mpf(ImageData *imgdata)
{
    mpf_init(imgdata->action_buf);
}

static void
_imageData_init_view(ImageData *imgdata)
{
    const Settings *const settings = imgdata->settings;
    imgdata->view = View_create();
    View_fill_from_Settings(imgdata->view, settings);
}

static void
_imageData_init_data(ImageData *imgdata)
{
    const Settings *const settings = imgdata->settings;
    const int num_tot = settings->width * settings->height;
    imgdata->data = malloc(num_tot * sizeof *imgdata->data);
    for (int i = 0; i < num_tot; ++i) {
        PixelData_init(&imgdata->data[i]);
    }
    imgdata->framebuf = malloc(num_tot * sizeof *imgdata->framebuf);
}

static void
_imageData_init_tbuf(ImageData *imgdata)
{
    const int tnum = imgdata->tnum = omp_get_max_threads();
    imgdata->tbuf = malloc(tnum * sizeof *imgdata->tbuf);

    mpf_t max_sqr;
    mpf_init_set_d(max_sqr, ITERATION_CUTOFF_ABSOLUTE_VALUE);
    mpf_mul(max_sqr, max_sqr, max_sqr);
    for (int i = 0; i < tnum; ++i) {
        PixelDataBuffer *const buf = &imgdata->tbuf[i];
        PixelDataBuffer_init(buf, max_sqr);
    }
    mpf_clear(max_sqr);
}

static void
_imageData_init_chunks(ImageData *imgdata)
{
    ChunkData *const chunks = &imgdata->chunks;
    const Settings *const settings = imgdata->settings;
    PixelData *const data = imgdata->data;
    ChunkData_init(chunks, settings, data);
}

static void
_imageData_init_view_fname(ImageData *imgdata)
{
    const char *const path = App_get_env_path();
    char *const fname = imgdata->settings->view_file;
    const size_t bufsiz = snprintf(NULL, 0, "%s/%s", path, fname) + 1;
    imgdata->view_fname = malloc(bufsiz * sizeof *imgdata->view_fname);
    snprintf(imgdata->view_fname, bufsiz, "%s/%s", path, fname);
}

static ImageData *
_imageData_alloc(const Settings *settings)
{
    ImageData *const imgdata = malloc(sizeof *imgdata);

    imgdata->settings = Settings_duplicate(settings);
    imgdata->prec = INITIAL_PRECISION;
    mpf_set_default_prec(INITIAL_PRECISION);

    _imageData_init_mpf(imgdata);
    _imageData_init_view(imgdata);
    _imageData_init_data(imgdata);
    _imageData_init_tbuf(imgdata);
    _imageData_init_chunks(imgdata);
    _imageData_init_view_fname(imgdata);

    imgdata->state = DATA_STATE_WORKING;
    imgdata->target_ticks = 0;

    return imgdata;
}

static void
_imageData_clear_mpf(ImageData *imgdata)
{
    mpf_clear(imgdata->action_buf);
}

static void
_imageData_clear_view(ImageData *imgdata)
{
    View_free(imgdata->view);
}

static void
_imageData_clear_data(ImageData *imgdata)
{
    const Settings *const settings = imgdata->settings;
    const int num_tot = settings->width * settings->height;
    for (int i = 0; i < num_tot; ++i) {
        PixelData_clear(&imgdata->data[i]);
    }
    free(imgdata->data);
    free(imgdata->framebuf);
    Settings_free(imgdata->settings);
    free(imgdata->view_fname);
}

static void
_imageData_clear_tbuf(ImageData *imgdata)
{
    const int tnum = imgdata->tnum;
    for (int i = 0; i < tnum; ++i) {
        PixelDataBuffer_clear(&imgdata->tbuf[i]);
    }
    free(imgdata->tbuf);
}

static void
_imageData_clear_chunks(ImageData *imgdata)
{
    ChunkData *const chunks = &imgdata->chunks;
    ChunkData_clear(chunks);
}

static void
_imageData_clear(ImageData *imgdata)
{
    _imageData_clear_mpf(imgdata);
    _imageData_clear_view(imgdata);
    _imageData_clear_data(imgdata);
    _imageData_clear_tbuf(imgdata);
    _imageData_clear_chunks(imgdata);
}

static void
_imageData_set_prec_mpf(ImageData *imgdata)
{
    const mp_bitcnt_t prec = imgdata->prec;
    mpf_set_prec(imgdata->action_buf, prec);
}

static void
_imageData_set_prec_view(ImageData *imgdata)
{
    const mp_bitcnt_t prec = imgdata->prec;
    View_set_precision(imgdata->view, prec);
}

static void
_imageData_set_prec_data(ImageData *imgdata)
{
    const Settings *const settings = imgdata->settings;
    const int num_tot = settings->width * settings->height;
    const mp_bitcnt_t prec = imgdata->prec;
    for (int i = 0; i < num_tot; ++i) {
        PixelData_set_prec(&imgdata->data[i], prec);
    }
}

static void
_imageData_set_prec_tbuf(ImageData *imgdata)
{
    const mp_bitcnt_t prec = imgdata->prec;
    const int tnum = imgdata->tnum;
    for (int i = 0; i < tnum; ++i) {
        PixelDataBuffer *const buf = &imgdata->tbuf[i];
        PixelDataBuffer_set_prec(buf, prec);
    }
}

static void
_imageData_set_prec(ImageData *imgdata, mp_bitcnt_t prec)
{
    imgdata->prec = prec;
    _imageData_set_prec_mpf(imgdata);
    _imageData_set_prec_view(imgdata);
    _imageData_set_prec_data(imgdata);
    _imageData_set_prec_tbuf(imgdata);
}

static void
_imageData_update_chunk_pixels(
  const ImageData *imgdata, PixelChunk *chunk, int idx_px_re, int idx_px_im
)
{
    const ChunkData *const chunks = &imgdata->chunks;
    const ChunkParams *const params = &chunks->params;

    const int stride = params->stride;
    const int idx_px = idx_px_re * stride + idx_px_im;
    PixelData *const px = &chunk->data[idx_px];

    if (px->state == PIXEL_STATE_VALID) {
        return;
    }

    PixelDataBuffer *const tbuf = imgdata->tbuf;
    const Settings *const settings = imgdata->settings;

    const View *const view = imgdata->view;
    const mpf_srcptr cntr_re = view->cntr_re;
    const mpf_srcptr cntr_im = view->cntr_im;
    const mpf_srcptr upp = view->upp;

    const int num_re = settings->width;
    const int num_im = settings->height;
    const int idx_cntr_re = num_re / 2;
    const int idx_cntr_im = num_im / 2;

    const int idx_re = chunk->idx_re * params->num_px_re + idx_px_re;
    const int idx_im = chunk->idx_im * params->num_px_im + idx_px_im;

    mpf_set_d(px->re, (1.0 * (idx_re - idx_cntr_re)));
    mpf_mul(px->re, px->re, upp);
    mpf_add(px->re, px->re, cntr_re);

    mpf_set_d(px->im, (1.0 * (idx_im - idx_cntr_im)));
    mpf_mul(px->im, px->im, upp);
    mpf_add(px->im, px->im, cntr_im);

    const int tid = omp_get_thread_num();
    PixelDataBuffer *const buf = &tbuf[tid];
    PixelData_iterate(px, buf, settings->max_itrs);

    px->state = PIXEL_STATE_VALID;
}

static void
_imageData_apply_to_all_chunks(
  ImageData *imgdata, PixelChunk_callback *callback, const void *vparams
)
{
    ChunkData *const chunks = &imgdata->chunks;
    const int num_tot = chunks->num_re * chunks->num_im;
#pragma omp parallel for
    for (int idx = 0; idx < num_tot; ++idx) {
        PixelChunk *const chunk = &chunks->data[idx];
        callback(chunk, chunks, vparams);
    }
}

static void
_imageData_register_zoom(ImageData *imgdata, int stages)
{
    const Settings *const settings = imgdata->settings;
    const mpf_ptr buf = imgdata->action_buf;

    View *const view = imgdata->view;
    const mpf_ptr upp = view->upp;

    mpf_set_d(buf, settings->zoom_fac);
    for (int i = 0; i < abs(stages); ++i) {
        (stages > 0) ? mpf_mul(upp, upp, buf) : mpf_div(upp, upp, buf);
    }

    const mp_bitcnt_t new_prec = Util_calculate_new_prec(upp);
    if (new_prec != imgdata->prec) {
        _imageData_set_prec(imgdata, new_prec);
    }

    PixelChunk_callback *const callback = &PixelChunk_callback_zoom;
    _imageData_apply_to_all_chunks(imgdata, callback, &stages);

    cutil_log_debug("Performed zoom: %i", stages);
}

static void
_imageData_register_shift(ImageData *imgdata, int shift_re, int shift_im)
{
    const Settings *const settings = imgdata->settings;
    const mpf_ptr buf = imgdata->action_buf;

    View *const view = imgdata->view;
    const mpf_ptr cntr_re = view->cntr_re;
    const mpf_ptr cntr_im = view->cntr_im;
    const mpf_ptr upp = view->upp;

    if (shift_re != 0) {
        const int num_px_re = settings->width / settings->num_chnks_re;
        const long int offs_re = shift_re * num_px_re;
        mpf_set_si(buf, offs_re);
        mpf_mul(buf, buf, upp);
        mpf_add(cntr_re, cntr_re, buf);
    }

    if (shift_im != 0) {
        const int num_px_im = settings->height / settings->num_chnks_im;
        const long int offs_im = shift_im * num_px_im;
        mpf_set_si(buf, offs_im);
        mpf_mul(buf, buf, upp);
        mpf_add(cntr_im, cntr_im, buf);
    }

    PixelChunk_callback *const callback = &PixelChunk_callback_shift;
    const int shifts[] = {shift_re, shift_im};
    _imageData_apply_to_all_chunks(imgdata, callback, shifts);

    cutil_log_debug("Performed shift: %i %i", shift_re, shift_im);
}

static void
_imageData_register_reset(ImageData *imgdata)
{
    _imageData_init_view(imgdata);
    PixelChunk_callback *const callback = &PixelChunk_callback_reset;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);

    cutil_log_debug("Performed reset");
}

static void
_imageData_register_view_save(ImageData *imgdata)
{
    View *const view = imgdata->view;
    char *const fname = imgdata->view_fname;

    JsonUtil_write(view, fname, &View_to_Json_void);

    cutil_log_debug("Saved view");
}

static void
_imageData_register_view_load(ImageData *imgdata)
{
    View *const view = imgdata->view;
    char *const fname = imgdata->view_fname;

    JsonUtil_read(view, fname, &View_fill_from_Json_void);
    PixelChunk_callback *const callback = &PixelChunk_callback_reset;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);

    cutil_log_debug("Loaded view");
}

/**
 * Updates all pixels in framebuffer of `imgdata`.
 */
static void
_imageData_update_framebuffer(ImageData *imgdata)
{
    const ChunkData *const chunks = &imgdata->chunks;
    const int num_chnks_re = chunks->num_re;
    const int num_chnks_im = chunks->num_im;
    const int num_chnks_tot = num_chnks_re * num_chnks_im;

    const ChunkParams *const params = &chunks->params;
    const int stride = params->stride;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

#pragma omp parallel for
    for (int idx_chnk = 0; idx_chnk < num_chnks_tot; ++idx_chnk) {
        const PixelChunk *const chunk = &chunks->data[idx_chnk];
        for (int idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
            for (int idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
                const int idx_rel = idx_px_re * stride + idx_px_im;
                const PixelData *const px = &chunk->data[idx_rel];

                const int idx_abs_re = chunk->idx_re * num_px_re + idx_px_re;
                const int idx_abs_im = chunk->idx_im * num_px_im + idx_px_im;
                const int idx_abs = idx_abs_re * stride + idx_abs_im;
                imgdata->framebuf[idx_abs] = px->itrs;
            }
        }
    }
}

static void
_imageData_update_pixels(ImageData *imgdata)
{
    PixelChunk_callback *const callback = &PixelChunk_callback_update;
    _imageData_apply_to_all_chunks(imgdata, callback, imgdata);
    _imageData_update_framebuffer(imgdata);
}

static bool
_imageData_is_complete(ImageData *imgdata)
{
    ChunkData *const chunks = &imgdata->chunks;
    const int num_tot = chunks->num_re * chunks->num_im;
    for (int idx = 0; idx < num_tot; ++idx) {
        PixelChunk *const chunk = &chunks->data[idx];
        if (chunk->state == CHUNK_STATE_INVALID) {
            return false;
        }
    }
    return true;
}

ImageData *
ImageData_create(const Settings *settings)
{
    ImageData *const imgdata = _imageData_alloc(settings);
    _imageData_update_pixels(imgdata);
    return imgdata;
}

void
ImageData_free(ImageData *imgdata)
{
    if (imgdata == NULL) {
        return;
    }
    _imageData_clear(imgdata);
    free(imgdata);
}

void
ImageData_register_action(ImageData *imgdata, enum Key key)
{
    static const int ZOOM_STAGES = 1;
    static const int SHIFT_STAGES = 2;
    if (imgdata->state == DATA_STATE_WORKING) {
        return;
    }
    switch (key) {
    case KEY_ZOOM_IN: {
        _imageData_register_zoom(imgdata, +ZOOM_STAGES);
    } break;
    case KEY_ZOOM_OUT: {
        _imageData_register_zoom(imgdata, -ZOOM_STAGES);
    } break;
    case KEY_UP: {
        _imageData_register_shift(imgdata, 0, -SHIFT_STAGES);
    } break;
    case KEY_DOWN: {
        _imageData_register_shift(imgdata, 0, +SHIFT_STAGES);
    } break;
    case KEY_LEFT: {
        _imageData_register_shift(imgdata, -SHIFT_STAGES, 0);
    } break;
    case KEY_RIGHT: {
        _imageData_register_shift(imgdata, +SHIFT_STAGES, 0);
    } break;
    case KEY_RESET: {
        _imageData_register_reset(imgdata);
    } break;
    case KEY_VIEW_SAVE: {
        _imageData_register_view_save(imgdata);
    } break;
    case KEY_VIEW_LOAD: {
        _imageData_register_view_load(imgdata);
    } break;
    default:
        return;
    }
    imgdata->state = DATA_STATE_WORKING;
}

int
ImageData_perform_action(ImageData *imgdata, unsigned int mseconds)
{
    if (imgdata->state == DATA_STATE_IDLE) {
        msleep(mseconds);
        return 0;
    }
    imgdata->target_ticks = SDL_GetTicks64() + mseconds;
    _imageData_update_pixels(imgdata);
    if (_imageData_is_complete(imgdata)) {
        imgdata->state = DATA_STATE_IDLE;
    }
    return 1;
}

void
ImageData_update_chunk(const ImageData *imgdata, PixelChunk *chunk)
{
    if (chunk->state == CHUNK_STATE_VALID) {
        return;
    }

    const ChunkData *const chunks = &imgdata->chunks;
    const ChunkParams *const params = &chunks->params;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

    for (int idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
        for (int idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
            _imageData_update_chunk_pixels(
              imgdata, chunk, idx_px_re, idx_px_im
            );
            if (SDL_GetTicks64() > imgdata->target_ticks) {
                return;
            }
        }
    }

    chunk->state = CHUNK_STATE_VALID;
}

const float *
ImageData_get_pixel_data(const ImageData *imgdata)
{
    return imgdata->framebuf;
}
