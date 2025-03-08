/* data.h
 *
 * Header for data objects
 *
 */

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include "key.h"

/**
 * Opaque ImageData type
 */
typedef struct ImageData ImageData;

/**
 * Initializes and returns ImageData object according to Settings in App.
 *
 * @return ImageData object according to Settings in App
 */
ImageData *
ImageData_app_init(void);

/**
 * Frees memory pointed to by `imgdata`.
 *
 * @param[in] imgdata pointer to ImageData object to be freed
 */
void
ImageData_free(ImageData *imgdata);

/**
 * Registers action according to keypress in `key` to `imgdata`.
 *
 * @param[in] imgdata pointer to ImageData object to register action to
 * @param[in] key pressed key
 */
void
ImageData_register_action(ImageData *imgdata, enum Key key);

/**
 * Performs registered action on `imgdata` for `mseconds`
 * milliseconds. Returns nonzero value if an action has been performed.
 *
 * @param[in] imgdata pointer to ImageData object to perform action on
 * @param[in] mseconds pressed key
 *
 * @return nonzero value if an action has been performed
 */
int
ImageData_perform_action(ImageData *imgdata, unsigned int mseconds);

/**
 * Returns array of pixel data in `imgdata`.
 *
 * @param[in] imgdata pointer to ImageData object to get pixel data of
 *
 * @return array of pixel data in `imgdata`
 */
const float *
ImageData_get_pixel_data(const ImageData *imgdata);

#endif /* DATA_H_INCLUDED */
