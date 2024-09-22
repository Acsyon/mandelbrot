#include "color.h"

#include <math.h>
#include <stddef.h>

typedef struct {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} _argb;

#define RGB_FROM_RAW(R, G, B)                                                  \
    {.r = UINT8_C((R)), .g = UINT8_C((G)), .b = UINT8_C((B))}

static inline _argb
_float2rgb(float r, float g, float b)
{
    _argb rgb = {0};
    rgb.r = r * UINT8_MAX;
    rgb.g = g * UINT8_MAX;
    rgb.b = b * UINT8_MAX;
    return rgb;
}

static inline _argb
_u322rgb(uint32_t u)
{
    _argb rgb = {0};
    rgb.a = (u /= UINT32_C(0x01000000));
    rgb.r = (u /= UINT32_C(0x00010000));
    rgb.g = (u /= UINT32_C(0x00000100));
    rgb.b = (u /= UINT32_C(0x00000001));
    return rgb;
}

static inline uint32_t
_rgb2u32(_argb argb)
{
    uint32_t res = 0;
    res += argb.a * UINT32_C(0x01000000);
    res += argb.r * UINT32_C(0x00010000);
    res += argb.g * UINT32_C(0x00000100);
    res += argb.b * UINT32_C(0x00000001);
    return res;
}

#define HSV_H_MAX 360

typedef struct {
    float a;
    uint16_t h;
    float s;
    float v;
} _ahsv;

static inline _ahsv
_float2hsv(float h, float s, float v)
{
    _ahsv hsv = {0};
    hsv.h = h * HSV_H_MAX;
    hsv.s = s;
    hsv.v = v;
    return hsv;
}

static _argb
_hsv2rgb(_ahsv hsv)
{
    float h = 1.0f * hsv.h;
    const float s = hsv.s;
    const float v = hsv.v;

    /* Achromatic (grey) */
    if (s == 0) {
        return _float2rgb(v, v, v);
    }

    h /= 60.0f; /* Sector 0 to 5 or 6 (= full circle) */
    const int i = (int) h;

    const float f = h - i; /* Fractional part of h */
    const float p = v * (1.0f - s);
    const float q = v * (1.0f - s * f);
    const float t = v * (1.0f - s * (1.0f - f));

    switch (i) {
    default:
    case 0:
        return _float2rgb(v, t, p);
    case 1:
        return _float2rgb(q, v, p);
    case 2:
        return _float2rgb(p, v, t);
    case 3:
        return _float2rgb(p, q, v);
    case 4:
        return _float2rgb(t, p, v);
    case 5:
        return _float2rgb(v, p, q);
    }
}

static _ahsv
_rgb2hsv(_argb rgb)
{
    const float r = 1.0f * rgb.r / UINT8_MAX;
    const float g = 1.0f * rgb.g / UINT8_MAX;
    const float b = 1.0f * rgb.b / UINT8_MAX;

    const float max = fmaxf(r, fmaxf(g, b));
    const float min = fminf(r, fminf(g, b));
    const float delta = max - min;

    float h = 0;
    float s = 0.0f;
    const float v = max; /* Value */

    /* r = g = b = 0 (black) */
    if (max == 0.0) {
        return _float2hsv(h, s, v);
    }

    s = delta / max; /* Saturation */

    /* Achromatic (grey) */
    if (delta == 0.0f) {
        return _float2hsv(h, s, v);
    }

    if (r == max) {
        h = 0.0f + (g - b) / delta; /* between yellow and magenta */
    } else if (g == max) {
        h = 2.0f + (b - r) / delta; /* between cyan and yellow */
    } else {
        h = 4.0f + (r - g) / delta; /* between magenta and cyan */
    }

    h *= 60.0f; /* Convert to degrees */

    if (h < 0) {
        h += 360.0f;
    }

    return _float2hsv(h / 360.0f, s, v);
}

static inline uint32_t
_hsv2u32(_ahsv hsv)
{
    const _argb rgb = _hsv2rgb(hsv);
    return _rgb2u32(rgb);
}

uint32_t
Palette_gray(float pos)
{
    const uint8_t val = powf(pos, 1.0 / 1.5) * UINT8_MAX;
    uint32_t res = 0;
    res += val * UINT32_C(0x00010000);
    res += val * UINT32_C(0x00000100);
    res += val * UINT32_C(0x00000001);
    return res;
}

uint32_t
Palette_red(float pos)
{
    const uint8_t val = powf(pos, 1.0 / 1.5) * UINT8_MAX;
    return val * UINT32_C(0x00010000);
}

uint32_t
Palette_green(float pos)
{
    const uint8_t val = powf(pos, 1.0 / 1.5) * UINT8_MAX;
    return val * UINT32_C(0x00000100);
}

uint32_t
Palette_blue(float pos)
{
    const uint8_t val = powf(pos, 1.0 / 1.5) * UINT8_MAX;
    return val * UINT32_C(0x00000001);
}

uint32_t
Palette_exp_hsv(float pos)
{
    (void) &_rgb2hsv;
    const _ahsv hsv = {
      .h = (((uint16_t) powf(pos * 360, 1.5)) % 360),
      .s = 1.0,
      .v = pos,
    };
    return _hsv2u32(hsv);
}

static inline uint32_t
_lerp_rgb_to_u32(float pos1, float pos2, _argb rgb1, _argb rgb2, float pos)
{
    const float ratio = (pos2 - pos) / (pos2 - pos1);
    _argb rgb = {0};
    rgb.r = rgb1.r * ratio + (1.0f - ratio) * rgb2.r;
    rgb.g = rgb1.g * ratio + (1.0f - ratio) * rgb2.g;
    rgb.b = rgb1.b * ratio + (1.0f - ratio) * rgb2.b;
    return _rgb2u32(rgb);
}

uint32_t
Palette_ultra_fractal(float pos)
{
    if (pos == 0.0f) {
        return 0;
    }

    static const float positions[] = {
      0.0000f, 0.1600f, 0.4200f, 0.6425f, 0.8575f,
    };
    static const _argb colors[] = {
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
