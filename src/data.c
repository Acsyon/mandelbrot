#include "data.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <omp.h>

#include "settings.h"

#define NUMBER_CHUNKS_REAL 10
#define NUMBER_CHUNKS_IMAG 10

/**
 * Possible pixel states
 */
enum PixelState {
    PX_VALID = 0,
    PX_INTERPOLATED,
    PX_INVALID,
};

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
    px->state = PX_INVALID;
}

static void
_pixelData_clear(_pixelData *px)
{
    mpf_clear(px->re);
    mpf_clear(px->im);
}

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
}

/**
 * Perform actual Mandelbrot iterations on _pixelData `px` for position in
 * _pixelDataBuffer `tbuf`
 *
 * @param[in] px _pixelData to work with
 * @param[in] buf _pixelDataBuffer
 */
static void
_pixelData_iterate(_pixelData *px, _pixelDataBuffer *buf, uint16_t max_itrs)
{
    mpf_set_ui(buf->re, 0UL);
    mpf_set_ui(buf->im, 0UL);

    for (uint16_t i = 1; i <= max_itrs; ++i) {
        mpf_mul(buf->re_sqr, buf->re, buf->re);
        mpf_mul(buf->im_sqr, buf->im, buf->im);

        mpf_mul_ui(buf->im, buf->im, 2UL);
        mpf_mul(buf->im, buf->im, buf->re);
        mpf_add(buf->im, buf->im, px->im);

        mpf_sub(buf->re, buf->re_sqr, buf->im_sqr);
        mpf_add(buf->re, buf->re, px->re);

        mpf_add(buf->abs_sqr, buf->re_sqr, buf->im_sqr);
        if (mpf_cmp(buf->abs_sqr, buf->max_sqr) > 0) {
            px->itrs = 1.0f * i / max_itrs;
            return;
        }
    }

    px->itrs = 0.0; /* Converged I guess.. */
}

/**
 * Global ImageData object
 */
typedef struct {
    Settings settings;
    mpf_t resolution;
    mpf_t centre_re;
    mpf_t centre_im;
    _pixelData *data;
    _pixelDataBuffer *tbuf;
    mpf_t action_buf;
    float *framebuf;
} _imageData;

static _imageData *
_imageData_alloc(const Settings *settings)
{
    _imageData *const imgdata = malloc(sizeof *imgdata);

    imgdata->settings = *settings;
    mpf_set_default_prec(imgdata->settings.prec);

    mpf_init(imgdata->resolution);
    mpf_init(imgdata->centre_re);
    mpf_init(imgdata->centre_im);
    mpf_set_d(imgdata->resolution, Settings_get_resolution(&imgdata->settings));
    mpf_set_d(imgdata->centre_re, imgdata->settings.cntr_re);
    mpf_set_d(imgdata->centre_im, imgdata->settings.cntr_im);

    const int dims = imgdata->settings.width * imgdata->settings.height;
    imgdata->data = malloc(dims * sizeof *imgdata->data);
    for (int i = 0; i < dims; ++i) {
        _pixelData_init(&imgdata->data[i]);
    }
    imgdata->framebuf = malloc(dims * sizeof *imgdata->framebuf);

    const int tnum = omp_get_max_threads();
    imgdata->tbuf = malloc(tnum * sizeof *imgdata->tbuf);

    mpf_t max_sqr;
    mpf_init(max_sqr);
    mpf_set_d(max_sqr, imgdata->settings.max_absval);
    mpf_mul(max_sqr, max_sqr, max_sqr);
    for (int i = 0; i < tnum; ++i) {
        _pixelDataBuffer *const buf = &imgdata->tbuf[i];
        _pixelDataBuffer_init(buf);
        mpf_set(buf->max_sqr, max_sqr);
    }
    mpf_clear(max_sqr);

    mpf_init(imgdata->action_buf);

    return imgdata;
}

static _pixelData *
_imageData_get_pixel(_imageData *imgdata, int idx_re, int idx_im)
{
    const int height = imgdata->settings.height;
    return &imgdata->data[idx_re * height + idx_im];
}

/**
 * Updates all (necessary) pixels in global ImageData object.
 */
static void
_imageData_update(_imageData *imgdata)
{
    _pixelDataBuffer *const tbuf = imgdata->tbuf;
    const Settings *const settings = &imgdata->settings;

    const mpf_ptr res = imgdata->resolution;
    const mpf_ptr cntr_re = imgdata->centre_re;
    const mpf_ptr cntr_im = imgdata->centre_im;

    const int num_re = imgdata->settings.width;
    const int num_im = imgdata->settings.height;
    const int idx_cntr_re = num_re / 2;
    const int idx_cntr_im = num_im / 2;

    const int num_prog = (num_re * num_im) / 100;
    int ctr = 0;
#pragma omp parallel for collapse(2)
    for (int idx_re = 0; idx_re < num_re; ++idx_re) {
        for (int idx_im = 0; idx_im < num_im; ++idx_im) {
            _pixelData *const px
              = _imageData_get_pixel(imgdata, idx_re, idx_im);

#pragma omp atomic
            ++ctr;
            if (ctr % num_prog == 0) {
                const int prct = ctr / num_prog;
                printf("\33[2K\rRasterizing complex plane... (%3u %%)", prct);
                fflush(stdout);
            }

            if (px->state == PX_VALID) {
                continue;
            }

            mpf_set_d(px->re, (1.0 * (idx_re - idx_cntr_re)));
            mpf_mul(px->re, px->re, res);
            mpf_add(px->re, px->re, cntr_re);

            mpf_set_d(px->im, (1.0 * (idx_im - idx_cntr_im)));
            mpf_mul(px->im, px->im, res);
            mpf_add(px->im, px->im, cntr_im);

            const int tid = omp_get_thread_num();
            _pixelDataBuffer *const buf = &tbuf[tid];
            _pixelData_iterate(px, buf, settings->max_itrs);

            imgdata->framebuf[idx_re * num_im + idx_im] = px->itrs;
        }
    }
    printf("\n");
}

static _imageData *
_imageData_create(const Settings *settings)
{
    _imageData *const imgdata = _imageData_alloc(settings);
    _imageData_update(imgdata);
    return imgdata;
}

static void
_imageData_free(_imageData *imgdata)
{
    if (imgdata == NULL) {
        return;
    }

    const int dims = imgdata->settings.width * imgdata->settings.height;
    for (int i = 0; i < dims; ++i) {
        _pixelData_clear(&imgdata->data[i]);
    }
    free(imgdata->data);

    const int tnum = omp_get_max_threads();
    for (int i = 0; i < tnum; ++i) {
        _pixelDataBuffer_clear(&imgdata->tbuf[i]);
    }
    free(imgdata->tbuf);

    mpf_clear(imgdata->action_buf);

    free(imgdata->framebuf);

    free(imgdata);
}

void
_imageData_action(_imageData *imgdata, enum Key key)
{
    const Settings *const settings = &imgdata->settings;
    const mpf_ptr buf = imgdata->action_buf;
    const mpf_ptr res = imgdata->resolution;
    const mpf_ptr cntr_re = imgdata->centre_re;
    const mpf_ptr cntr_im = imgdata->centre_im;
    switch (key) {
    case KEY_ZOOM_IN: {
        mpf_set_d(buf, settings->zoom_spd);
        mpf_mul(res, res, buf);
    } break;
    case KEY_ZOOM_OUT: {
        mpf_set_d(buf, settings->zoom_spd);
        mpf_div(res, res, buf);
    } break;
    case KEY_UP: {
        const double offs = settings->height * settings->scrl_spd;
        mpf_set_d(buf, offs);
        mpf_mul(buf, buf, res);
        mpf_sub(cntr_im, cntr_im, buf);
    } break;
    case KEY_DOWN: {
        const double offs = settings->height * settings->scrl_spd;
        mpf_set_d(buf, offs);
        mpf_mul(buf, buf, res);
        mpf_add(cntr_im, cntr_im, buf);
    } break;
    case KEY_LEFT: {
        const double offs = settings->width * settings->scrl_spd;
        mpf_set_d(buf, offs);
        mpf_mul(buf, buf, res);
        mpf_sub(cntr_re, cntr_re, buf);
    } break;
    case KEY_RIGHT: {
        const double offs = settings->width * settings->scrl_spd;
        mpf_set_d(buf, offs);
        mpf_mul(buf, buf, res);
        mpf_add(cntr_re, cntr_re, buf);
    } break;
    case KEY_RESET: {
        mpf_set_d(res, Settings_get_resolution(settings));
        mpf_set_d(cntr_re, settings->cntr_re);
        mpf_set_d(cntr_im, settings->cntr_im);
    } break;
    case KEY_INVALID:
    default:
        return;
    }
    _imageData_update(imgdata);
}

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

void
ImageData_action(enum Key key)
{
    _imageData_action(_imgdata, key);
}

const float *
ImageData_get_pixel_data(void)
{
    return _imgdata->framebuf;
}
