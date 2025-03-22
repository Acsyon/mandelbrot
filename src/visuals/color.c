#include "color.h"

#include <math.h>

extern inline struct ARGB
float_to_ARGB(float r, float g, float b);

extern inline struct ARGB
u32_to_ARGB(uint32_t u);

extern inline uint32_t
ARGB_to_u32(struct ARGB argb);

extern inline struct AHSV
float_to_AHSV(float h, float s, float v);

struct ARGB
AHSV_to_ARGB(struct AHSV hsv)
{
    float h = 1.0F * hsv.h;
    const float s = hsv.s;
    const float v = hsv.v;

    /* Achromatic (grey) */
    if (s == 0.0F) {
        return float_to_ARGB(v, v, v);
    }

    h /= 60.0F; /* Sector 0 to 5 or 6 (= full circle) */
    const int i = (int) h;

    const float f = h - i; /* Fractional part of h */
    const float p = v * (1.0F - s);
    const float q = v * (1.0F - s * f);
    const float t = v * (1.0F - s * (1.0F - f));

    switch (i) {
    default:
    case 0:
        return float_to_ARGB(v, t, p);
    case 1:
        return float_to_ARGB(q, v, p);
    case 2:
        return float_to_ARGB(p, v, t);
    case 3:
        return float_to_ARGB(p, q, v);
    case 4:
        return float_to_ARGB(t, p, v);
    case 5:
        return float_to_ARGB(v, p, q);
    }
}

struct AHSV
ARGB_to_AHSV(struct ARGB rgb)
{
    const float r = 1.0F * rgb.r / UINT8_MAX;
    const float g = 1.0F * rgb.g / UINT8_MAX;
    const float b = 1.0F * rgb.b / UINT8_MAX;

    const float max = fmaxf(r, fmaxf(g, b));
    const float min = fminf(r, fminf(g, b));
    const float delta = max - min;

    float h = 0.0F;
    float s = 0.0F;
    const float v = max; /* Value */

    /* r = g = b = 0 (black) */
    if (max == 0.0F) {
        return float_to_AHSV(h, s, v);
    }

    s = delta / max; /* Saturation */

    /* Achromatic (grey) */
    if (delta == 0.0F) {
        return float_to_AHSV(h, s, v);
    }

    if (r == max) {
        h = 0.0F + (g - b) / delta; /* between yellow and magenta */
    } else if (g == max) {
        h = 2.0F + (b - r) / delta; /* between cyan and yellow */
    } else {
        h = 4.0F + (r - g) / delta; /* between magenta and cyan */
    }

    h *= 60.0F; /* Convert to degrees */

    if (h < 0.0F) {
        h += 360.0F;
    }

    return float_to_AHSV(h / 360.0F, s, v);
}

extern inline uint32_t
AHSV_to_u32(struct AHSV hsv);
