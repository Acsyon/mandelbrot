#include "data.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_timer.h>
#include <gmp.h>
#include <omp.h>

#include "app.h"
#include "color.h"
#include "settings.h"
#include "sys.h"
#include "view.h"

#define FILENAME_BUFFER_SIZE 4096
#define VIEW_SAVE_FILE "view.json"

#define INITIAL_PRECISION GMP_LIMB_BITS
#define ITERATION_CUTOFF_ABSOLUTE_VALUE 2.0

#define PERIODICITY_CHECK_CYCLE_LENGTH 25
#define MPF_SIMILARITY_THRESHOLD 1.0e-6

static inline int
_mpf_is_similar(mpf_srcptr lhs, mpf_srcptr rhs, mpf_ptr tmp, double reldiff)
{
    mpf_reldiff(tmp, lhs, rhs);
    mpf_abs(tmp, tmp);
    return (mpf_cmp_d(tmp, reldiff) < 0);
}

#define MPF_IS_SIMILAR(lhs, rhs, tmp)                                          \
    _mpf_is_similar((lhs), (rhs), (tmp), MPF_SIMILARITY_THRESHOLD)

/* -------------------------------------------------------------------------- */

/**
 * Possible pixel states
 */
enum PixelState {
    PIXEL_STATE_INVALID = -1,
    PIXEL_STATE_VALID = 0,
    PIXEL_STATE_INTERPOLATED,
};

/**
 * Possible chunk states
 */
enum ChunkState {
    CHUNK_STATE_INVALID = -1,
    CHUNK_STATE_VALID = 0,
    CHUNK_STATE_INTERPOLATED,
};

/**
 * Possible data states
 */
enum DataState {
    DATA_STATE_WORKING = -1,
    DATA_STATE_IDLE = 0,
};

/* -------------------------------------------------------------------------- */

/**
 * Struct containing data for each pixel
 */
typedef struct {
    mpf_t re;
    mpf_t im;
    float itrs;
    enum PixelState state;
} _pixelData;

static void
_pixelData_init(_pixelData *px)
{
    mpf_init(px->re);
    mpf_init(px->im);
    px->state = PIXEL_STATE_INVALID;
    px->itrs = INVALID_POS;
}

static void
_pixelData_clear(_pixelData *px)
{
    mpf_clear(px->re);
    mpf_clear(px->im);
}

static void
_pixelData_set_prec(_pixelData *px, mp_bitcnt_t prec)
{
    mpf_set_prec(px->re, prec);
    mpf_set_prec(px->im, prec);
}

/* -------------------------------------------------------------------------- */

/**
 * Struct for buffer of thread-safe variables
 */
typedef struct {
    mpf_t re;
    mpf_t im;
    mpf_t re_sqr;
    mpf_t im_sqr;
    mpf_t abs_sqr;
    mpf_t max_sqr;
    mpf_t re_old;
    mpf_t im_old;
    mpf_t tmp;
} _pixelDataBuffer;

static void
_pixelDataBuffer_init(_pixelDataBuffer *buf)
{
    mpf_init(buf->re);
    mpf_init(buf->im);
    mpf_init(buf->re_sqr);
    mpf_init(buf->im_sqr);
    mpf_init(buf->abs_sqr);
    mpf_init(buf->max_sqr);
    mpf_init(buf->re_old);
    mpf_init(buf->im_old);
    mpf_init(buf->tmp);
}

static void
_pixelDataBuffer_clear(_pixelDataBuffer *buf)
{
    mpf_clear(buf->re);
    mpf_clear(buf->im);
    mpf_clear(buf->re_sqr);
    mpf_clear(buf->im_sqr);
    mpf_clear(buf->abs_sqr);
    mpf_clear(buf->max_sqr);
    mpf_clear(buf->re_old);
    mpf_clear(buf->im_old);
    mpf_clear(buf->tmp);
}

static void
_pixelDataBuffer_set_prec(_pixelDataBuffer *buf, mp_bitcnt_t prec)
{
    mpf_set_prec(buf->re, prec);
    mpf_set_prec(buf->im, prec);
    mpf_set_prec(buf->re_sqr, prec);
    mpf_set_prec(buf->im_sqr, prec);
    mpf_set_prec(buf->abs_sqr, prec);
    mpf_set_prec(buf->max_sqr, prec);
    mpf_set_prec(buf->re_old, prec);
    mpf_set_prec(buf->im_old, prec);
    mpf_set_prec(buf->tmp, prec);
}

/* -------------------------------------------------------------------------- */

/**
 * Performs actual Mandelbrot iterations on _pixelData `px` for position in
 * _pixelDataBuffer `tbuf`.
 *
 * @param[in] px _pixelData to work with
 * @param[in] buf _pixelDataBuffer
 */
static void
_pixelData_iterate(_pixelData *px, _pixelDataBuffer *buf, uint16_t max_itrs)
{
    mpf_set_ui(buf->re, 0UL);
    mpf_set_ui(buf->im, 0UL);

    mpf_set_ui(buf->re_old, 0UL);
    mpf_set_ui(buf->im_old, 0UL);

    uint16_t period = 0;
    for (uint16_t itrs = 1; itrs <= max_itrs; ++itrs) {
        mpf_mul(buf->re_sqr, buf->re, buf->re);
        mpf_mul(buf->im_sqr, buf->im, buf->im);

        mpf_mul_ui(buf->im, buf->im, 2UL);
        mpf_mul(buf->im, buf->im, buf->re);
        mpf_add(buf->im, buf->im, px->im);

        mpf_sub(buf->re, buf->re_sqr, buf->im_sqr);
        mpf_add(buf->re, buf->re, px->re);

        mpf_add(buf->abs_sqr, buf->re_sqr, buf->im_sqr);
        if (mpf_cmp(buf->abs_sqr, buf->max_sqr) > 0) {
            px->itrs = 1.0F * itrs / max_itrs;
            return;
        }

        if (MPF_IS_SIMILAR(buf->re, buf->re_old, buf->tmp)
            && MPF_IS_SIMILAR(buf->im, buf->im_old, buf->tmp))
        {
            px->itrs = 0.0F;
            return;
        }

        ++period;
        if (period > PERIODICITY_CHECK_CYCLE_LENGTH) {
            period = 0;
            mpf_set(buf->re_old, buf->re);
            mpf_set(buf->im_old, buf->im);
        }
    }

    px->itrs = 0.0F; /* Converged I guess.. */
}

/* -------------------------------------------------------------------------- */

typedef struct {
    int stride;
    int num_px_re;
    int num_px_im;
} _chunkParams;

static void
_chunkParams_init(_chunkParams *params, const Settings *settings)
{
    params->stride = settings->height;
    params->num_px_re = settings->width / settings->num_chnks_re;
    params->num_px_im = settings->height / settings->num_chnks_im;
}

/* -------------------------------------------------------------------------- */

typedef struct {
    int idx_re;
    int idx_im;
    _pixelData *data;
    enum ChunkState state;
} _pixelChunk;

static void
_pixelChunk_shift(
  int num_chnks, int shift, int *p_idx, enum ChunkState *p_state
)
{
    if (shift == 0) {
        return;
    }
    const int idx_old = *p_idx;
    const int new_idx_raw = idx_old - shift;
    const int new_idx = *p_idx = (new_idx_raw + num_chnks) % num_chnks;

    /* In case of overflow, chunk has to be recreated */
    if (new_idx != new_idx_raw) {
        *p_state = CHUNK_STATE_INVALID;
    }
}

/* -------------------------------------------------------------------------- */

typedef struct {
    _chunkParams params;
    int num_re;
    int num_im;
    _pixelChunk *data;
} _chunkData;

static void
_chunkData_init(_chunkData *chunks, const Settings *settings, _pixelData *px)
{
    _chunkParams *const params = &chunks->params;
    _chunkParams_init(params, settings);

    const int num_chnks_re = chunks->num_re = settings->num_chnks_re;
    const int num_chnks_im = chunks->num_im = settings->num_chnks_im;

    const int dims = num_chnks_re * num_chnks_im;
    chunks->data = malloc(dims * sizeof *chunks->data);

    const int stride = params->stride;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

    for (int idx_chnk_re = 0; idx_chnk_re < num_chnks_re; ++idx_chnk_re) {
        for (int idx_chnk_im = 0; idx_chnk_im < num_chnks_im; ++idx_chnk_im) {
            const int idx_chnk = idx_chnk_re * num_chnks_im + idx_chnk_im;
            _pixelChunk *const chunk = &chunks->data[idx_chnk];
            chunk->idx_re = idx_chnk_re;
            chunk->idx_im = idx_chnk_im;
            chunk->state = CHUNK_STATE_INVALID;

            const int idx_px_re = idx_chnk_re * num_px_re;
            const int idx_px_im = idx_chnk_im * num_px_im;
            const int idx_px = idx_px_re * stride + idx_px_im;
            chunk->data = &px[idx_px];
        }
    }
}

static void
_chunkData_clear(_chunkData *chunks)
{
    free(chunks->data);
}

/* -------------------------------------------------------------------------- */

/**
 * ImageData container struct
 */
struct ImageData {
    Settings *settings;
    mp_bitcnt_t prec;
    View *view;
    mpf_t action_buf;
    _pixelData *data;
    int tnum;
    _pixelDataBuffer *tbuf;
    _chunkData chunks;
    float *framebuf;
    enum DataState state;
    uint64_t target_ticks;
    char view_fname[FILENAME_BUFFER_SIZE];
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
        _pixelData_init(&imgdata->data[i]);
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
        _pixelDataBuffer *const buf = &imgdata->tbuf[i];
        _pixelDataBuffer_init(buf);
        mpf_set(buf->max_sqr, max_sqr);
    }
    mpf_clear(max_sqr);
}

static void
_imageData_init_chunks(ImageData *imgdata)
{
    _chunkData *const chunks = &imgdata->chunks;
    const Settings *const settings = imgdata->settings;
    _pixelData *const data = imgdata->data;
    _chunkData_init(chunks, settings, data);
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
        _pixelData_clear(&imgdata->data[i]);
    }
    free(imgdata->data);
    free(imgdata->framebuf);
    Settings_free(imgdata->settings);
}

static void
_imageData_clear_tbuf(ImageData *imgdata)
{
    const int tnum = imgdata->tnum;
    for (int i = 0; i < tnum; ++i) {
        _pixelDataBuffer_clear(&imgdata->tbuf[i]);
    }
    free(imgdata->tbuf);
}

static void
_imageData_clear_chunks(ImageData *imgdata)
{
    _chunkData *const chunks = &imgdata->chunks;
    _chunkData_clear(chunks);
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
        _pixelData_set_prec(&imgdata->data[i], prec);
    }
}

static void
_imageData_set_prec_tbuf(ImageData *imgdata)
{
    const mp_bitcnt_t prec = imgdata->prec;
    const int tnum = imgdata->tnum;
    for (int i = 0; i < tnum; ++i) {
        _pixelDataBuffer *const buf = &imgdata->tbuf[i];
        _pixelDataBuffer_set_prec(buf, prec);
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

/* -------------------------------------------------------------------------- */

static void
_pixelChunk_update_pixel(
  _pixelChunk *chunk, const ImageData *imgdata, int idx_px_re, int idx_px_im
)
{
    const _chunkData *const chunks = &imgdata->chunks;
    const _chunkParams *const params = &chunks->params;

    const int stride = params->stride;
    const int idx_px = idx_px_re * stride + idx_px_im;
    _pixelData *const px = &chunk->data[idx_px];

    if (px->state == PIXEL_STATE_VALID) {
        return;
    }

    _pixelDataBuffer *const tbuf = imgdata->tbuf;
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
    _pixelDataBuffer *const buf = &tbuf[tid];
    _pixelData_iterate(px, buf, settings->max_itrs);

    px->state = PIXEL_STATE_VALID;
}

static void
_pixelChunk_update(_pixelChunk *chunk, const ImageData *imgdata)
{
    if (chunk->state == CHUNK_STATE_VALID) {
        return;
    }

    const _chunkData *const chunks = &imgdata->chunks;
    const _chunkParams *const params = &chunks->params;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

    for (int idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
        for (int idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
            _pixelChunk_update_pixel(chunk, imgdata, idx_px_re, idx_px_im);
            if (SDL_GetTicks64() > imgdata->target_ticks) {
                return;
            }
        }
    }

    chunk->state = CHUNK_STATE_VALID;
}

static void
_pixelChunk_invalidate_all(_pixelChunk *chunk, const ImageData *imgdata)
{
    const _chunkData *const chunks = &imgdata->chunks;
    const _chunkParams *const params = &chunks->params;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

    for (int idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
        for (int idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
            const int stride = params->stride;
            const int idx_px = idx_px_re * stride + idx_px_im;
            _pixelData *const px = &chunk->data[idx_px];
            px->state = PIXEL_STATE_INVALID;
            px->itrs = INVALID_POS;
        }
    }

    chunk->state = CHUNK_STATE_INVALID;
}

typedef void
_pixelChunk_callback(
  _pixelChunk *chunk, const ImageData *imgdata, const void *vparams
);

static void
_imageData_apply_to_all_chunks(
  ImageData *imgdata, _pixelChunk_callback *callback, const void *vparams
)
{
    _chunkData *const chunks = &imgdata->chunks;
    const int num_tot = chunks->num_re * chunks->num_im;
#pragma omp parallel for
    for (int idx = 0; idx < num_tot; ++idx) {
        _pixelChunk *const chunk = &chunks->data[idx];
        callback(chunk, imgdata, vparams);
    }
}

static void
_pixelChunk_callback_update(
  _pixelChunk *chunk, const ImageData *imgdata, const void *vparams
)
{
    (void) vparams;
    _pixelChunk_update(chunk, imgdata);
}

static inline void
_pixelChunk_shift_re(_pixelChunk *chunk, const _chunkData *chunks, int shift)
{
    _pixelChunk_shift(chunks->num_re, shift, &chunk->idx_re, &chunk->state);
}

static inline void
_pixelChunk_shift_im(_pixelChunk *chunk, const _chunkData *chunks, int shift)
{
    _pixelChunk_shift(chunks->num_im, shift, &chunk->idx_im, &chunk->state);
}

static void
_pixelChunk_callback_shift(
  _pixelChunk *chunk, const ImageData *imgdata, const void *vparams
)
{
    const _chunkData *const chunks = &imgdata->chunks;

    const int *const shifts = vparams;
    const int shift_re = shifts[0];
    const int shift_im = shifts[1];

    _pixelChunk_shift_re(chunk, chunks, shift_re);
    _pixelChunk_shift_im(chunk, chunks, shift_im);

    if (chunk->state == CHUNK_STATE_INVALID) {
        _pixelChunk_invalidate_all(chunk, imgdata);
    }
}

static void
_pixelChunk_zoom(_pixelChunk *chunk, const _chunkData *chunks, int stages)
{
    (void) chunk;
    (void) chunks;
    (void) stages;
}

static void
_pixelChunk_callback_zoom(
  _pixelChunk *chunk, const ImageData *imgdata, const void *vparams
)
{
    const _chunkData *const chunks = &imgdata->chunks;
    const int stages = *(const int *) vparams;
    _pixelChunk_zoom(chunk, chunks, stages);
    _pixelChunk_invalidate_all(chunk, imgdata);
}

static mp_bitcnt_t
_calculate_new_prec(mpf_srcptr upp)
{
    static const mp_bitcnt_t MIN_PREC = GMP_LIMB_BITS;
    static const mp_bitcnt_t MAX_PREC = GMP_LIMB_BITS * 1024;
    static const mp_bitcnt_t MAX_DIFF = GMP_LIMB_BITS / 4;

    long int exp = 0;
    (void) mpf_get_d_2exp(&exp, upp);
    exp = labs(exp);

    const mp_bitcnt_t new_prec
      = ((exp + MAX_DIFF + GMP_LIMB_BITS) / GMP_LIMB_BITS) * GMP_LIMB_BITS;
    if (new_prec < MIN_PREC) {
        return MIN_PREC;
    }
    if (new_prec > MAX_PREC) {
        return MAX_PREC;
    }
    return new_prec;
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

    const mp_bitcnt_t new_prec = _calculate_new_prec(upp);
    if (new_prec != imgdata->prec) {
        _imageData_set_prec(imgdata, new_prec);
    }

    _pixelChunk_callback *const callback = &_pixelChunk_callback_zoom;
    _imageData_apply_to_all_chunks(imgdata, callback, &stages);
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

    _pixelChunk_callback *const callback = &_pixelChunk_callback_shift;
    const int shifts[] = {shift_re, shift_im};
    _imageData_apply_to_all_chunks(imgdata, callback, shifts);
}

static void
_pixelChunk_callback_reset(
  _pixelChunk *chunk, const ImageData *imgdata, const void *vparams
)
{
    (void) vparams;
    _pixelChunk_invalidate_all(chunk, imgdata);
}

static void
_imageData_register_reset(ImageData *imgdata)
{
    _imageData_init_view(imgdata);
    _pixelChunk_callback *const callback = &_pixelChunk_callback_reset;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);
}

static void
_imageData_register_view_save(ImageData *imgdata)
{
    View *const view = imgdata->view;
    const char *const path = App_get_env_path();
    char *const fname = imgdata->view_fname;
    snprintf(fname, FILENAME_BUFFER_SIZE, "%s/%s", path, VIEW_SAVE_FILE);

    JsonUtil_write(view, fname, &View_to_Json_void);
}

static void
_imageData_register_view_load(ImageData *imgdata)
{
    View *const view = imgdata->view;
    const char *const path = App_get_env_path();
    char *const fname = imgdata->view_fname;
    snprintf(fname, FILENAME_BUFFER_SIZE, "%s/%s", path, VIEW_SAVE_FILE);

    JsonUtil_read(view, fname, &View_fill_from_Json_void);
    _pixelChunk_callback *const callback = &_pixelChunk_callback_reset;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);
}

/**
 * Updates all pixels in framebuffer of `imgdata`.
 */
static void
_imageData_update_framebuffer(ImageData *imgdata)
{
    const _chunkData *const chunks = &imgdata->chunks;
    const int num_chnks_re = chunks->num_re;
    const int num_chnks_im = chunks->num_im;
    const int num_chnks_tot = num_chnks_re * num_chnks_im;

    const _chunkParams *const params = &chunks->params;
    const int stride = params->stride;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

#pragma omp parallel for
    for (int idx_chnk = 0; idx_chnk < num_chnks_tot; ++idx_chnk) {
        const _pixelChunk *const chunk = &chunks->data[idx_chnk];
        for (int idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
            for (int idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
                const int idx_rel = idx_px_re * stride + idx_px_im;
                const _pixelData *const px = &chunk->data[idx_rel];

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
    _pixelChunk_callback *const callback = &_pixelChunk_callback_update;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);
    _imageData_update_framebuffer(imgdata);
}

static bool
_imageData_is_complete(ImageData *imgdata)
{
    _chunkData *const chunks = &imgdata->chunks;
    const int num_tot = chunks->num_re * chunks->num_im;
    for (int idx = 0; idx < num_tot; ++idx) {
        _pixelChunk *const chunk = &chunks->data[idx];
        if (chunk->state == CHUNK_STATE_INVALID) {
            return false;
        }
    }
    return true;
}

/* -------------------------------------------------------------------------- */

static ImageData *
_imageData_create(const Settings *settings)
{
    ImageData *const imgdata = _imageData_alloc(settings);
    _imageData_update_pixels(imgdata);
    return imgdata;
}

static void
_imageData_free(ImageData *imgdata)
{
    if (imgdata == NULL) {
        return;
    }
    _imageData_clear(imgdata);
    free(imgdata);
}

static void
_imageData_register_action(ImageData *imgdata, enum Key key)
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

static int
_imageData_perform_action(ImageData *imgdata, unsigned int mseconds)
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

/* -------------------------------------------------------------------------- */

ImageData *
ImageData_app_init(void)
{
    ImageData *const imgdata = App_get_image_data();
    if (imgdata != NULL) {
        return imgdata;
    }
    const Settings *const settings = App_get_settings();
    return _imageData_create(settings);
}

void
ImageData_free(ImageData *imgdata)
{
    _imageData_free(imgdata);
}

void
ImageData_register_action(ImageData *imgdata, enum Key key)
{
    _imageData_register_action(imgdata, key);
}

int
ImageData_perform_action(ImageData *imgdata, unsigned int mseconds)
{
    return _imageData_perform_action(imgdata, mseconds);
}

const float *
ImageData_get_pixel_data(const ImageData *imgdata)
{
    return imgdata->framebuf;
}
