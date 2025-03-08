#include "num.h"

#include <math.h>

#include "log.h"

#define DEFINE_INLINE_FUNC_LERP_SINGLE_GENERIC(TYPE)                           \
    static inline TYPE _inline_lerp_single_##TYPE(                             \
      TYPE x0, TYPE x1, TYPE y0, TYPE y1, TYPE x                               \
    )                                                                          \
    {                                                                          \
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);                          \
    }

DEFINE_INLINE_FUNC_LERP_SINGLE_GENERIC(float)

#define DEFINE_AUX_FUNC_LERP_SINGLE_GENERIC(TYPE)                              \
    static float _aux_lerp_single_##TYPE(                                      \
      TYPE x0, TYPE x1, TYPE y0, TYPE y1, TYPE x                               \
    )                                                                          \
    {                                                                          \
        if (x < x0 || x > x1) {                                                \
            log_err("Outside of interpolation interval");                      \
            return NAN;                                                        \
        }                                                                      \
        return _inline_lerp_single_##TYPE(x0, x1, y0, y1, x);                  \
    }

DEFINE_AUX_FUNC_LERP_SINGLE_GENERIC(float)

float
lerpf_single(float x0, float x1, float y0, float y1, float x)
{
    return _aux_lerp_single_float(x0, x1, y0, y1, x);
}

/**
 * Algorithm stolen from GSL
 */
#define DEFINE_FUNC_BSEARCH_GENERIC(TYPE)                                      \
    static size_t _bsearch_##TYPE(                                             \
      const TYPE *xarr, TYPE x, size_t idx_lo, size_t idx_hi                   \
    )                                                                          \
    {                                                                          \
        size_t ilo = idx_lo;                                                   \
        size_t ihi = idx_hi;                                                   \
        while (ihi > ilo + 1) {                                                \
            const size_t i = (ihi + ilo) / 2;                                  \
            if (xarr[i] > x) {                                                 \
                ihi = i;                                                       \
            } else {                                                           \
                ilo = i;                                                       \
            }                                                                  \
        }                                                                      \
        return ilo;                                                            \
    }

DEFINE_FUNC_BSEARCH_GENERIC(float)

#define DEFINE_AUX_FUNC_LERP_GENERIC(TYPE)                                     \
    static float _aux_lerp_##TYPE(                                             \
      size_t num, const TYPE *xarr, const TYPE *yarr, TYPE x                   \
    )                                                                          \
    {                                                                          \
        const size_t idx_lo = 0;                                               \
        const size_t idx_hi = num - 1;                                         \
        if (x < xarr[idx_lo] || x > xarr[idx_hi]) {                            \
            log_err("Outside of interpolation interval");                      \
            return NAN;                                                        \
        }                                                                      \
        const size_t idx = _bsearch_##TYPE(xarr, x, idx_lo, idx_hi);           \
        const float x0 = xarr[idx];                                            \
        const float x1 = xarr[idx + 1];                                        \
        const float y0 = yarr[idx];                                            \
        const float y1 = yarr[idx + 1];                                        \
        return _inline_lerp_single_##TYPE(x0, x1, y0, y1, x);                  \
    }

DEFINE_AUX_FUNC_LERP_GENERIC(float)

float
lerpf(size_t num, const float *xarr, const float *yarr, float x)
{
    return _aux_lerp_float(num, xarr, yarr, x);
}
