#include "pixel.h"

#include <visuals/palette.h>

#define PERIODICITY_CHECK_CYCLE_LENGTH 25
#define MPF_SIMILARITY_THRESHOLD 1.0e-6

static inline int
_mpf_is_similar(mpf_srcptr lhs, mpf_srcptr rhs, mpf_ptr tmp, double reldiff)
{
    mpf_reldiff(tmp, lhs, rhs);
    mpf_abs(tmp, tmp);
    return (mpf_cmp_d(tmp, reldiff) < 0);
}

#define MPF_IS_SIMILAR(lhs, rhs, tmp)                                          \
    _mpf_is_similar((lhs), (rhs), (tmp), MPF_SIMILARITY_THRESHOLD)

void
PixelDataBuffer_init(PixelDataBuffer *buf, mpf_t max_sqr)
{
    mpf_init(buf->re);
    mpf_init(buf->im);
    mpf_init(buf->re_sqr);
    mpf_init(buf->im_sqr);
    mpf_init(buf->abs_sqr);
    mpf_init(buf->max_sqr);
    mpf_init(buf->re_old);
    mpf_init(buf->im_old);
    mpf_init(buf->tmp);

    mpf_set(buf->max_sqr, max_sqr);
}

void
PixelDataBuffer_clear(PixelDataBuffer *buf)
{
    mpf_clear(buf->re);
    mpf_clear(buf->im);
    mpf_clear(buf->re_sqr);
    mpf_clear(buf->im_sqr);
    mpf_clear(buf->abs_sqr);
    mpf_clear(buf->max_sqr);
    mpf_clear(buf->re_old);
    mpf_clear(buf->im_old);
    mpf_clear(buf->tmp);
}

void
PixelDataBuffer_set_prec(PixelDataBuffer *buf, mp_bitcnt_t prec)
{
    mpf_set_prec(buf->re, prec);
    mpf_set_prec(buf->im, prec);
    mpf_set_prec(buf->re_sqr, prec);
    mpf_set_prec(buf->im_sqr, prec);
    mpf_set_prec(buf->abs_sqr, prec);
    mpf_set_prec(buf->max_sqr, prec);
    mpf_set_prec(buf->re_old, prec);
    mpf_set_prec(buf->im_old, prec);
    mpf_set_prec(buf->tmp, prec);
}

void
PixelData_init(PixelData *px)
{
    mpf_init(px->re);
    mpf_init(px->im);
    px->state = PIXEL_STATE_INVALID;
    px->itrs = PALETTE_INVALID_POS;
}

void
PixelData_clear(PixelData *px)
{
    mpf_clear(px->re);
    mpf_clear(px->im);
}

void
PixelData_set_prec(PixelData *px, mp_bitcnt_t prec)
{
    mpf_set_prec(px->re, prec);
    mpf_set_prec(px->im, prec);
}

void
PixelData_iterate(PixelData *px, PixelDataBuffer *buf, uint16_t max_itrs)
{
    mpf_set_ui(buf->re, 0UL);
    mpf_set_ui(buf->im, 0UL);

    mpf_set_ui(buf->re_old, 0UL);
    mpf_set_ui(buf->im_old, 0UL);

    uint16_t period = 0;
    for (uint16_t itrs = 1; itrs <= max_itrs; ++itrs) {
        mpf_mul(buf->re_sqr, buf->re, buf->re);
        mpf_mul(buf->im_sqr, buf->im, buf->im);

        mpf_mul_ui(buf->im, buf->im, 2UL);
        mpf_mul(buf->im, buf->im, buf->re);
        mpf_add(buf->im, buf->im, px->im);

        mpf_sub(buf->re, buf->re_sqr, buf->im_sqr);
        mpf_add(buf->re, buf->re, px->re);

        mpf_add(buf->abs_sqr, buf->re_sqr, buf->im_sqr);
        if (mpf_cmp(buf->abs_sqr, buf->max_sqr) > 0) {
            px->itrs = 1.0F * itrs / max_itrs;
            return;
        }

        if (MPF_IS_SIMILAR(buf->re, buf->re_old, buf->tmp)
            && MPF_IS_SIMILAR(buf->im, buf->im_old, buf->tmp))
        {
            px->itrs = 0.0F;
            return;
        }

        ++period;
        if (period > PERIODICITY_CHECK_CYCLE_LENGTH) {
            period = 0;
            mpf_set(buf->re_old, buf->re);
            mpf_set(buf->im_old, buf->im);
        }
    }

    px->itrs = 0.0F; /* Converged I guess.. */
}
