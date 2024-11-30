#include <unity/unity.h>

#include "../src/color.c"

#define ASSERT_EQUALS_RGB(LHS, RHS)                                            \
    TEST_ASSERT_EQUAL_UINT32(_rgb2u32(LHS), _rgb2u32(RHS))
#define ASSERT_EQUALS_HSV(LHS, RHS)                                            \
    TEST_ASSERT_EQUAL_UINT32(_hsv2u32(LHS), _hsv2u32(RHS))

static void
_test_rgb2hsv(void)
{
    /* Arrange */
    const _argb black_rgb = _float2rgb(0.0f, 0.0f, 0.0f);
    const _argb white_rgb = _float2rgb(1.0f, 1.0f, 1.0f);
    const _argb gray_rgb = _float2rgb(0.5f, 0.5f, 0.5f);

    const _argb red_rgb = _float2rgb(1.0f, 0.0f, 0.0f);
    const _argb lime_rgb = _float2rgb(0.0f, 1.0f, 0.0f);
    const _argb blue_rgb = _float2rgb(0.0f, 0.0f, 1.0f);

    const _argb yellow_rgb = _float2rgb(1.0f, 1.0f, 0.0f);
    const _argb cyan_rgb = _float2rgb(0.0f, 1.0f, 1.0f);
    const _argb magenta_rgb = _float2rgb(1.0f, 0.0f, 1.0f);

    const _argb maroon_rgb = _float2rgb(0.5f, 0.0f, 0.0f);
    const _argb darkgreen_rgb = _float2rgb(0.0f, 0.5f, 0.0f);
    const _argb navy_rgb = _float2rgb(0.0f, 0.0f, 0.5f);

    const _argb olive_rgb = _float2rgb(0.5f, 0.5f, 0.0f);
    const _argb purple_rgb = _float2rgb(0.5f, 0.0f, 0.5f);
    const _argb teal_rgb = _float2rgb(0.0f, 0.5f, 0.5f);

    const _ahsv black_hsv = _float2hsv(0.0f, 0.0f, 0.0f);
    const _ahsv white_hsv = _float2hsv(0.0f, 0.0f, 1.0f);
    const _ahsv gray_hsv = _float2hsv(0.0f, 0.0f, 0.5f);

    const _ahsv red_hsv = _float2hsv(000.0f / 360, 1.0f, 1.0f);
    const _ahsv lime_hsv = _float2hsv(120.0f / 360, 1.0f, 1.0f);
    const _ahsv blue_hsv = _float2hsv(240.0f / 360, 1.0f, 1.0f);

    const _ahsv yellow_hsv = _float2hsv(060.0f / 360, 1.0f, 1.0f);
    const _ahsv cyan_hsv = _float2hsv(180.0f / 360, 1.0f, 1.0f);
    const _ahsv magenta_hsv = _float2hsv(300.0f / 360, 1.0f, 1.0f);

    const _ahsv maroon_hsv = _float2hsv(000.0f / 360, 1.0f, 0.5f);
    const _ahsv darkgreen_hsv = _float2hsv(120.0f / 360, 1.0f, 0.5f);
    const _ahsv navy_hsv = _float2hsv(240.0f / 360, 1.0f, 0.5f);

    const _ahsv olive_hsv = _float2hsv(060.0f / 360, 1.0f, 0.5f);
    const _ahsv purple_hsv = _float2hsv(300.0f / 360, 1.0f, 0.5f);
    const _ahsv teal_hsv = _float2hsv(180.0f / 360, 1.0f, 0.5f);

    /* Act */
    const _ahsv black_hsv_conv = _rgb2hsv(black_rgb);
    const _ahsv white_hsv_conv = _rgb2hsv(white_rgb);
    const _ahsv gray_hsv_conv = _rgb2hsv(gray_rgb);

    const _ahsv red_hsv_conv = _rgb2hsv(red_rgb);
    const _ahsv lime_hsv_conv = _rgb2hsv(lime_rgb);
    const _ahsv blue_hsv_conv = _rgb2hsv(blue_rgb);

    const _ahsv yellow_hsv_conv = _rgb2hsv(yellow_rgb);
    const _ahsv cyan_hsv_conv = _rgb2hsv(cyan_rgb);
    const _ahsv magenta_hsv_conv = _rgb2hsv(magenta_rgb);

    const _ahsv maroon_hsv_conv = _rgb2hsv(maroon_rgb);
    const _ahsv darkgreen_hsv_conv = _rgb2hsv(darkgreen_rgb);
    const _ahsv navy_hsv_conv = _rgb2hsv(navy_rgb);

    const _ahsv olive_hsv_conv = _rgb2hsv(olive_rgb);
    const _ahsv purple_hsv_conv = _rgb2hsv(purple_rgb);
    const _ahsv teal_hsv_conv = _rgb2hsv(teal_rgb);

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
_test_hsv2rgb(void)
{
    /* Arrange */
    const _argb black_rgb = _float2rgb(0.0f, 0.0f, 0.0f);
    const _argb white_rgb = _float2rgb(1.0f, 1.0f, 1.0f);
    const _argb gray_rgb = _float2rgb(0.5f, 0.5f, 0.5f);

    const _argb red_rgb = _float2rgb(1.0f, 0.0f, 0.0f);
    const _argb lime_rgb = _float2rgb(0.0f, 1.0f, 0.0f);
    const _argb blue_rgb = _float2rgb(0.0f, 0.0f, 1.0f);

    const _argb yellow_rgb = _float2rgb(1.0f, 1.0f, 0.0f);
    const _argb cyan_rgb = _float2rgb(0.0f, 1.0f, 1.0f);
    const _argb magenta_rgb = _float2rgb(1.0f, 0.0f, 1.0f);

    const _argb maroon_rgb = _float2rgb(0.5f, 0.0f, 0.0f);
    const _argb darkgreen_rgb = _float2rgb(0.0f, 0.5f, 0.0f);
    const _argb navy_rgb = _float2rgb(0.0f, 0.0f, 0.5f);

    const _argb olive_rgb = _float2rgb(0.5f, 0.5f, 0.0f);
    const _argb purple_rgb = _float2rgb(0.5f, 0.0f, 0.5f);
    const _argb teal_rgb = _float2rgb(0.0f, 0.5f, 0.5f);

    const _ahsv black_hsv = _float2hsv(0.0f, 0.0f, 0.0f);
    const _ahsv white_hsv = _float2hsv(0.0f, 0.0f, 1.0f);
    const _ahsv gray_hsv = _float2hsv(0.0f, 0.0f, 0.5f);

    const _ahsv red_hsv = _float2hsv(000.0f / 360, 1.0f, 1.0f);
    const _ahsv lime_hsv = _float2hsv(120.0f / 360, 1.0f, 1.0f);
    const _ahsv blue_hsv = _float2hsv(240.0f / 360, 1.0f, 1.0f);

    const _ahsv yellow_hsv = _float2hsv(060.0f / 360, 1.0f, 1.0f);
    const _ahsv cyan_hsv = _float2hsv(180.0f / 360, 1.0f, 1.0f);
    const _ahsv magenta_hsv = _float2hsv(300.0f / 360, 1.0f, 1.0f);

    const _ahsv maroon_hsv = _float2hsv(000.0f / 360, 1.0f, 0.5f);
    const _ahsv darkgreen_hsv = _float2hsv(120.0f / 360, 1.0f, 0.5f);
    const _ahsv navy_hsv = _float2hsv(240.0f / 360, 1.0f, 0.5f);

    const _ahsv olive_hsv = _float2hsv(060.0f / 360, 1.0f, 0.5f);
    const _ahsv purple_hsv = _float2hsv(300.0f / 360, 1.0f, 0.5f);
    const _ahsv teal_hsv = _float2hsv(180.0f / 360, 1.0f, 0.5f);

    /* Act */
    const _argb black_rgb_conv = _hsv2rgb(black_hsv);
    const _argb white_rgb_conv = _hsv2rgb(white_hsv);
    const _argb gray_rgb_conv = _hsv2rgb(gray_hsv);

    const _argb red_rgb_conv = _hsv2rgb(red_hsv);
    const _argb lime_rgb_conv = _hsv2rgb(lime_hsv);
    const _argb blue_rgb_conv = _hsv2rgb(blue_hsv);

    const _argb yellow_rgb_conv = _hsv2rgb(yellow_hsv);
    const _argb cyan_rgb_conv = _hsv2rgb(cyan_hsv);
    const _argb magenta_rgb_conv = _hsv2rgb(magenta_hsv);

    const _argb maroon_rgb_conv = _hsv2rgb(maroon_hsv);
    const _argb darkgreen_rgb_conv = _hsv2rgb(darkgreen_hsv);
    const _argb navy_rgb_conv = _hsv2rgb(navy_hsv);

    const _argb olive_rgb_conv = _hsv2rgb(olive_hsv);
    const _argb purple_rgb_conv = _hsv2rgb(purple_hsv);
    const _argb teal_rgb_conv = _hsv2rgb(teal_hsv);

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
main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(_test_rgb2hsv);
    RUN_TEST(_test_hsv2rgb);
    return UNITY_END();
}
