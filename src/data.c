#include "data.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <omp.h>

#include "settings.h"

#define INITIAL_PRECISION GMP_LIMB_BITS
#define PERIODICITY_CHECK_CYCLE_LENGTH 25
#define ITERATION_CUTOFF_ABSOLUTE_VALUE 2.0

/**
 * Possible data (pixel and chunk) states. Not all apply to all.
 */
enum DataState {
    STATE_INVALID = -1,
    STATE_VALID = 0,
    STATE_INTERPOLATED,
};

/* -------------------------------------------------------------------------- */

/**
 * Struct containing data for each pixel
 */
typedef struct {
    mpf_t re;
    mpf_t im;
    float itrs;
    enum DataState state;
} _pixelData;

static void
_pixelData_init(_pixelData *px)
{
    mpf_init(px->re);
    mpf_init(px->im);
    px->state = STATE_INVALID;
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

static inline int
_mpf_is_similar(mpf_srcptr lhs, mpf_srcptr rhs, mpf_ptr tmp, double reldiff)
{
    mpf_reldiff(tmp, lhs, rhs);
    mpf_abs(tmp, tmp);
    return (mpf_cmp_d(tmp, reldiff) < 0);
}

#define MPF_IS_SIMILAR(lhs, rhs, tmp)                                          \
    _mpf_is_similar((lhs), (rhs), (tmp), 1.0e-6)

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
            px->itrs = 1.0f * itrs / max_itrs;
            return;
        }

        if (MPF_IS_SIMILAR(buf->re, buf->re_old, buf->tmp)
            && MPF_IS_SIMILAR(buf->im, buf->im_old, buf->tmp))
        {
            px->itrs = 0.0f;
            return;
        }

        ++period;
        if (period > PERIODICITY_CHECK_CYCLE_LENGTH) {
            period = 0;
            mpf_set(buf->re_old, buf->re);
            mpf_set(buf->im_old, buf->im);
        }
    }

    px->itrs = 0.0f; /* Converged I guess.. */
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
    enum DataState state;
} _pixelChunk;

static void
_pixelChunk_shift(int num_chnks, int shift, int *p_idx, enum DataState *p_state)
{
    if (shift == 0) {
        return;
    }
    const int idx_old = *p_idx;
    const int new_idx_raw = idx_old - shift;
    const int new_idx = *p_idx = (new_idx_raw + num_chnks) % num_chnks;

    /* In case of overflow, chunk has to be recreated */
    if (new_idx != new_idx_raw) {
        *p_state = STATE_INVALID;
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
            chunk->state = STATE_INVALID;

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
typedef struct {
    Settings settings;
    mp_bitcnt_t prec;
    mpf_t upp;
    mpf_t cntr_re;
    mpf_t cntr_im;
    mpf_t action_buf;
    _pixelData *data;
    int tnum;
    _pixelDataBuffer *tbuf;
    _chunkData chunks;
    float *framebuf;
} _imageData;

static void
_imageData_init_mpf(_imageData *imgdata)
{
    mpf_init(imgdata->upp);
    mpf_init(imgdata->cntr_re);
    mpf_init(imgdata->cntr_im);
    mpf_init(imgdata->action_buf);
}

static void
_imageData_init_view(_imageData *imgdata)
{
    const Settings *const settings = &imgdata->settings;
    mpf_set_d(imgdata->upp, Settings_get_units_per_pixel(settings));
    mpf_set_d(imgdata->cntr_re, settings->cntr_re);
    mpf_set_d(imgdata->cntr_im, settings->cntr_im);
}

static void
_imageData_init_data(_imageData *imgdata)
{
    const Settings *const settings = &imgdata->settings;
    const int num_tot = settings->width * settings->height;
    imgdata->data = malloc(num_tot * sizeof *imgdata->data);
    for (int i = 0; i < num_tot; ++i) {
        _pixelData_init(&imgdata->data[i]);
    }
    imgdata->framebuf = malloc(num_tot * sizeof *imgdata->framebuf);
}

static void
_imageData_init_tbuf(_imageData *imgdata)
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
_imageData_init_chunks(_imageData *imgdata)
{
    _chunkData *const chunks = &imgdata->chunks;
    const Settings *const settings = &imgdata->settings;
    _pixelData *const data = imgdata->data;
    _chunkData_init(chunks, settings, data);
}

static _imageData *
_imageData_alloc(const Settings *settings)
{
    _imageData *const imgdata = malloc(sizeof *imgdata);

    imgdata->settings = *settings;
    imgdata->prec = INITIAL_PRECISION;
    mpf_set_default_prec(INITIAL_PRECISION);

    _imageData_init_mpf(imgdata);
    _imageData_init_view(imgdata);
    _imageData_init_data(imgdata);
    _imageData_init_tbuf(imgdata);
    _imageData_init_chunks(imgdata);

    return imgdata;
}

static void
_imageData_clear_mpf(_imageData *imgdata)
{
    mpf_clear(imgdata->upp);
    mpf_clear(imgdata->cntr_re);
    mpf_clear(imgdata->cntr_im);
    mpf_clear(imgdata->action_buf);
}

static void
_imageData_clear_data(_imageData *imgdata)
{
    const Settings *const settings = &imgdata->settings;
    const int num_tot = settings->width * settings->height;
    for (int i = 0; i < num_tot; ++i) {
        _pixelData_clear(&imgdata->data[i]);
    }
    free(imgdata->data);
    free(imgdata->framebuf);
}

static void
_imageData_clear_tbuf(_imageData *imgdata)
{
    const int tnum = imgdata->tnum;
    for (int i = 0; i < tnum; ++i) {
        _pixelDataBuffer_clear(&imgdata->tbuf[i]);
    }
    free(imgdata->tbuf);
}

static void
_imageData_clear_chunks(_imageData *imgdata)
{
    _chunkData *const chunks = &imgdata->chunks;
    _chunkData_clear(chunks);
}

static void
_imageData_clear(_imageData *imgdata)
{
    _imageData_clear_mpf(imgdata);
    _imageData_clear_data(imgdata);
    _imageData_clear_tbuf(imgdata);
    _imageData_clear_chunks(imgdata);
}

static void
_imageData_set_prec_mpf(_imageData *imgdata)
{
    const mp_bitcnt_t prec = imgdata->prec;
    mpf_set_prec(imgdata->upp, prec);
    mpf_set_prec(imgdata->cntr_re, prec);
    mpf_set_prec(imgdata->cntr_im, prec);
    mpf_set_prec(imgdata->action_buf, prec);
}

static void
_imageData_set_prec_data(_imageData *imgdata)
{
    const Settings *const settings = &imgdata->settings;
    const int num_tot = settings->width * settings->height;
    const mp_bitcnt_t prec = imgdata->prec;
    for (int i = 0; i < num_tot; ++i) {
        _pixelData_set_prec(&imgdata->data[i], prec);
    }
}

static void
_imageData_set_prec_tbuf(_imageData *imgdata)
{
    const mp_bitcnt_t prec = imgdata->prec;
    const int tnum = imgdata->tnum;
    for (int i = 0; i < tnum; ++i) {
        _pixelDataBuffer *const buf = &imgdata->tbuf[i];
        _pixelDataBuffer_set_prec(buf, prec);
    }
}

static void
_imageData_set_prec(_imageData *imgdata, mp_bitcnt_t prec)
{
    imgdata->prec = prec;
    _imageData_set_prec_mpf(imgdata);
    _imageData_set_prec_data(imgdata);
    _imageData_set_prec_tbuf(imgdata);
}

/* -------------------------------------------------------------------------- */

static void
_pixelChunk_update_pixel(
  _pixelChunk *chunk, const _imageData *imgdata, int idx_px_re, int idx_px_im
)
{
    const _chunkData *const chunks = &imgdata->chunks;
    const _chunkParams *const params = &chunks->params;

    const int stride = params->stride;
    const int idx_px = idx_px_re * stride + idx_px_im;
    _pixelData *const px = &chunk->data[idx_px];

    if (px->state == STATE_VALID) {
        return;
    }

    _pixelDataBuffer *const tbuf = imgdata->tbuf;
    const Settings *const settings = &imgdata->settings;

    const mpf_srcptr upp = imgdata->upp;
    const mpf_srcptr cntr_re = imgdata->cntr_re;
    const mpf_srcptr cntr_im = imgdata->cntr_im;

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

    px->state = STATE_VALID;
}

static void
_pixelChunk_update(_pixelChunk *chunk, const _imageData *imgdata)
{
    if (chunk->state == STATE_VALID) {
        return;
    }

    const _chunkData *const chunks = &imgdata->chunks;
    const _chunkParams *const params = &chunks->params;
    const int num_px_re = params->num_px_re;
    const int num_px_im = params->num_px_im;

    for (int idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
        for (int idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
            _pixelChunk_update_pixel(chunk, imgdata, idx_px_re, idx_px_im);
        }
    }

    chunk->state = STATE_VALID;
}

static void
_pixelChunk_invalidate_all(_pixelChunk *chunk, const _imageData *imgdata)
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
            px->state = STATE_INVALID;
        }
    }

    chunk->state = STATE_INVALID;
}

typedef void
_pixelChunk_callback(
  _pixelChunk *chunk, const _imageData *imgdata, const void *vparams
);

static void
_imageData_apply_to_all_chunks(
  _imageData *imgdata, _pixelChunk_callback *callback, const void *vparams
)
{
    _chunkData *const chunks = &imgdata->chunks;
    const int num_tot = chunks->num_re * chunks->num_im;
    int ctr = 0;
#pragma omp parallel for
    for (int idx = 0; idx < num_tot; ++idx) {
        _pixelChunk *const chunk = &chunks->data[idx];
        callback(chunk, imgdata, vparams);
#pragma omp critical
        {
            ++ctr;
            printf("\33[2K\rWorking... (% 3.2f %%)", 100.0 * ctr / num_tot);
            fflush(stdout);
        }
    }
}

static void
_pixelChunk_callback_update(
  _pixelChunk *chunk, const _imageData *imgdata, const void *vparams
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
  _pixelChunk *chunk, const _imageData *imgdata, const void *vparams
)
{
    const _chunkData *const chunks = &imgdata->chunks;

    const int *const shifts = vparams;
    const int shift_re = shifts[0];
    const int shift_im = shifts[1];

    _pixelChunk_shift_re(chunk, chunks, shift_re);
    _pixelChunk_shift_im(chunk, chunks, shift_im);

    if (chunk->state == STATE_INVALID) {
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
  _pixelChunk *chunk, const _imageData *imgdata, const void *vparams
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
_imageData_zoom(_imageData *imgdata, int stages)
{
    const Settings *const settings = &imgdata->settings;
    const mpf_ptr buf = imgdata->action_buf;
    const mpf_ptr upp = imgdata->upp;

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
_imageData_shift(_imageData *imgdata, int shift_re, int shift_im)
{
    const Settings *const settings = &imgdata->settings;
    const mpf_ptr buf = imgdata->action_buf;
    const mpf_ptr upp = imgdata->upp;
    const mpf_ptr cntr_re = imgdata->cntr_re;
    const mpf_ptr cntr_im = imgdata->cntr_im;

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
  _pixelChunk *chunk, const _imageData *imgdata, const void *vparams
)
{
    (void) vparams;
    _pixelChunk_invalidate_all(chunk, imgdata);
}

static void
_imageData_reset_view(_imageData *imgdata)
{
    _imageData_init_view(imgdata);
    _pixelChunk_callback *const callback = &_pixelChunk_callback_reset;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);
}

/**
 * Updates all pixels in framebuffer of `imgdata`.
 */
static void
_imageData_update_framebuffer(_imageData *imgdata)
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
_imageData_update_pixels(_imageData *imgdata)
{
    _pixelChunk_callback *const callback = &_pixelChunk_callback_update;
    _imageData_apply_to_all_chunks(imgdata, callback, NULL);
    _imageData_update_framebuffer(imgdata);
    printf("\33[2K\rReady...");
    fflush(stdout);
}

/* -------------------------------------------------------------------------- */

static _imageData *
_imageData_create(const Settings *settings)
{
    _imageData *const imgdata = _imageData_alloc(settings);
    _imageData_update_pixels(imgdata);
    return imgdata;
}

static void
_imageData_free(_imageData *imgdata)
{
    if (imgdata == NULL) {
        return;
    }
    _imageData_clear(imgdata);
    free(imgdata);
}

static inline int
_imageData_action(_imageData *imgdata, enum Key key)
{
    switch (key) {
    case KEY_ZOOM_IN: {
        _imageData_zoom(imgdata, +1);
    } break;
    case KEY_ZOOM_OUT: {
        _imageData_zoom(imgdata, -1);
    } break;
    case KEY_UP: {
        _imageData_shift(imgdata, 0, -1);
    } break;
    case KEY_DOWN: {
        _imageData_shift(imgdata, 0, +1);
    } break;
    case KEY_LEFT: {
        _imageData_shift(imgdata, -1, 0);
    } break;
    case KEY_RIGHT: {
        _imageData_shift(imgdata, +1, 0);
    } break;
    case KEY_RESET: {
        _imageData_reset_view(imgdata);
    } break;
    case KEY_INVALID:
    default:
        return 0;
    }
    _imageData_update_pixels(imgdata);
    return 1;
}

/* -------------------------------------------------------------------------- */

/**
 * Global ImageData object
 */
static _imageData *_imgdata = NULL;

void
ImageData_init(void)
{
    if (_imgdata != NULL) {
        return;
    }
    _imgdata = _imageData_create(GLOBAL_SETTINGS);
}

void
ImageData_free(void)
{
    _imageData_free(_imgdata);
    _imgdata = NULL;
}

int
ImageData_action(enum Key key)
{
    return _imageData_action(_imgdata, key);
}

const float *
ImageData_get_pixel_data(void)
{
    return _imgdata->framebuf;
}
