#include <data/chunk.h>

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#include <data/image.h>
#include <visuals/palette.h>

static void
_pixelChunk_shift_aux(
  uint8_t num_chnks, int8_t shift, uint8_t *p_idx, enum ChunkState *p_state
)
{
    CUTIL_RETURN_IF_VAL(shift, 0);

    const uint8_t idx_old = *p_idx;
    const int16_t new_idx_raw = idx_old - shift;
    const uint8_t new_idx = *p_idx = (new_idx_raw + num_chnks) % num_chnks;

    /* In case of overflow, chunk has to be recreated */
    if (new_idx != new_idx_raw) {
        *p_state = CHUNK_STATE_INVALID;
    }
}

static inline void
_pixelChunk_shift_re(PixelChunk *chunk, const ChunkData *chunks, int8_t shift)
{
    _pixelChunk_shift_aux(chunks->num_re, shift, &chunk->idx_re, &chunk->state);
}

static inline void
_pixelChunk_shift_im(PixelChunk *chunk, const ChunkData *chunks, int8_t shift)
{
    _pixelChunk_shift_aux(chunks->num_im, shift, &chunk->idx_im, &chunk->state);
}

void
ChunkParams_init(ChunkParams *params, const Settings *settings)
{
    params->stride = settings->height;
    params->num_px_re = settings->width / settings->num_chnks_re;
    params->num_px_im = settings->height / settings->num_chnks_im;
}

void
ChunkData_init(ChunkData *chunks, const Settings *settings, PixelData *px)
{
    ChunkParams *const params = &chunks->params;
    ChunkParams_init(params, settings);

    const uint8_t num_chnks_re = chunks->num_re = settings->num_chnks_re;
    const uint8_t num_chnks_im = chunks->num_im = settings->num_chnks_im;

    const uint16_t dims = num_chnks_re * num_chnks_im;
    chunks->data = malloc(dims * sizeof *chunks->data);

    const uint16_t stride = params->stride;
    const uint16_t num_px_re = params->num_px_re;
    const uint16_t num_px_im = params->num_px_im;

    for (uint8_t idx_chnk_re = 0; idx_chnk_re < num_chnks_re; ++idx_chnk_re) {
        for (uint8_t idx_chnk_im = 0; idx_chnk_im < num_chnks_im; ++idx_chnk_im)
        {
            const uint16_t idx_chnk = idx_chnk_re * num_chnks_im + idx_chnk_im;
            PixelChunk *const chunk = &chunks->data[idx_chnk];
            chunk->idx_re = idx_chnk_re;
            chunk->idx_im = idx_chnk_im;
            chunk->state = CHUNK_STATE_INVALID;

            const uint16_t idx_px_re = idx_chnk_re * num_px_re;
            const uint16_t idx_px_im = idx_chnk_im * num_px_im;
            const uint32_t idx_px = idx_px_re * stride + idx_px_im;
            chunk->data = &px[idx_px];
        }
    }
}

void
ChunkData_clear(ChunkData *chunks)
{
    free(chunks->data);
}

void
PixelChunk_invalidate_all_pixels(PixelChunk *chunk, const ChunkData *chunks)
{
    const ChunkParams *const params = &chunks->params;
    const uint16_t num_px_re = params->num_px_re;
    const uint16_t num_px_im = params->num_px_im;

    for (uint16_t idx_px_re = 0; idx_px_re < num_px_re; ++idx_px_re) {
        for (uint16_t idx_px_im = 0; idx_px_im < num_px_im; ++idx_px_im) {
            const uint16_t stride = params->stride;
            const uint32_t idx_px = idx_px_re * stride + idx_px_im;
            PixelData *const px = &chunk->data[idx_px];
            px->state = PIXEL_STATE_INVALID;
            px->itrs = PALETTE_INVALID_POS;
        }
    }

    chunk->state = CHUNK_STATE_INVALID;
}

void
PixelChunk_zoom(PixelChunk *chunk, const ChunkData *chunks, int8_t stages)
{
    CUTIL_UNUSED(chunk);
    CUTIL_UNUSED(chunks);
    CUTIL_UNUSED(stages);
}

void
PixelChunk_callback_update(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
)
{
    CUTIL_UNUSED(chunks);
    const ImageData *const imgdata = vparams;
    ImageData_update_chunk(imgdata, chunk);
}

void
PixelChunk_callback_shift(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
)
{
    const int8_t *const shifts = vparams;
    const int8_t shift_re = shifts[0];
    const int8_t shift_im = shifts[1];

    _pixelChunk_shift_re(chunk, chunks, shift_re);
    _pixelChunk_shift_im(chunk, chunks, shift_im);

    if (chunk->state == CHUNK_STATE_INVALID) {
        PixelChunk_invalidate_all_pixels(chunk, chunks);
    }
}

void
PixelChunk_callback_zoom(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
)
{
    const int8_t stages = *(const int8_t *) vparams;
    PixelChunk_zoom(chunk, chunks, stages);
    PixelChunk_invalidate_all_pixels(chunk, chunks);
}

void
PixelChunk_callback_reset(
  PixelChunk *chunk, const ChunkData *chunks, const void *vparams
)
{
    CUTIL_UNUSED(vparams);
    PixelChunk_invalidate_all_pixels(chunk, chunks);
}
