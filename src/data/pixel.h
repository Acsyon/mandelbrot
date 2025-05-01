/* data/pixel.h
 *
 * Header for pixel data objects
 *
 */

#ifndef MANDELBROT_DATA_PIXEL_H_INCLUDED
#define MANDELBROT_DATA_PIXEL_H_INCLUDED

#include <inttypes.h>

#include <gmp.h>

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
} PixelDataBuffer;

/**
 * Initializes fields in `buf` with `max_sqr` set explicitly.
 *
 * @param[in] buf PixelDataBuffer object to initialize
 * @param[in] max_sqr maximum absolute square for iteration
 */
void
PixelDataBuffer_init(PixelDataBuffer *buf, mpf_t max_sqr);

/**
 * Clears fields in `buf`.
 *
 * @param[in] buf PixelDataBuffer object to clear
 */
void
PixelDataBuffer_clear(PixelDataBuffer *buf);

/**
 * Sets precision of mpf fields in `buf` to `prec`.
 *
 * @param[in] buf PixelDataBuffer object to set precision of
 * @param[in] prec precision to set to
 */
void
PixelDataBuffer_set_prec(PixelDataBuffer *buf, mp_bitcnt_t prec);

/**
 * Possible pixel states
 */
enum PixelState {
    PIXEL_STATE_INVALID = -1,
    PIXEL_STATE_VALID = 0,
    PIXEL_STATE_INTERPOLATED,
};

/**
 * Struct containing data for each pixel
 */
typedef struct {
    mpf_t re;
    mpf_t im;
    float itrs;
    enum PixelState state;
} PixelData;

/**
 * Initializes fields in `px`.
 *
 * @param[in] px PixelData object to initialize
 */
void
PixelData_init(PixelData *px);

/**
 * Clears fields in `px`.
 *
 * @param[in] px PixelData object to clear
 */
void
PixelData_clear(PixelData *px);

/**
 * Sets precision of mpf fields in `px` to `prec`.
 *
 * @param[in] px PixelData object to set precision of
 * @param[in] prec precision to set to
 */
void
PixelData_set_prec(PixelData *px, mp_bitcnt_t prec);

/**
 * Performs actual Mandelbrot iterations on PixelData `px` for position in
 * PixelDataBuffer `buf` up to at most `max_itrs`.
 *
 * @param[in] px PixelData to work with
 * @param[in] buf PixelDataBuffer to use for iteration
 * @param[in] max_itrs maximum number of iterations to perform
 */
void
PixelData_iterate(PixelData *px, PixelDataBuffer *buf, uint16_t max_itrs);

#endif /* MANDELBROT_DATA_PIXEL_H_INCLUDED */
