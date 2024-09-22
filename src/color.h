/* color.h
 *
 * Header for color space shenanigans
 *
 */

#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <stdint.h>

/**
 * Typedef for palette functions.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
typedef uint32_t
Palette_fnc(float pos);

/**
 * A plain greyscale palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_gray(float pos);

/**
 * A plain red palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_red(float pos);

/**
 * A plain green palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_green(float pos);

/**
 * The OG plain blue palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_blue(float pos);

/**
 * Palette using exponentially mapped iterations and HSV (as described on
 * Wikipedia).
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_exp_hsv(float pos);

/**
 * Palette similar to the one used by Ultra Fractal
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_ultra_fractal(float pos);

#endif /* COLOR_H_INCLUDED */
