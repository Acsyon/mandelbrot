/* data/chunk.h
 *
 * Header for chunk data objects
 *
 */

#ifndef MANDELBROT_DATA_CHUNK_H_INCLUDED
#define MANDELBROT_DATA_CHUNK_H_INCLUDED

#include <app/settings.h>
#include <data/pixel.h>

/**
 * Possible chunk states
 */
enum ChunkState {
    CHUNK_STATE_INVALID = -1,
    CHUNK_STATE_VALID = 0,
    CHUNK_STATE_INTERPOLATED,
};

/**
 * Struct containing the parameters of the chunks
 */
typedef struct {
    uint16_t stride;
    uint16_t num_px_re;
    uint16_t num_px_im;
} ChunkParams;

/**
 * Initializes fields in `params` according to `settings`.
 *
 * @param[in] params ChunkParams object to initialize
 * @param[in] settings Settings object to base initialization on
 */
void
ChunkParams_init(ChunkParams *params, const Settings *settings);

/**
 * Struct containing data for each chunk
 */
typedef struct {
    uint8_t idx_re;
    uint8_t idx_im;
    PixelData *data;
    enum ChunkState state;
} PixelChunk;

/**
 * Struct containing data for all chunks
 */
typedef struct {
    ChunkParams params;
    uint8_t num_re;
    uint8_t num_im;
    PixelChunk *data;
} ChunkData;

/**
 * Initializes fields in `chunks` according to `settings` and `px`.
 *
 * @param[in] chunks ChunkData object to initialize
 * @param[in] settings Settings object to base initialization on
 * @param[in] px PixelData object to base initialization on
 */
void
ChunkData_init(ChunkData *chunks, const Settings *settings, PixelData *px);

/**
 * Clears fields in `chunks`.
 *
 * @param[in] chunks ChunkData object to clear
 */
void
ChunkData_clear(ChunkData *chunks);

/**
 * Zooms `chunk` in `chunks` by `stages` where each stage corresponds to zooming
 * by the zoom factor. A positive zoom stage amounts to zooming in. Currently,
 * this function is a no op as this functionality is handled by setting the
 * units per pixel.
 *
 * @param[in] chunk PixelChunk object to zoom
 * @param[in] stages zoom stages
 */
void
PixelChunk_zoom(PixelChunk *chunk, int8_t stages);

/**
 * Auxiliary struct for parameters of callback function
 */
typedef struct {
    struct _imageData *imgdata;
    const ChunkData *chunks;
    const void *vparams;
} ChunkCallbackParams;

/**
 * Typedef for PixelChunk action callback function
 *
 * @param[in] chunk PixelChunk object to perform action on
 * @param[in] params parameters for callback function
 */
typedef void
PixelChunk_callback(PixelChunk *chunk, const ChunkCallbackParams *params);

/**
 * Updates all pixels of `chunk`.
 *
 * @param[in] chunk PixelChunk object to update
 * @param[in] vparams unused
 */
void
PixelChunk_callback_update(
  PixelChunk *chunk, const ChunkCallbackParams *params
);

/**
 * Shifts `chunk` according to `vparams`, which is an integer array with two
 * elements containing the real and imaginary shifts as its first and second
 * element, respectively.
 *
 * @param[in] chunk PixelChunk object to shift
 * @param[in] vparams integer array of shifts
 */
void
PixelChunk_callback_shift(PixelChunk *chunk, const ChunkCallbackParams *params);

/**
 * Zooms `chunk` according to `vparams`, which is an integer describing the
 * 'zoom stages' where each stage corresponds to zooming by the zoom factor. A
 * positive zoom stage amounts to zooming in.
 *
 * @param[in] chunk PixelChunk object to zoom
 * @param[in] vparams zoom stage
 */
void
PixelChunk_callback_zoom(PixelChunk *chunk, const ChunkCallbackParams *params);

/**
 * Resets, i.e., invalidates, all pixels of `chunk`.
 *
 * @param[in] chunk PixelChunk object to reset
 * @param[in] vparams unused
 */
void
PixelChunk_callback_reset(PixelChunk *chunk, const ChunkCallbackParams *params);

#endif /* MANDELBROT_DATA_CHUNK_H_INCLUDED */
