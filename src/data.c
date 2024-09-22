#include "data.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <omp.h>

#include "settings.h"

/**
 * Struct containing data for each pixel
 */
typedef struct {
    mpf_t re;
    mpf_t im;
    float itrs;
    bool redraw;
} PixelData;

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
} PixelDataBuffer;

/**
 * Global ImageData object
 */
static struct {
    unsigned int width;
    unsigned int height;
    mpf_t resolution;
    mpf_t centre_re;
    mpf_t centre_im;
    PixelData *data;
    PixelDataBuffer *tbuf;
    mpf_t action_buf;
    float *framebuf;
} *_imgdata = NULL;

static inline double
_default_resolution(void)
{
    const unsigned int width = GLOBAL_SETTINGS->width;
    const double max_re = GLOBAL_SETTINGS->max_re;
    const double min_re = GLOBAL_SETTINGS->min_re;
    return (max_re - min_re) / width;
}

/**
 * Perform actual Mandelbrot iterations on PixelData `px` for position with real
 * part `re0` and imaginary part `im0` (using PixelDataBuffer `tbuf` for
 * optimizations)
 *
 * @param[in] px PixelData to work with
 * @param[in] re0 real part of position
 * @param[in] im0 imaginary part of position
 * @param[in] buf PixelDataBuffer
 */
static void
_pixeldata_iterate(PixelData *px, PixelDataBuffer *buf)
{
    mpf_set_ui(buf->re, 0UL);
    mpf_set_ui(buf->im, 0UL);

    for (uint16_t i = 1; i <= GLOBAL_SETTINGS->max_itrs; ++i) {
        mpf_mul(buf->re_sqr, buf->re, buf->re);
        mpf_mul(buf->im_sqr, buf->im, buf->im);

        mpf_mul_ui(buf->im, buf->im, 2UL);
        mpf_mul(buf->im, buf->im, buf->re);
        mpf_add(buf->im, buf->im, px->im);

        mpf_sub(buf->re, buf->re_sqr, buf->im_sqr);
        mpf_add(buf->re, buf->re, px->re);

        mpf_add(buf->abs_sqr, buf->re_sqr, buf->im_sqr);
        if (mpf_cmp(buf->abs_sqr, buf->max_sqr) > 0) {
            px->itrs = 1.0f * i / GLOBAL_SETTINGS->max_itrs;
            return;
        }
    }

    px->itrs = 0.0; /* Converged I guess.. */
}

static void
_imagedata_alloc(void)
{
    _imgdata = malloc(sizeof *_imgdata);

    _imgdata->width = GLOBAL_SETTINGS->width;
    _imgdata->height = GLOBAL_SETTINGS->height;

    mpf_init(_imgdata->resolution);
    mpf_init(_imgdata->centre_re);
    mpf_init(_imgdata->centre_im);
    mpf_set_d(_imgdata->resolution, _default_resolution());
    mpf_set_d(_imgdata->centre_re, GLOBAL_SETTINGS->cntr_re);
    mpf_set_d(_imgdata->centre_im, GLOBAL_SETTINGS->cntr_im);

    const unsigned int dims = GLOBAL_SETTINGS->width * GLOBAL_SETTINGS->height;
    _imgdata->data = malloc(dims * sizeof *_imgdata->data);
    for (unsigned int i = 0; i < dims; ++i) {
        PixelData *const px = &_imgdata->data[i];
        mpf_init(px->re);
        mpf_init(px->im);
    }
    _imgdata->framebuf = malloc(dims * sizeof *_imgdata->framebuf);

    const int tnum = omp_get_max_threads();
    _imgdata->tbuf = malloc(tnum * sizeof *_imgdata->tbuf);

    mpf_t max_sqr;
    mpf_init(max_sqr);
    mpf_set_d(max_sqr, GLOBAL_SETTINGS->max_absval);
    mpf_mul(max_sqr, max_sqr, max_sqr);
    for (int i = 0; i < tnum; ++i) {
        PixelDataBuffer *const buf = &_imgdata->tbuf[i];
        mpf_init(buf->re);
        mpf_init(buf->im);
        mpf_init(buf->re_sqr);
        mpf_init(buf->im_sqr);
        mpf_init(buf->abs_sqr);
        mpf_init(buf->max_sqr);
        mpf_set(buf->max_sqr, max_sqr);
    }
    mpf_clear(max_sqr);

    mpf_init(_imgdata->action_buf);
}

/**
 * Updates all (necessary) pixels in global ImageData object.
 */
static void
_imagedata_update(void)
{
    PixelDataBuffer *const tbuf = _imgdata->tbuf;
    const unsigned int width = _imgdata->width;
    const unsigned int height = _imgdata->height;
    mpf_t *const d = &_imgdata->resolution;
    mpf_t *const c_re = &_imgdata->centre_re;
    mpf_t *const c_im = &_imgdata->centre_im;

    const unsigned int prog = (width * height) / 100;
    unsigned int ctr = 0;
#pragma omp parallel for collapse(2)
    for (unsigned int i = 0; i < width; ++i) {
        for (unsigned int j = 0; j < height; ++j) {
            PixelData *const px = &_imgdata->data[i * height + j];

            mpf_set_d(px->re, (-0.5 * width + 1.0 * i));
            mpf_mul(px->re, px->re, *d);
            mpf_add(px->re, px->re, *c_re);

            mpf_set_d(px->im, (-0.5 * height + 1.0 * j));
            mpf_mul(px->im, px->im, *d);
            mpf_add(px->im, px->im, *c_im);

            const int tid = omp_get_thread_num();
            PixelDataBuffer *const buf = &tbuf[tid];
            _pixeldata_iterate(px, buf);

            _imgdata->framebuf[i * height + j] = px->itrs;
#pragma omp atomic
            ++ctr;
            if (ctr % prog == 0) {
                const unsigned int prct = ctr / prog;
                printf("\33[2K\rRasterizing complex plane... (%3u %%)", prct);
                fflush(stdout);
            }
        }
    }
    printf("\n");
}

void
ImageData_init(void)
{
    if (_imgdata != NULL) {
        return;
    }

    mpf_set_default_prec(GLOBAL_SETTINGS->prec);
    _imagedata_alloc();
    _imagedata_update();
}

void
ImageData_free(void)
{
    if (_imgdata == NULL) {
        return;
    }

    const unsigned int dims = GLOBAL_SETTINGS->width * GLOBAL_SETTINGS->height;
    for (unsigned int i = 0; i < dims; ++i) {
        PixelData *const px = &_imgdata->data[i];
        mpf_clear(px->re);
        mpf_clear(px->im);
    }
    free(_imgdata->data);

    const int tnum = omp_get_max_threads();
    for (int i = 0; i < tnum; ++i) {
        PixelDataBuffer *const buf = &_imgdata->tbuf[i];
        mpf_clear(buf->re);
        mpf_clear(buf->im);
        mpf_clear(buf->re_sqr);
        mpf_clear(buf->im_sqr);
        mpf_clear(buf->abs_sqr);
        mpf_clear(buf->max_sqr);
    }
    free(_imgdata->tbuf);

    mpf_clear(_imgdata->action_buf);

    free(_imgdata->framebuf);

    free(_imgdata);
    _imgdata = NULL;
}

void
ImageData_action(Key key)
{
    mpf_t *const buf = &_imgdata->action_buf;
    mpf_t *const res = &_imgdata->resolution;
    mpf_t *const c_re = &_imgdata->centre_re;
    mpf_t *const c_im = &_imgdata->centre_im;
    switch (key) {
    case KEY_ZOOM_IN: {
        mpf_set_d(*buf, GLOBAL_SETTINGS->zoom_spd);
        mpf_mul(*res, *res, *buf);
    } break;
    case KEY_ZOOM_OUT: {
        mpf_set_d(*buf, GLOBAL_SETTINGS->zoom_spd);
        mpf_div(*res, *res, *buf);
    } break;
    case KEY_UP: {
        const double offs = _imgdata->height * GLOBAL_SETTINGS->scrl_spd;
        mpf_set_d(*buf, offs);
        mpf_mul(*buf, *buf, *res);
        mpf_sub(*c_im, *c_im, *buf);
    } break;
    case KEY_DOWN: {
        const double offs = _imgdata->height * GLOBAL_SETTINGS->scrl_spd;
        mpf_set_d(*buf, offs);
        mpf_mul(*buf, *buf, *res);
        mpf_add(*c_im, *c_im, *buf);
    } break;
    case KEY_LEFT: {
        const double offs = _imgdata->width * GLOBAL_SETTINGS->scrl_spd;
        mpf_set_d(*buf, offs);
        mpf_mul(*buf, *buf, *res);
        mpf_sub(*c_re, *c_re, *buf);
    } break;
    case KEY_RIGHT: {
        const double offs = _imgdata->width * GLOBAL_SETTINGS->scrl_spd;
        mpf_set_d(*buf, offs);
        mpf_mul(*buf, *buf, *res);
        mpf_add(*c_re, *c_re, *buf);
    } break;
    case KEY_RESET:
        mpf_set_d(*res, _default_resolution());
        mpf_set_d(*c_re, GLOBAL_SETTINGS->cntr_re);
        mpf_set_d(*c_im, GLOBAL_SETTINGS->cntr_im);
        break;
    case KEY_INVALID:
    default:
        return;
    }
    _imagedata_update();
}

const float *
get_pixel_data(void)
{
    return _imgdata->framebuf;
}
