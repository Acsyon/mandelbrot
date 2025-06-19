#include <visuals/palette.h>

#include <cutil/io/log.h>
#include <cutil/std/math.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#include <util/num.h>
#include <visuals/color.h>

#define RGB_FROM_RAW(R, G, B)                                                  \
    {.r = UINT8_C((R)), .g = UINT8_C((G)), .b = UINT8_C((B))}

Palette_fnc *const PALETTE_FUNCTIONS[] = {
  &Palette_gray, &Palette_red,     &Palette_green,
  &Palette_blue, &Palette_exp_hsv, &Palette_ultra_fractal,
};

const size_t PALETTE_FUNCTION_COUNT
  = (sizeof PALETTE_FUNCTIONS) / (sizeof *PALETTE_FUNCTIONS);

uint32_t
Palette_gray(float pos, const void *params)
{
    CUTIL_UNUSED(params);
    const uint8_t val = powf(pos, 1.0F / 1.5F) * UINT8_MAX;
    uint32_t res = 0;
    res += val * UINT32_C(0x00010000);
    res += val * UINT32_C(0x00000100);
    res += val * UINT32_C(0x00000001);
    return res;
}

uint32_t
Palette_red(float pos, const void *params)
{
    CUTIL_UNUSED(params);
    const uint8_t val = powf(pos, 1.0F / 1.5F) * UINT8_MAX;
    return val * UINT32_C(0x00010000);
}

uint32_t
Palette_green(float pos, const void *params)
{
    CUTIL_UNUSED(params);
    const uint8_t val = powf(pos, 1.0F / 1.5F) * UINT8_MAX;
    return val * UINT32_C(0x00000100);
}

uint32_t
Palette_blue(float pos, const void *params)
{
    CUTIL_UNUSED(params);
    const uint8_t val = powf(pos, 1.0F / 1.5F) * UINT8_MAX;
    return val * UINT32_C(0x00000001);
}

uint32_t
Palette_exp_hsv_offset(float pos, const void *params)
{
    const struct AHSV *offset = params;
    if (pos == PALETTE_INVALID_POS) {
        return 0x000D152D;
    }
    const struct AHSV hsv = {
      .h = (((uint16_t) powf(pos * 360, 1.5F) + offset->h) % HSV_H_MAX),
      .s = 1.0F,
      .v = fmodf(powf(pos, 0.5F) + offset->v, HSV_V_MAX),
    };
    return AHSV_to_u32(hsv);
}

uint32_t
Palette_exp_hsv(float pos, const void *params)
{
    CUTIL_UNUSED(params);
    static const struct AHSV offset = {0};
    return Palette_exp_hsv_offset(pos, &offset);
}

static inline uint32_t
_lerp_rgb_to_u32(
  float pos1, float pos2, struct ARGB rgb1, struct ARGB rgb2, float pos
)
{
    const float ratio = (pos2 - pos) / (pos2 - pos1);
    struct ARGB rgb = {0};
    rgb.r = rgb1.r * ratio + (1.0F - ratio) * rgb2.r;
    rgb.g = rgb1.g * ratio + (1.0F - ratio) * rgb2.g;
    rgb.b = rgb1.b * ratio + (1.0F - ratio) * rgb2.b;
    return ARGB_to_u32(rgb);
}

uint32_t
Palette_ultra_fractal(float pos, const void *params)
{
    CUTIL_UNUSED(params);
    if (pos == 0.0F) {
        return 0;
    }

    static const float positions[] = {
      0.0000F, 0.1600F, 0.4200F, 0.6425F, 0.8575F,
    };
    static const struct ARGB colors[] = {
      RGB_FROM_RAW(000, 007, 100), RGB_FROM_RAW(032, 107, 203),
      RGB_FROM_RAW(237, 255, 255), RGB_FROM_RAW(255, 170, 000),
      RGB_FROM_RAW(000, 002, 000),
    };

    int idx = -1;
    for (size_t i = 0; i < (sizeof positions / sizeof positions[0]); ++i) {
        if (pos <= positions[i + 1]) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        return 0;
    }

    return _lerp_rgb_to_u32(
      positions[idx], positions[idx + 1], colors[idx], colors[idx + 1], pos
    );
}

struct _paletteCycler {
    size_t num;
    Palette_fnc **fncs;
    size_t idx;
};

PaletteCycler *
PaletteCycler_create(Palette_fnc *const *fncs, size_t num, size_t idx)
{
    PaletteCycler *const cycler = malloc(sizeof *cycler);

    cycler->num = num;
    cycler->fncs = malloc(cycler->num * sizeof *cycler->fncs);
    memcpy(cycler->fncs, fncs, num * sizeof *fncs);
    if (idx >= num) {
        idx = num - 1;
        cutil_log_warn(
          "Default palette index too large, truncating to %zu", idx
        );
    }
    cycler->idx = idx;

    return cycler;
}

void
PaletteCycler_free(PaletteCycler *cycler)
{
    CUTIL_RETURN_IF_NULL(cycler);

    free(cycler->fncs);

    free(cycler);
}

Palette_fnc *
PaletteCycler_cycle_palette(PaletteCycler *cycler)
{
    Palette_fnc *const fnc = cycler->fncs[cycler->idx];
    cycler->idx = (cycler->idx + 1) % cycler->num;
    return fnc;
}

static uint32_t
_palette_trip_mode_lerp(float pos, const void *params)
{
    const float idx = *(const float *) params;
    const size_t idx_lo = (size_t) floor(idx);
    const size_t idx_hi = (size_t) ceil(idx);
    const float color0 = PALETTE_FUNCTIONS[idx_lo](pos, params);
    const float color1 = PALETTE_FUNCTIONS[idx_hi](pos, params);
    return lerpf_single(idx_lo * 1.0F, idx_hi * 1.0F, color0, color1, idx);
}

#define TRIP_PHASE_HUE_INCREASE 5
#define TRIP_PHASE_VALUE_INCREASE 0.025F

static void
_tripModeGenerator_advance_phase(void *params)
{
    struct AHSV *const p_offset = params;
    p_offset->h = (p_offset->h + TRIP_PHASE_HUE_INCREASE) % HSV_H_MAX;
    p_offset->v = fmodf(p_offset->v + TRIP_PHASE_VALUE_INCREASE, HSV_V_MAX);
}

#define MINIMUM_PALETTE_CYCLE_INDEX_TRIP_LERP 0.0F
#define MAXIMUM_PALETTE_CYCLE_INDEX_TRIP_LERP 3.0F

static void
_tripModeGenerator_advance_lerp(void *params)
{
    static const float idx_min = MINIMUM_PALETTE_CYCLE_INDEX_TRIP_LERP;
    static const float idx_max = MAXIMUM_PALETTE_CYCLE_INDEX_TRIP_LERP;
    float *const p_idx = params;
    *p_idx
      = idx_min + fmodf(*p_idx - idx_min + 0.01F, 1.0F * (idx_max - idx_min));
}

struct _tripModeGenerator {
    void (*advance)(void *params);
    Palette_fnc *palette;
    void *params;
};

static void
_tripModeGenerator_init_phase(TripModeGenerator *tripgen)
{
    struct AHSV *const p_offset = malloc(sizeof *p_offset);
    *p_offset = (struct AHSV) {0};
    tripgen->advance = &_tripModeGenerator_advance_phase;
    tripgen->palette = &Palette_exp_hsv_offset;
    tripgen->params = p_offset;
}

static void
_tripModeGenerator_init_lerp(TripModeGenerator *tripgen)
{
    float *const p_idx = malloc(sizeof *p_idx);
    *p_idx = 0.0F;
    tripgen->advance = &_tripModeGenerator_advance_lerp;
    tripgen->palette = &_palette_trip_mode_lerp;
    tripgen->params = p_idx;
}

TripModeGenerator *
TripModeGenerator_create(enum TripModeType type)
{
    TripModeGenerator *const tripgen = malloc(sizeof *tripgen);

    switch (type) {
    case TRIP_MODE_PHASE:
        _tripModeGenerator_init_phase(tripgen);
        break;
    case TRIP_MODE_LERP:
        _tripModeGenerator_init_lerp(tripgen);
        break;
    default:
        cutil_log_warn(
          "Wrong trip mode type, using %i instead", TRIP_MODE_PHASE
        );
        _tripModeGenerator_init_phase(tripgen);
        break;
    }

    return tripgen;
}

void
TripModeGenerator_free(TripModeGenerator *tripgen)
{
    CUTIL_RETURN_IF_NULL(tripgen);

    free(tripgen->params);

    free(tripgen);
}

void
TripModeGenerator_advance(TripModeGenerator *tripgen)
{
    tripgen->advance(tripgen->params);
}

Palette_fnc *
TripModeGenerator_get_palette(const TripModeGenerator *tripgen)
{
    return tripgen->palette;
}

const void *
TripModeGenerator_get_params(const TripModeGenerator *tripgen)
{
    return tripgen->params;
}
