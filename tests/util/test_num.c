#include "unity.h"

#include <cutil/std/math.h>

#include <util/num.h>

static void
_should_interpolateCorrectly_when_xIsWithinRange(void)
{
    /* Arrange */
    const float x0 = 0.0F;
    const float x1 = 1.0F;
    const float y0 = 0.0F;
    const float y1 = 1.0F;
    const float x = 0.5F;

    /* Act */
    const float result = lerpf_single(x0, x1, y0, y1, x);

    /* Assert */
    TEST_ASSERT_EQUAL_FLOAT(0.5F, result);
}

static void
_should_returnNan_when_xIsOutsideRange(void)
{
    /* Arrange */
    const float x0 = 0.0F;
    const float x1 = 1.0F;
    const float y0 = 0.0F;
    const float y1 = 1.0F;
    const float x = 2.0F;

    /* Act */
    const float result = lerpf_single(x0, x1, y0, y1, x);

    /* Assert */
    TEST_ASSERT_TRUE(isnan(result));
}

static void
_should_interpolatePiecewiseCorrectly_when_xIsWithinRange(void)
{
    /* Arrange */
    const size_t num = 3;
    const float xarr[] = {0.0F, 1.0F, 2.0F};
    const float yarr[] = {0.0F, 1.0F, 4.0F};
    const float x = 1.5F;

    /* Act */
    const float result = lerpf(num, xarr, yarr, x);

    /* Assert */
    TEST_ASSERT_EQUAL_FLOAT(2.5F, result);
}

static void
_should_returnNan_when_xIsOutsidePiecewiseRange(void)
{
    /* Arrange */
    const size_t num = 3;
    const float xarr[] = {0.0F, 1.0F, 2.0F};
    const float yarr[] = {0.0F, 1.0F, 4.0F};
    const float x = 3.0F;

    /* Act */
    const float result = lerpf(num, xarr, yarr, x);

    /* Assert */
    TEST_ASSERT_TRUE(isnan(result));
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

    RUN_TEST(_should_interpolateCorrectly_when_xIsWithinRange);
    RUN_TEST(_should_returnNan_when_xIsOutsideRange);
    RUN_TEST(_should_interpolatePiecewiseCorrectly_when_xIsWithinRange);
    RUN_TEST(_should_returnNan_when_xIsOutsidePiecewiseRange);

    return UNITY_END();
}
