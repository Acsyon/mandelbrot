#include "unity.h"

#include <stdlib.h>

#include <visuals/palette.h>

static void
_should_returnGrayColor_when_callPaletteGray(void)
{
    /* Arrange */
    const float pos = 0.5f;

    /* Act */
    const uint32_t color = Palette_gray(pos, NULL);

    /* Assert */
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF;
    const uint8_t b = color & 0xFF;
    TEST_ASSERT_EQUAL_UINT8(r, g);
    TEST_ASSERT_EQUAL_UINT8(g, b);
}

static void
_should_returnRedColor_when_callPaletteRed(void)
{
    /* Arrange */
    const float pos = 0.5f;

    /* Act */
    uint32_t color = Palette_red(pos, NULL);

    /* Assert */
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF;
    const uint8_t b = color & 0xFF;
    TEST_ASSERT_GREATER_THAN_UINT8(0, r);
    TEST_ASSERT_EQUAL_UINT8(0, g);
    TEST_ASSERT_EQUAL_UINT8(0, b);
}

static void
_should_returnGreenColor_when_callPaletteGreen(void)
{
    /* Arrange */
    const float pos = 0.5f;

    /* Act */
    const uint32_t color = Palette_green(pos, NULL);

    /* Assert */
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF;
    const uint8_t b = color & 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, r);
    TEST_ASSERT_GREATER_THAN_UINT8(0, g);
    TEST_ASSERT_EQUAL_UINT8(0, b);
}

static void
_should_returnBlueColor_when_callPaletteBlue(void)
{
    /* Arrange */
    const float pos = 0.5f;

    /* Act */
    const uint32_t color = Palette_blue(pos, NULL);

    /* Assert */
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF;
    const uint8_t b = color & 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0, r);
    TEST_ASSERT_EQUAL_UINT8(0, g);
    TEST_ASSERT_GREATER_THAN_UINT8(0, b);
}

static void
_should_returnValidColor_when_callPaletteExpHsv(void)
{
    /* Arrange */
    const float pos = 0.5f;

    /* Act */
    const uint32_t color = Palette_exp_hsv(pos, NULL);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL(0, color);
}

static void
_should_returnValidColor_when_callPaletteUltraFractal(void)
{
    /* Arrange */
    const float pos = 0.5f;

    /* Act */
    const uint32_t color = Palette_ultra_fractal(pos, NULL);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL(0, color);
}

static void
_should_cyclePalettes_when_callPaletteCyclerCycle(void)
{
    /* Arrange */
    PaletteCycler *const cycler
      = PaletteCycler_create(PALETTE_FUNCTIONS, PALETTE_FUNCTION_COUNT, 0);

    /* Act */
    Palette_fnc *const first_palette = PaletteCycler_cycle_palette(cycler);
    Palette_fnc *const second_palette = PaletteCycler_cycle_palette(cycler);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL(first_palette, second_palette);

    /* Cleanup */
    PaletteCycler_free(cycler);
}

static void
_should_advanceTripMode_when_callTripModeGeneratorAdvance(void)
{
    /* Arrange */
    const float pos = 0.5f;
    TripModeGenerator *const tripgen
      = TripModeGenerator_create(TRIP_MODE_PHASE);
    Palette_fnc *const palette = TripModeGenerator_get_palette(tripgen);
    const void *const params = TripModeGenerator_get_params(tripgen);

    /* Act */
    const uint32_t first_color = palette(pos, params);
    TripModeGenerator_advance(tripgen);
    const uint32_t second_color = palette(pos, params);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL(first_color, second_color);

    /* Cleanup */
    TripModeGenerator_free(tripgen);
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

    RUN_TEST(_should_returnGrayColor_when_callPaletteGray);
    RUN_TEST(_should_returnRedColor_when_callPaletteRed);
    RUN_TEST(_should_returnGreenColor_when_callPaletteGreen);
    RUN_TEST(_should_returnBlueColor_when_callPaletteBlue);
    RUN_TEST(_should_returnValidColor_when_callPaletteExpHsv);
    RUN_TEST(_should_returnValidColor_when_callPaletteUltraFractal);
    RUN_TEST(_should_cyclePalettes_when_callPaletteCyclerCycle);
    RUN_TEST(_should_advanceTripMode_when_callTripModeGeneratorAdvance);

    return UNITY_END();
}
