/* app/data.h
 *
 * Header for data objects
 *
 */

#ifndef MANDELBROT_APP_DATA_H_INCLUDED
#define MANDELBROT_APP_DATA_H_INCLUDED

#include <app/key.h>

#include <cutil/std/stdbool.h>

/**
 * Opaque GraphicsData type
 */
typedef struct _graphicsData GraphicsData;

/**
 * Initializes and returns GraphicsData object according to Settings in App.
 *
 * @return GraphicsData object according to Settings in App
 */
GraphicsData *
GraphicsData_app_init(void);

/**
 * Frees memory pointed to by `gfxdata`.
 *
 * @param[in] gfxdata GraphicsData object to be freed
 */
void
GraphicsData_free(GraphicsData *gfxdata);

/**
 * Registers action according to keypress in `key` to `gfxdata`.
 *
 * @param[in] gfxdata GraphicsData object to register action to
 * @param[in] key pressed key
 */
void
GraphicsData_register_action(GraphicsData *gfxdata, enum Key key);

/**
 * Performs registered action on `gfxdata` for `mseconds` milliseconds.
 *
 * @param[in] gfxdata GraphicsData object to resume action for
 * @param[in] mseconds number of milliseconds to resume action for
 */
void
GraphicsData_resume_action(GraphicsData *gfxdata, unsigned int mseconds);

/**
 * Performs action according to keypress in `key` to `gfxdata`.
 *
 * @param[in] gfxdata GraphicsData object to perform action on
 * @param[in] key pressed key
 */
void
GraphicsData_perform_action(GraphicsData *gfxdata, enum Key key);

/**
 * Returns array of pixel data in `gfxdata`.
 *
 * @param[in] gfxdata GraphicsData object to get pixel data of
 *
 * @return array of pixel data in `gfxdata`
 */
const float *
GraphicsData_get_pixel_data(const GraphicsData *gfxdata);

/**
 * Returns whether at least one pixel in `gfxdata` has changed since the last
 * call of this function.
 *
 * @param[in] gfxdata GraphicsData object to check for changes
 *
 * @return has at least one pixel in `gfxdata` changed since last call?
 */
bool
GraphicsData_has_changed(const GraphicsData *gfxdata);

/**
 * Returns whether `gfxdata` is currently working, i.e., whether there are
 * calculations going on.
 *
 * @param[in] gfxdata GraphicsData object to check for working state
 *
 * @return Is `gfxdata` currently working?
 */
bool
GraphicsData_is_working(const GraphicsData *gfxdata);

#endif /* MANDELBROT_APP_DATA_H_INCLUDED */
