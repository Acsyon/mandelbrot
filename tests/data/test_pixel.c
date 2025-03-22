#include "unity.h"

#include <data/pixel.h>
#include <visuals/palette.h>

static void
_pixelDataBuffer_assert_prec(const PixelDataBuffer *buf, mp_bitcnt_t prec)
{
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->re));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->im));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->re_sqr));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->im_sqr));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->abs_sqr));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->max_sqr));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->re_old));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->im_old));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(buf->tmp));
}

static void
_pixelData_assert_prec(const PixelData *px, mp_bitcnt_t prec)
{
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(px->re));
    TEST_ASSERT_EQUAL_UINT(prec, mpf_get_prec(px->im));
}

static void
_should_initBufferCorrectly_when_provideMaxSqr(void)
{
    /* Arrange */
    const double squares[] = {0.3, 2.4, 5.5, 10.2, 53.1, 1234.5};
    const size_t num = (sizeof squares) / (sizeof *squares);
    const mp_bitcnt_t prec = mpf_get_default_prec();

    PixelDataBuffer buf = {0};

    mpf_t max_sqr;
    mpf_init(max_sqr);

    for (size_t i = 0; i < num; ++i) {
        const double sqr = squares[i];
        mpf_set_d(max_sqr, sqr);

        /* Act */
        PixelDataBuffer_init(&buf, max_sqr);

        /* Assert */
        _pixelDataBuffer_assert_prec(&buf, prec);
        TEST_ASSERT_EQUAL_INT(0, mpf_cmp(max_sqr, buf.max_sqr));

        /* Cleanup */
        PixelDataBuffer_clear(&buf);
    }

    mpf_clear(max_sqr);
}

static void
_should_setBufferPrecCorrectly_when_providePrec(void)
{
    /* Arrange */
    const mp_bitcnt_t precs[] = {1024UL, 512UL, 256UL, 128UL, 64UL};
    const size_t num = (sizeof precs) / (sizeof *precs);

    PixelDataBuffer buf = {0};

    mpf_t max_sqr;
    mpf_init_set_d(max_sqr, 0.0);

    for (size_t i = 0; i < num; ++i) {
        const mp_bitcnt_t prec = precs[i];
        PixelDataBuffer_init(&buf, max_sqr);

        /* Act */
        PixelDataBuffer_set_prec(&buf, prec);

        /* Assert */
        _pixelDataBuffer_assert_prec(&buf, prec);

        /* Cleanup */
        PixelDataBuffer_clear(&buf);
    }

    mpf_clear(max_sqr);
}

static void
_should_initPixelCorrectly_when_initializePixelData(void)
{
    /* Arrange */
    const mp_bitcnt_t prec = mpf_get_default_prec();
    PixelData px = {0};

    /* Act */
    PixelData_init(&px);

    /* Assert */
    _pixelData_assert_prec(&px, prec);
    TEST_ASSERT_EQUAL_INT(PIXEL_STATE_INVALID, px.state);
    TEST_ASSERT_EQUAL_FLOAT(PALETTE_INVALID_POS, px.itrs);

    /* Cleanup */
    PixelData_clear(&px);
}

static void
_should_setPixelPrecCorrectly_when_providePrec(void)
{
    /* Arrange */
    const mp_bitcnt_t precs[] = {1024UL, 512UL, 256UL, 128UL, 64UL};
    const size_t num = (sizeof precs) / (sizeof *precs);

    PixelData px = {0};

    for (size_t i = 0; i < num; ++i) {
        const mp_bitcnt_t prec = precs[i];
        PixelData_init(&px);

        /* Act */
        PixelData_set_prec(&px, prec);

        /* Assert */
        _pixelData_assert_prec(&px, prec);

        /* Cleanup */
        PixelData_clear(&px);
    }
}

static void
_should_converge_when_pixelIsAtOrigin(void)
{
    /* Arrange */
    PixelData px = {0};
    PixelDataBuffer buf = {0};

    mpf_t max_sqr;
    mpf_init_set_d(max_sqr, 4.0);

    PixelData_init(&px);
    PixelDataBuffer_init(&buf, max_sqr);

    mpf_set_d(px.re, 0.0);
    mpf_set_d(px.im, 0.0);

    /* Act */
    PixelData_iterate(&px, &buf, 100);

    /* Assert */
    TEST_ASSERT_EQUAL_FLOAT(0.0F, px.itrs);

    /* Cleanup */
    PixelData_clear(&px);
    PixelDataBuffer_clear(&buf);
    mpf_clear(max_sqr);
}

void
_should_diverge_when_pixelIsOutsideMandelbrotSet(void)
{
    /* Arrange */
    PixelData px = {0};
    PixelDataBuffer buf = {0};

    mpf_t max_sqr;
    mpf_init_set_d(max_sqr, 4.0);

    PixelData_init(&px);
    PixelDataBuffer_init(&buf, max_sqr);

    mpf_set_d(px.re, 2.0);
    mpf_set_d(px.im, 2.0);

    /* Act */
    PixelData_iterate(&px, &buf, 100);

    /* Assert */
    TEST_ASSERT_GREATER_THAN_FLOAT(0.0F, px.itrs);

    /* Cleanup */
    PixelData_clear(&px);
    PixelDataBuffer_clear(&buf);
    mpf_clear(max_sqr);
}

void
_should_bePeriodic_when_pixelIsAtPeriodicPoint(void)
{
    /* Arrange */
    PixelData px = {0};
    PixelDataBuffer buf = {0};

    mpf_t max_sqr;
    mpf_init_set_d(max_sqr, 4.0);

    PixelData_init(&px);
    PixelDataBuffer_init(&buf, max_sqr);

    mpf_set_d(px.re, -1.0);
    mpf_set_d(px.im, 0.0);

    /* Act */
    PixelData_iterate(&px, &buf, 100);

    /* Assert */
    TEST_ASSERT_EQUAL_FLOAT(0.0F, px.itrs);

    /* Cleanup */
    PixelData_clear(&px);
    PixelDataBuffer_clear(&buf);
    mpf_clear(max_sqr);
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

    RUN_TEST(_should_initBufferCorrectly_when_provideMaxSqr);
    RUN_TEST(_should_setBufferPrecCorrectly_when_providePrec);
    RUN_TEST(_should_initPixelCorrectly_when_initializePixelData);
    RUN_TEST(_should_setPixelPrecCorrectly_when_providePrec);
    RUN_TEST(_should_converge_when_pixelIsAtOrigin);
    RUN_TEST(_should_diverge_when_pixelIsOutsideMandelbrotSet);
    RUN_TEST(_should_bePeriodic_when_pixelIsAtPeriodicPoint);
    
    return UNITY_END();
}
