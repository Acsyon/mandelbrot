#include "unity.h"

#include <visuals/color.h>

#define ASSERT_EQUALS_RGB(LHS, RHS)                                            \
    TEST_ASSERT_EQUAL_UINT32(ARGB_to_u32(LHS), ARGB_to_u32(RHS))
#define ASSERT_EQUALS_HSV(LHS, RHS)                                            \
    TEST_ASSERT_EQUAL_UINT32(AHSV_to_u32(LHS), AHSV_to_u32(RHS))

static void
_should_convertARGBtoAHSV_when_callARGBtoAHSV(void)
{
    /* Arrange */
    const struct ARGB black_rgb = float_to_ARGB(0.0F, 0.0F, 0.0F);
    const struct ARGB white_rgb = float_to_ARGB(1.0F, 1.0F, 1.0F);
    const struct ARGB gray_rgb = float_to_ARGB(0.5F, 0.5F, 0.5F);

    const struct ARGB red_rgb = float_to_ARGB(1.0F, 0.0F, 0.0F);
    const struct ARGB lime_rgb = float_to_ARGB(0.0F, 1.0F, 0.0F);
    const struct ARGB blue_rgb = float_to_ARGB(0.0F, 0.0F, 1.0F);

    const struct ARGB yellow_rgb = float_to_ARGB(1.0F, 1.0F, 0.0F);
    const struct ARGB cyan_rgb = float_to_ARGB(0.0F, 1.0F, 1.0F);
    const struct ARGB magenta_rgb = float_to_ARGB(1.0F, 0.0F, 1.0F);

    const struct ARGB maroon_rgb = float_to_ARGB(0.5F, 0.0F, 0.0F);
    const struct ARGB darkgreen_rgb = float_to_ARGB(0.0F, 0.5F, 0.0F);
    const struct ARGB navy_rgb = float_to_ARGB(0.0F, 0.0F, 0.5F);

    const struct ARGB olive_rgb = float_to_ARGB(0.5F, 0.5F, 0.0F);
    const struct ARGB purple_rgb = float_to_ARGB(0.5F, 0.0F, 0.5F);
    const struct ARGB teal_rgb = float_to_ARGB(0.0F, 0.5F, 0.5F);

    const struct AHSV black_hsv = float_to_AHSV(0.0F, 0.0F, 0.0F);
    const struct AHSV white_hsv = float_to_AHSV(0.0F, 0.0F, 1.0F);
    const struct AHSV gray_hsv = float_to_AHSV(0.0F, 0.0F, 0.5F);

    const struct AHSV red_hsv = float_to_AHSV(000.0F / 360, 1.0F, 1.0F);
    const struct AHSV lime_hsv = float_to_AHSV(120.0F / 360, 1.0F, 1.0F);
    const struct AHSV blue_hsv = float_to_AHSV(240.0F / 360, 1.0F, 1.0F);

    const struct AHSV yellow_hsv = float_to_AHSV(060.0F / 360, 1.0F, 1.0F);
    const struct AHSV cyan_hsv = float_to_AHSV(180.0F / 360, 1.0F, 1.0F);
    const struct AHSV magenta_hsv = float_to_AHSV(300.0F / 360, 1.0F, 1.0F);

    const struct AHSV maroon_hsv = float_to_AHSV(000.0F / 360, 1.0F, 0.5F);
    const struct AHSV darkgreen_hsv = float_to_AHSV(120.0F / 360, 1.0F, 0.5F);
    const struct AHSV navy_hsv = float_to_AHSV(240.0F / 360, 1.0F, 0.5F);

    const struct AHSV olive_hsv = float_to_AHSV(060.0F / 360, 1.0F, 0.5F);
    const struct AHSV purple_hsv = float_to_AHSV(300.0F / 360, 1.0F, 0.5F);
    const struct AHSV teal_hsv = float_to_AHSV(180.0F / 360, 1.0F, 0.5F);

    /* Act */
    const struct AHSV black_hsv_conv = ARGB_to_AHSV(black_rgb);
    const struct AHSV white_hsv_conv = ARGB_to_AHSV(white_rgb);
    const struct AHSV gray_hsv_conv = ARGB_to_AHSV(gray_rgb);

    const struct AHSV red_hsv_conv = ARGB_to_AHSV(red_rgb);
    const struct AHSV lime_hsv_conv = ARGB_to_AHSV(lime_rgb);
    const struct AHSV blue_hsv_conv = ARGB_to_AHSV(blue_rgb);

    const struct AHSV yellow_hsv_conv = ARGB_to_AHSV(yellow_rgb);
    const struct AHSV cyan_hsv_conv = ARGB_to_AHSV(cyan_rgb);
    const struct AHSV magenta_hsv_conv = ARGB_to_AHSV(magenta_rgb);

    const struct AHSV maroon_hsv_conv = ARGB_to_AHSV(maroon_rgb);
    const struct AHSV darkgreen_hsv_conv = ARGB_to_AHSV(darkgreen_rgb);
    const struct AHSV navy_hsv_conv = ARGB_to_AHSV(navy_rgb);

    const struct AHSV olive_hsv_conv = ARGB_to_AHSV(olive_rgb);
    const struct AHSV purple_hsv_conv = ARGB_to_AHSV(purple_rgb);
    const struct AHSV teal_hsv_conv = ARGB_to_AHSV(teal_rgb);

    /* Assert */
    ASSERT_EQUALS_HSV(black_hsv, black_hsv_conv);
    ASSERT_EQUALS_HSV(white_hsv, white_hsv_conv);
    ASSERT_EQUALS_HSV(gray_hsv, gray_hsv_conv);

    ASSERT_EQUALS_HSV(red_hsv, red_hsv_conv);
    ASSERT_EQUALS_HSV(lime_hsv, lime_hsv_conv);
    ASSERT_EQUALS_HSV(blue_hsv, blue_hsv_conv);

    ASSERT_EQUALS_HSV(yellow_hsv, yellow_hsv_conv);
    ASSERT_EQUALS_HSV(cyan_hsv, cyan_hsv_conv);
    ASSERT_EQUALS_HSV(magenta_hsv, magenta_hsv_conv);

    ASSERT_EQUALS_HSV(maroon_hsv, maroon_hsv_conv);
    ASSERT_EQUALS_HSV(darkgreen_hsv, darkgreen_hsv_conv);
    ASSERT_EQUALS_HSV(navy_hsv, navy_hsv_conv);

    ASSERT_EQUALS_HSV(olive_hsv, olive_hsv_conv);
    ASSERT_EQUALS_HSV(purple_hsv, purple_hsv_conv);
    ASSERT_EQUALS_HSV(teal_hsv, teal_hsv_conv);
}

static void
_should_convertAHSVtoARGB_when_callAHSVtoARGB(void)
{
    /* Arrange */
    const struct ARGB black_rgb = float_to_ARGB(0.0F, 0.0F, 0.0F);
    const struct ARGB white_rgb = float_to_ARGB(1.0F, 1.0F, 1.0F);
    const struct ARGB gray_rgb = float_to_ARGB(0.5F, 0.5F, 0.5F);

    const struct ARGB red_rgb = float_to_ARGB(1.0F, 0.0F, 0.0F);
    const struct ARGB lime_rgb = float_to_ARGB(0.0F, 1.0F, 0.0F);
    const struct ARGB blue_rgb = float_to_ARGB(0.0F, 0.0F, 1.0F);

    const struct ARGB yellow_rgb = float_to_ARGB(1.0F, 1.0F, 0.0F);
    const struct ARGB cyan_rgb = float_to_ARGB(0.0F, 1.0F, 1.0F);
    const struct ARGB magenta_rgb = float_to_ARGB(1.0F, 0.0F, 1.0F);

    const struct ARGB maroon_rgb = float_to_ARGB(0.5F, 0.0F, 0.0F);
    const struct ARGB darkgreen_rgb = float_to_ARGB(0.0F, 0.5F, 0.0F);
    const struct ARGB navy_rgb = float_to_ARGB(0.0F, 0.0F, 0.5F);

    const struct ARGB olive_rgb = float_to_ARGB(0.5F, 0.5F, 0.0F);
    const struct ARGB purple_rgb = float_to_ARGB(0.5F, 0.0F, 0.5F);
    const struct ARGB teal_rgb = float_to_ARGB(0.0F, 0.5F, 0.5F);

    const struct AHSV black_hsv = float_to_AHSV(0.0F, 0.0F, 0.0F);
    const struct AHSV white_hsv = float_to_AHSV(0.0F, 0.0F, 1.0F);
    const struct AHSV gray_hsv = float_to_AHSV(0.0F, 0.0F, 0.5F);

    const struct AHSV red_hsv = float_to_AHSV(000.0F / 360, 1.0F, 1.0F);
    const struct AHSV lime_hsv = float_to_AHSV(120.0F / 360, 1.0F, 1.0F);
    const struct AHSV blue_hsv = float_to_AHSV(240.0F / 360, 1.0F, 1.0F);

    const struct AHSV yellow_hsv = float_to_AHSV(060.0F / 360, 1.0F, 1.0F);
    const struct AHSV cyan_hsv = float_to_AHSV(180.0F / 360, 1.0F, 1.0F);
    const struct AHSV magenta_hsv = float_to_AHSV(300.0F / 360, 1.0F, 1.0F);

    const struct AHSV maroon_hsv = float_to_AHSV(000.0F / 360, 1.0F, 0.5F);
    const struct AHSV darkgreen_hsv = float_to_AHSV(120.0F / 360, 1.0F, 0.5F);
    const struct AHSV navy_hsv = float_to_AHSV(240.0F / 360, 1.0F, 0.5F);

    const struct AHSV olive_hsv = float_to_AHSV(060.0F / 360, 1.0F, 0.5F);
    const struct AHSV purple_hsv = float_to_AHSV(300.0F / 360, 1.0F, 0.5F);
    const struct AHSV teal_hsv = float_to_AHSV(180.0F / 360, 1.0F, 0.5F);

    /* Act */
    const struct ARGB black_rgb_conv = AHSV_to_ARGB(black_hsv);
    const struct ARGB white_rgb_conv = AHSV_to_ARGB(white_hsv);
    const struct ARGB gray_rgb_conv = AHSV_to_ARGB(gray_hsv);

    const struct ARGB red_rgb_conv = AHSV_to_ARGB(red_hsv);
    const struct ARGB lime_rgb_conv = AHSV_to_ARGB(lime_hsv);
    const struct ARGB blue_rgb_conv = AHSV_to_ARGB(blue_hsv);

    const struct ARGB yellow_rgb_conv = AHSV_to_ARGB(yellow_hsv);
    const struct ARGB cyan_rgb_conv = AHSV_to_ARGB(cyan_hsv);
    const struct ARGB magenta_rgb_conv = AHSV_to_ARGB(magenta_hsv);

    const struct ARGB maroon_rgb_conv = AHSV_to_ARGB(maroon_hsv);
    const struct ARGB darkgreen_rgb_conv = AHSV_to_ARGB(darkgreen_hsv);
    const struct ARGB navy_rgb_conv = AHSV_to_ARGB(navy_hsv);

    const struct ARGB olive_rgb_conv = AHSV_to_ARGB(olive_hsv);
    const struct ARGB purple_rgb_conv = AHSV_to_ARGB(purple_hsv);
    const struct ARGB teal_rgb_conv = AHSV_to_ARGB(teal_hsv);

    /* Assert */
    ASSERT_EQUALS_RGB(black_rgb, black_rgb_conv);
    ASSERT_EQUALS_RGB(white_rgb, white_rgb_conv);
    ASSERT_EQUALS_RGB(gray_rgb, gray_rgb_conv);

    ASSERT_EQUALS_RGB(red_rgb, red_rgb_conv);
    ASSERT_EQUALS_RGB(lime_rgb, lime_rgb_conv);
    ASSERT_EQUALS_RGB(blue_rgb, blue_rgb_conv);

    ASSERT_EQUALS_RGB(yellow_rgb, yellow_rgb_conv);
    ASSERT_EQUALS_RGB(cyan_rgb, cyan_rgb_conv);
    ASSERT_EQUALS_RGB(magenta_rgb, magenta_rgb_conv);

    ASSERT_EQUALS_RGB(maroon_rgb, maroon_rgb_conv);
    ASSERT_EQUALS_RGB(darkgreen_rgb, darkgreen_rgb_conv);
    ASSERT_EQUALS_RGB(navy_rgb, navy_rgb_conv);

    ASSERT_EQUALS_RGB(olive_rgb, olive_rgb_conv);
    ASSERT_EQUALS_RGB(purple_rgb, purple_rgb_conv);
    ASSERT_EQUALS_RGB(teal_rgb, teal_rgb_conv);
}

static void
_should_convertFloatToARGB_when_callFloatToARGB(void)
{
    /* Arrange */
    const float r = 0.5F;
    const float g = 0.75F;
    const float b = 1.0F;

    /* Act */
    const struct ARGB result = float_to_ARGB(r, g, b);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(127, result.r);
    TEST_ASSERT_EQUAL_UINT8(191, result.g);
    TEST_ASSERT_EQUAL_UINT8(255, result.b);
}

static void
_should_convertU32ToARGB_when_callU32ToARGB(void)
{
    /* Arrange */
    const uint32_t u = 0xFFAABBCC;

    /* Act */
    const struct ARGB result = u32_to_ARGB(u);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(0xFF, result.a);
    TEST_ASSERT_EQUAL_UINT8(0xAA, result.r);
    TEST_ASSERT_EQUAL_UINT8(0xBB, result.g);
    TEST_ASSERT_EQUAL_UINT8(0xCC, result.b);
}

static void
_should_convertARGBToU32_when_callARGBToU32(void)
{
    /* Arrange */
    const struct ARGB argb = {0xFF, 0xAA, 0xBB, 0xCC};

    /* Act */
    const uint32_t result = ARGB_to_u32(argb);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(0xFFAABBCC, result);
}

static void
_should_convertFloatToAHSV_when_callFloatToAHSV(void)
{
    /* Arrange */
    const float h = 0.5F;
    const float s = 0.75F;
    const float v = 0.25F;

    /* Act */
    const struct AHSV result = float_to_AHSV(h, s, v);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT16(180, result.h);
    TEST_ASSERT_EQUAL_FLOAT(0.75F, result.s);
    TEST_ASSERT_EQUAL_FLOAT(0.25F, result.v);
}

static void
_should_convertAHSVToU32_when_callAHSVToU32(void)
{
    /* Arrange */
    const struct AHSV hsv = {0.0F, 120, 1.0F, 1.0F};

    /* Act */
    const uint32_t result = AHSV_to_u32(hsv);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(0x0000FF00, result);
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

    RUN_TEST(_should_convertARGBtoAHSV_when_callARGBtoAHSV);
    RUN_TEST(_should_convertAHSVtoARGB_when_callAHSVtoARGB);
    RUN_TEST(_should_convertFloatToARGB_when_callFloatToARGB);
    RUN_TEST(_should_convertU32ToARGB_when_callU32ToARGB);
    RUN_TEST(_should_convertARGBToU32_when_callARGBToU32);
    RUN_TEST(_should_convertFloatToAHSV_when_callFloatToAHSV);
    RUN_TEST(_should_convertAHSVToU32_when_callAHSVToU32);

    return UNITY_END();
}
