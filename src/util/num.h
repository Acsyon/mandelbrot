/* util/num.h
 *
 * Header for auxiliary numerical functions
 *
 */

#ifndef MANDELBROT_UTIL_NUM_H_INCLUDED
#define MANDELBROT_UTIL_NUM_H_INCLUDED

#include <cutil/std/stdlib.h>

/**
 * Performs a linear interpolation between the points (x0, y0) and (x1, y1),
 * i.e., returns y = f(x) for the linear function that is defined by these
 * points for x \elem [x0, x1] and NAN otherwise.
 *
 * @param[in] x0 x value of the first point
 * @param[in] x1 y value of the first point
 * @param[in] y0 x value of the second point
 * @param[in] y1 y value of the second point
 * @param[in] x x value to return interpolated value of
 *
 * @return y = f(x) for the linear function that is defined by the points (x0,
 * y0) and (x1, y1) points for x \elem [x0, x1] and NAN otherwise
 */
float
lerpf_single(float x0, float x1, float y0, float y1, float x);

/**
 * Performs a piecewise linear interpolation between the `num` points which are
 * defined by `xarr` and `yarr`. Result is undefined if `xarr` is not sorted.
 * Returns NAN if x is not inside the interval [xarr[0], xarr[num-1]].
 *
 * @param[in] num number of points (for both `xarr` and `yarr`)
 * @param[in] xarr array of x values
 * @param[in] yarr array of y values
 * @param[in] x x value to return interpolated value of
 *
 * @return interpolated value if x is inside the interval [xarr[0],
 * xarr[num-1]], NAN otherwise
 */
float
lerpf(size_t num, const float *xarr, const float *yarr, float x);

#endif /* MANDELBROT_UTIL_NUM_H_INCLUDED */
