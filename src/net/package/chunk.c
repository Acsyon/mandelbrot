#include <net/chunk.h>

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

NetworkChunk *
NetworkChunk_create(const ChunkParams *params)
{
    NetworkChunk *const chunk = malloc(sizeof *chunk);

    chunk->params = *params;
    chunk->state = CHUNK_STATE_INVALID;

    chunk->idx_re = 0;
    chunk->idx_im = 0;

    const uint32_t num_tot = params->num_px_re * params->num_px_im;
    chunk->pxdata = malloc(num_tot * sizeof *chunk->pxdata);
    chunk->pxstates = malloc(num_tot * sizeof *chunk->pxstates);

    return chunk;
}

void
NetworkChunk_free(NetworkChunk *chunk)
{
    CUTIL_RETURN_IF_NULL(chunk);

    free(chunk->pxdata);
    free(chunk->pxstates);

    free(chunk);
}

void
NetworkChunk_fill(NetworkChunk *netchunk, const PixelChunk *pxchunk)
{
    CUTIL_NULL_CHECK(netchunk);
    CUTIL_NULL_CHECK(pxchunk);

    netchunk->state = pxchunk->state;
}
