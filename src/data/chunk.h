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
    int stride;
    int num_px_re;
    int num_px_im;
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
    int idx_re;
    int idx_im;
    PixelData *data;
    enum ChunkState state;
} PixelChunk;

/**
 * Struct containing data for all chunks
 */
typedef struct {
    ChunkParams params;
    int num_re;
    int num_im;
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
 * Invalidates all pixels in `chunk` in `chunks`.
 *
 * @param[in] chunk PixelChunk object to invalidate all pixels of
 * @param[in] chunks ChunkData object to get chunk parameters from
 */
void
PixelChunk_invalidate_all_pixels(PixelChunk *chunk, const ChunkData *chunks);

/**
 * Zooms `chunk` in `chunks` by `stages` where each stage corresponds to zooming
 * by the zoom factor. A positive zoom stage amounts to zooming in. Currently,
 * this function is a no op as this functionality is handled by setting the
 * units per pixel.
 *
 * @param[in] chunk PixelChunk object to zoom
 * @param[in] chunks ChunkData object to get chunk parameters from
 * @param[in] stages zoom stages
 */
void
PixelChunk_zoom(PixelChunk *chunk, const ChunkData *chunks, int stages);

/**
 * Typedef for PixelChunk action callback function
 *
 * @param[in] chunk PixelChunk object to perform action on
 * @param[in] chunks ChunkData object to get chunk parameters from
 * @param[in] vparams parameters for callback function
 */
typedef void
PixelChunk_callback(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
);

/**
 * Updates all pixels of `chunk`.
 *
 * @param[in] chunk PixelChunk object to update
 * @param[in] chunks ChunkData object to get chunk parameters from
 * @param[in] vparams ImageData object the chunks live in
 */
void
PixelChunk_callback_update(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
);

/**
 * Shifts `chunk` according to `vparams`, which is an integer array with two
 * elements containing the real and imaginary shifts as its first and second
 * element, respectively.
 *
 * @param[in] chunk PixelChunk object to shift
 * @param[in] chunks ChunkData object to get chunk parameters from
 * @param[in] vparams integer array of shifts
 */
void
PixelChunk_callback_shift(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
);

/**
 * Zooms `chunk` according to `vparams`, which is an integer describing the
 * 'zoom stages' where each stage corresponds to zooming by the zoom factor. A
 * positive zoom stage amounts to zooming in.
 *
 * @param[in] chunk PixelChunk object to zoom
 * @param[in] chunks ChunkData object to get chunk parameters from
 * @param[in] vparams zoom stage
 */
void
PixelChunk_callback_zoom(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
);

/**
 * Resets, i.e., invalidates, all pixels of `chunk`.
 *
 * @param[in] chunk PixelChunk object to reset
 * @param[in] chunks ChunkData object to get chunk parameters from
 * @param[in] vparams unused
 */
void
PixelChunk_callback_reset(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
);

#endif /* MANDELBROT_DATA_CHUNK_H_INCLUDED */
