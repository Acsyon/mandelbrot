#include "unity.h"

#include "color.h"

#define ASSERT_EQUALS_RGB(LHS, RHS)                                            \
    TEST_ASSERT_EQUAL_UINT32(ARGB_to_u32(LHS), ARGB_to_u32(RHS))
#define ASSERT_EQUALS_HSV(LHS, RHS)                                            \
    TEST_ASSERT_EQUAL_UINT32(AHSV_to_u32(LHS), AHSV_to_u32(RHS))

static void
_test_ARGB_to_AHSV(void)
{
    /* Arrange */
    const struct ARGB black_rgb = float_to_ARGB(0.0f, 0.0f, 0.0f);
    const struct ARGB white_rgb = float_to_ARGB(1.0f, 1.0f, 1.0f);
    const struct ARGB gray_rgb = float_to_ARGB(0.5f, 0.5f, 0.5f);

    const struct ARGB red_rgb = float_to_ARGB(1.0f, 0.0f, 0.0f);
    const struct ARGB lime_rgb = float_to_ARGB(0.0f, 1.0f, 0.0f);
    const struct ARGB blue_rgb = float_to_ARGB(0.0f, 0.0f, 1.0f);

    const struct ARGB yellow_rgb = float_to_ARGB(1.0f, 1.0f, 0.0f);
    const struct ARGB cyan_rgb = float_to_ARGB(0.0f, 1.0f, 1.0f);
    const struct ARGB magenta_rgb = float_to_ARGB(1.0f, 0.0f, 1.0f);

    const struct ARGB maroon_rgb = float_to_ARGB(0.5f, 0.0f, 0.0f);
    const struct ARGB darkgreen_rgb = float_to_ARGB(0.0f, 0.5f, 0.0f);
    const struct ARGB navy_rgb = float_to_ARGB(0.0f, 0.0f, 0.5f);

    const struct ARGB olive_rgb = float_to_ARGB(0.5f, 0.5f, 0.0f);
    const struct ARGB purple_rgb = float_to_ARGB(0.5f, 0.0f, 0.5f);
    const struct ARGB teal_rgb = float_to_ARGB(0.0f, 0.5f, 0.5f);

    const struct AHSV black_hsv = float_to_AHSV(0.0f, 0.0f, 0.0f);
    const struct AHSV white_hsv = float_to_AHSV(0.0f, 0.0f, 1.0f);
    const struct AHSV gray_hsv = float_to_AHSV(0.0f, 0.0f, 0.5f);

    const struct AHSV red_hsv = float_to_AHSV(000.0f / 360, 1.0f, 1.0f);
    const struct AHSV lime_hsv = float_to_AHSV(120.0f / 360, 1.0f, 1.0f);
    const struct AHSV blue_hsv = float_to_AHSV(240.0f / 360, 1.0f, 1.0f);

    const struct AHSV yellow_hsv = float_to_AHSV(060.0f / 360, 1.0f, 1.0f);
    const struct AHSV cyan_hsv = float_to_AHSV(180.0f / 360, 1.0f, 1.0f);
    const struct AHSV magenta_hsv = float_to_AHSV(300.0f / 360, 1.0f, 1.0f);

    const struct AHSV maroon_hsv = float_to_AHSV(000.0f / 360, 1.0f, 0.5f);
    const struct AHSV darkgreen_hsv = float_to_AHSV(120.0f / 360, 1.0f, 0.5f);
    const struct AHSV navy_hsv = float_to_AHSV(240.0f / 360, 1.0f, 0.5f);

    const struct AHSV olive_hsv = float_to_AHSV(060.0f / 360, 1.0f, 0.5f);
    const struct AHSV purple_hsv = float_to_AHSV(300.0f / 360, 1.0f, 0.5f);
    const struct AHSV teal_hsv = float_to_AHSV(180.0f / 360, 1.0f, 0.5f);

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
_test_AHSV_to_ARGB(void)
{
    /* Arrange */
    const struct ARGB black_rgb = float_to_ARGB(0.0f, 0.0f, 0.0f);
    const struct ARGB white_rgb = float_to_ARGB(1.0f, 1.0f, 1.0f);
    const struct ARGB gray_rgb = float_to_ARGB(0.5f, 0.5f, 0.5f);

    const struct ARGB red_rgb = float_to_ARGB(1.0f, 0.0f, 0.0f);
    const struct ARGB lime_rgb = float_to_ARGB(0.0f, 1.0f, 0.0f);
    const struct ARGB blue_rgb = float_to_ARGB(0.0f, 0.0f, 1.0f);

    const struct ARGB yellow_rgb = float_to_ARGB(1.0f, 1.0f, 0.0f);
    const struct ARGB cyan_rgb = float_to_ARGB(0.0f, 1.0f, 1.0f);
    const struct ARGB magenta_rgb = float_to_ARGB(1.0f, 0.0f, 1.0f);

    const struct ARGB maroon_rgb = float_to_ARGB(0.5f, 0.0f, 0.0f);
    const struct ARGB darkgreen_rgb = float_to_ARGB(0.0f, 0.5f, 0.0f);
    const struct ARGB navy_rgb = float_to_ARGB(0.0f, 0.0f, 0.5f);

    const struct ARGB olive_rgb = float_to_ARGB(0.5f, 0.5f, 0.0f);
    const struct ARGB purple_rgb = float_to_ARGB(0.5f, 0.0f, 0.5f);
    const struct ARGB teal_rgb = float_to_ARGB(0.0f, 0.5f, 0.5f);

    const struct AHSV black_hsv = float_to_AHSV(0.0f, 0.0f, 0.0f);
    const struct AHSV white_hsv = float_to_AHSV(0.0f, 0.0f, 1.0f);
    const struct AHSV gray_hsv = float_to_AHSV(0.0f, 0.0f, 0.5f);

    const struct AHSV red_hsv = float_to_AHSV(000.0f / 360, 1.0f, 1.0f);
    const struct AHSV lime_hsv = float_to_AHSV(120.0f / 360, 1.0f, 1.0f);
    const struct AHSV blue_hsv = float_to_AHSV(240.0f / 360, 1.0f, 1.0f);

    const struct AHSV yellow_hsv = float_to_AHSV(060.0f / 360, 1.0f, 1.0f);
    const struct AHSV cyan_hsv = float_to_AHSV(180.0f / 360, 1.0f, 1.0f);
    const struct AHSV magenta_hsv = float_to_AHSV(300.0f / 360, 1.0f, 1.0f);

    const struct AHSV maroon_hsv = float_to_AHSV(000.0f / 360, 1.0f, 0.5f);
    const struct AHSV darkgreen_hsv = float_to_AHSV(120.0f / 360, 1.0f, 0.5f);
    const struct AHSV navy_hsv = float_to_AHSV(240.0f / 360, 1.0f, 0.5f);

    const struct AHSV olive_hsv = float_to_AHSV(060.0f / 360, 1.0f, 0.5f);
    const struct AHSV purple_hsv = float_to_AHSV(300.0f / 360, 1.0f, 0.5f);
    const struct AHSV teal_hsv = float_to_AHSV(180.0f / 360, 1.0f, 0.5f);

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
    RUN_TEST(_test_ARGB_to_AHSV);
    RUN_TEST(_test_AHSV_to_ARGB);
    return UNITY_END();
}
