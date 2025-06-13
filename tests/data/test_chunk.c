#include "unity.h"

#include <cutil/std/stdlib.h>

#include <app/settings.h>
#include <data/chunk.h>
#include <data/pixel.h>
#include <visuals/palette.h>

void
_should_initializeChunkParamsCorrectly_when_provideSettings(void)
{
    /* Arrange */
    Settings settings = {0};
    settings.width = 800;
    settings.height = 600;
    settings.num_chnks_re = 4;
    settings.num_chnks_im = 3;

    ChunkParams params;

    /* Act */
    ChunkParams_init(&params, &settings);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(600, params.stride);
    TEST_ASSERT_EQUAL_INT(200, params.num_px_re);
    TEST_ASSERT_EQUAL_INT(200, params.num_px_im);
}

void
_should_initializeChunkDataCorrectly_when_provideSettingsAndPixelData(void)
{
    /* Arrange */
    Settings settings = {0};
    settings.width = 800;
    settings.height = 600;
    settings.num_chnks_re = 4;
    settings.num_chnks_im = 3;

    PixelData *const px = malloc(settings.width * settings.height * sizeof *px);
    ChunkData chunks = {0};

    /* Act */
    ChunkData_init(&chunks, &settings, px);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(4, chunks.num_re);
    TEST_ASSERT_EQUAL_INT(3, chunks.num_im);
    TEST_ASSERT_NOT_NULL(chunks.data);

    for (int idx_chnk_re = 0; idx_chnk_re < chunks.num_re; ++idx_chnk_re) {
        for (int idx_chnk_im = 0; idx_chnk_im < chunks.num_im; ++idx_chnk_im) {
            const PixelChunk *const chunk
              = &chunks.data[idx_chnk_re * chunks.num_im + idx_chnk_im];
            TEST_ASSERT_EQUAL_INT(idx_chnk_re, chunk->idx_re);
            TEST_ASSERT_EQUAL_INT(idx_chnk_im, chunk->idx_im);
            TEST_ASSERT_EQUAL_INT(CHUNK_STATE_INVALID, chunk->state);
            TEST_ASSERT_NOT_NULL(chunk->data);
        }
    }

    /* Cleanup */
    ChunkData_clear(&chunks);
    free(px);
}

void
_should_invalidateAllPixels_when_callInvalidateAllPixels(void)
{
    /* Arrange */
    Settings settings = {0};
    settings.width = 800;
    settings.height = 600;
    settings.num_chnks_re = 4;
    settings.num_chnks_im = 3;

    PixelData *const px = malloc(settings.width * settings.height * sizeof *px);
    ChunkData chunks = {0};
    ChunkData_init(&chunks, &settings, px);

    PixelChunk *const chunk = &chunks.data[0];

    /* Act */
    PixelChunk_invalidate_all_pixels(chunk, &chunks);

    /* Assert */
    for (int idx_px_re = 0; idx_px_re < chunks.params.num_px_re; ++idx_px_re) {
        for (int idx_px_im = 0; idx_px_im < chunks.params.num_px_im;
             ++idx_px_im)
        {
            const PixelData *const px
              = &chunk->data[idx_px_re * chunks.params.stride + idx_px_im];
            TEST_ASSERT_EQUAL_INT(PIXEL_STATE_INVALID, px->state);
            TEST_ASSERT_EQUAL_FLOAT(PALETTE_INVALID_POS, px->itrs);
        }
    }

    TEST_ASSERT_EQUAL_INT(CHUNK_STATE_INVALID, chunk->state);

    /* Cleanup */
    ChunkData_clear(&chunks);
    free(px);
}

void
_should_shiftChunkCorrectly_when_provideShiftParameters(void)
{
    /* Arrange */
    Settings settings = {0};
    settings.width = 800;
    settings.height = 600;
    settings.num_chnks_re = 4;
    settings.num_chnks_im = 3;

    PixelData *const px = malloc(settings.width * settings.height * sizeof *px);
    ChunkData chunks = {0};
    ChunkData_init(&chunks, &settings, px);

    PixelChunk *const chunk = &chunks.data[0];
    const int shifts[2] = {-1, 1};

    /* Act */
    PixelChunk_callback_shift(chunk, &chunks, shifts);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, chunk->idx_re);
    TEST_ASSERT_EQUAL_INT(2, chunk->idx_im);
    TEST_ASSERT_EQUAL_INT(CHUNK_STATE_INVALID, chunk->state);

    /* Cleanup */
    ChunkData_clear(&chunks);
    free(px);
}

void
_should_resetChunkCorrectly_when_callResetCallback(void)
{
    /* Arrange */
    Settings settings = {0};
    settings.width = 800;
    settings.height = 600;
    settings.num_chnks_re = 4;
    settings.num_chnks_im = 3;

    PixelData *const px = malloc(settings.width * settings.height * sizeof *px);
    ChunkData chunks = {0};
    ChunkData_init(&chunks, &settings, px);

    PixelChunk *const chunk = &chunks.data[0];

    /* Act */
    PixelChunk_callback_reset(chunk, &chunks, NULL);

    /* Assert */
    for (int idx_px_re = 0; idx_px_re < chunks.params.num_px_re; ++idx_px_re) {
        for (int idx_px_im = 0; idx_px_im < chunks.params.num_px_im;
             ++idx_px_im)
        {
            const PixelData *const px
              = &chunk->data[idx_px_re * chunks.params.stride + idx_px_im];
            TEST_ASSERT_EQUAL_INT(PIXEL_STATE_INVALID, px->state);
            TEST_ASSERT_EQUAL_FLOAT(PALETTE_INVALID_POS, px->itrs);
        }
    }

    TEST_ASSERT_EQUAL_INT(CHUNK_STATE_INVALID, chunk->state);

    /* Cleanup */
    ChunkData_clear(&chunks);
    free(px);
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(void)
{
    UNITY_BEGIN();

    RUN_TEST(_should_initializeChunkParamsCorrectly_when_provideSettings);
    RUN_TEST(
      _should_initializeChunkDataCorrectly_when_provideSettingsAndPixelData
    );
    RUN_TEST(_should_invalidateAllPixels_when_callInvalidateAllPixels);
    RUN_TEST(_should_shiftChunkCorrectly_when_provideShiftParameters);
    RUN_TEST(_should_resetChunkCorrectly_when_callResetCallback);
    
    return UNITY_END();
}
