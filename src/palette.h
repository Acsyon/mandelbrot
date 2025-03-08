/* palette.h
 *
 * Header for colour palette functions and manipulation
 *
 */

#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#define PALETTE_INVALID_POS -1.0f

/**
 * Typedef for palette functions.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
typedef uint32_t
Palette_fnc(float pos, const void *params);

/**
 * A plain greyscale palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_gray(float pos, const void *params);

/**
 * A plain red palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_red(float pos, const void *params);

/**
 * A plain green palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_green(float pos, const void *params);

/**
 * The OG plain blue palette.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_blue(float pos, const void *params);

/**
 * Palette using exponentially mapped iterations and HSV (as described on
 * Wikipedia) with an additional offset.
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params pointer to offset (struct AHSV)
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_exp_hsv_offset(float pos, const void *params);

/**
 * Palette using exponentially mapped iterations and HSV (as described on
 * Wikipedia).
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_exp_hsv(float pos, const void *params);

/**
 * Palette similar to the one used by Ultra Fractal
 *
 * @param[in] pos number of iterations as fraction of maximum iterations
 * @param[in] params parameters for palette function
 *
 * @return ARGB of palette as uint32_t
 */
uint32_t
Palette_ultra_fractal(float pos, const void *params);

/**
 * Array of all palette functions (for cycling and TRIP_MODE_LERP)
 */
extern Palette_fnc *const PALETTE_FUNCTIONS[];

/**
 * Size of palette-function array
 */
extern const size_t PALETTE_FUNCTION_COUNT;

/**
 * Opaque palette cycler object
 */
typedef struct PaletteCycler PaletteCycler;

/**
 * Initializes and returns newly malloc'd PaletteCycler object.
 *
 * @param[in] fncs function pointer array to palette functions
 * @param[in] num number of function pointers in array
 * @param[in] idx initial index
 *
 * @return newly malloc'd PaletteCycler object
 */
PaletteCycler *
PaletteCycler_create(Palette_fnc *const *fncs, size_t num, size_t idx);

/**
 * Frees memory pointed to by `cycler`.
 *
 * @param[in] cycler pointer to PaletteCycler object to be freed
 */
void
PaletteCycler_free(PaletteCycler *cycler);

/**
 * Cycles palettes in `cycler` and returns next palette function.
 *
 * @param[in] cycler PaletteCycler to cycle through
 *
 * @return next palette function in cycle
 */
Palette_fnc *
PaletteCycler_cycle_palette(PaletteCycler *cycler);

/**
 * Enumerator for different types of trip mode
 */
enum TripModeType { TRIP_MODE_PHASE, TRIP_MODE_LERP };

/**
 * Opaque trip mode generator object
 */
typedef struct TripModeGenerator TripModeGenerator;

/**
 * Initializes and returns newly malloc'd TripModeGenerator object.
 *
 * @param[in] type TripModeType to build generator with
 *
 * @return newly malloc'd TripModeGenerator object
 */
TripModeGenerator *
TripModeGenerator_create(enum TripModeType type);

/**
 * Frees memory pointed to by `tripgen`.
 *
 * @param[in] tripgen pointer to TripModeGenerator object to be freed
 */
void
TripModeGenerator_free(TripModeGenerator *tripgen);

/**
 * Advances colours in `tripgen` to next frame.
 *
 * @param[in] tripgen TripModeGenerator to advance colours of
 */
void
TripModeGenerator_advance(TripModeGenerator *tripgen);

/**
 * Returns palette function of `tripgen`.
 *
 * @param[in] tripgen TripModeGenerator to return palette function of
 *
 * @return palette function of `tripgen`
 */
Palette_fnc *
TripModeGenerator_get_palette(const TripModeGenerator *tripgen);

/**
 * Returns palette function parameters of `tripgen`.
 *
 * @param[in] tripgen TripModeGenerator to return palette function parameters of
 *
 * @return palette function parameters of `tripgen`
 */
const void *
TripModeGenerator_get_params(const TripModeGenerator *tripgen);

#endif /* PALETTE_H_INCLUDED */
