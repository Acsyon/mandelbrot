/* data/image.h
 *
 * Header for image data objects
 *
 */

#ifndef MANDELBROT_DATA_IMAGE_H_INCLUDED
#define MANDELBROT_DATA_IMAGE_H_INCLUDED

#include <cutil/std/stdbool.h>

#include <app/key.h>
#include <app/settings.h>
#include <data/chunk.h>

/**
 * Opaque ImageData type
 */
typedef struct _imageData ImageData;

/**
 * Initializes and returns ImageData object according to Settings in App.
 *
 * @return ImageData object according to Settings in App
 */
ImageData *
ImageData_create(const Settings *settings);

/**
 * Frees memory pointed to by `imgdata`.
 *
 * @param[in] imgdata ImageData object to be freed
 */
void
ImageData_free(ImageData *imgdata);

/**
 * Registers action according to keypress in `key` to `imgdata`.
 *
 * @param[in] imgdata ImageData object to register action to
 * @param[in] key pressed key
 */
void
ImageData_register_action(ImageData *imgdata, enum Key key);

/**
 * Resumes registered action on `imgdata` for `mseconds` milliseconds.
 *
 * @param[in] imgdata ImageData object to resume action of
 * @param[in] mseconds number of milliseconds to resume action for
 */
void
ImageData_resume_action(ImageData *imgdata, unsigned int mseconds);

/**
 * Performs action according to keypress in `key` to `imgdata`.
 *
 * @param[in] imgdata ImageData object to perform action on
 * @param[in] key pressed key
 */
void
ImageData_perform_action(ImageData *imgdata, enum Key key);

/**
 * Updates all pixels in `chunk` of `imgdata`.
 *
 * @param[in] imgdata ImageData object to update chunk of
 * @param[in] chunk PixelChunk to update
 */
void
ImageData_update_chunk(ImageData *imgdata, PixelChunk *chunk);

/**
 * Returns array of pixel data in `imgdata`.
 *
 * @param[in] imgdata ImageData object to get pixel data of
 *
 * @return array of pixel data in `imgdata`
 */
const float *
ImageData_get_pixel_data(const ImageData *imgdata);

/**
 * Returns whether at least one pixel in `imgdata` has changed since the last
 * call of this function. Resets the flag.
 *
 * @param[in] imgdata ImageData object to check for changes
 *
 * @return Has at least one pixel in `imgdata` changed since last call?
 */
bool
ImageData_has_changed(ImageData *imgdata);

/**
 * Returns whether `imgdata` is currently working, i.e., whether there there are
 * calculations going on.
 *
 * @param[in] imgdata ImageData object to check for working state
 *
 * @return Is `imgdata` currently working?
 */
bool
ImageData_is_working(const ImageData *imgdata);

#endif /* MANDELBROT_DATA_IMAGE_H_INCLUDED */
