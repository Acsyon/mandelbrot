/* net/chunk.h
 *
 * Header for chunk data objects to be sent over the network
 *
 */

#ifndef MANDELBROT_NET_CHUNK_H_INCLUDED
#define MANDELBROT_NET_CHUNK_H_INCLUDED

#include <data/chunk.h>
#include <data/pixel.h>

/**
 * Struct containing data for each chunk
 */
typedef struct {
    enum ChunkParams params;
    enum ChunkState state;
    uint8_t idx_re;
    uint8_t idx_im;
    uint16_t *pxdata;
    enum PixelState *pxstates;
} NetworkChunk;

NetworkChunk *
NetworkChunk_create(const ChunkParams *params);

void
NetworkChunk_free(NetworkChunk *chunk);

void
NetworkChunk_fill(NetworkChunk *netchunk, const PixelChunk *pxchunk);

#endif /* MANDELBROT_NET_CHUNK_H_INCLUDED */
