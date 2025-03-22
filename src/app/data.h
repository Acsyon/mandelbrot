/* app/data.h
 *
 * Header for data objects
 *
 */

#ifndef MANDELBROT_APP_DATA_H_INCLUDED
#define MANDELBROT_APP_DATA_H_INCLUDED

#include <app/key.h>

/**
 * Opaque GraphicsData type
 */
typedef struct GraphicsData GraphicsData;

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
 * Performs registered action on `gfxdata` for `mseconds`
 * milliseconds. Returns nonzero value if an action has been performed.
 *
 * @param[in] gfxdata GraphicsData object to perform action on
 * @param[in] mseconds pressed key
 *
 * @return nonzero value if an action has been performed
 */
int
GraphicsData_perform_action(GraphicsData *gfxdata, unsigned int mseconds);

/**
 * Returns array of pixel data in `gfxdata`.
 *
 * @param[in] gfxdata GraphicsData object to get pixel data of
 *
 * @return array of pixel data in `gfxdata`
 */
const float *
GraphicsData_get_pixel_data(const GraphicsData *gfxdata);

#endif /* MANDELBROT_APP_DATA_H_INCLUDED */
