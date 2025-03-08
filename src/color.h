/* color.h
 *
 * Header for colour space shenanigans
 *
 */

#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <stdint.h>

#define HSV_A_MAX 1.0f
#define HSV_H_MAX UINT16_C(360)
#define HSV_S_MAX 1.0f
#define HSV_V_MAX 1.0f

/**
 * Auxiliary struct for the (A)RGB colour space
 */
struct ARGB {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/**
 * Converts a triplet of floats between 0 and 1, corresponding to the percentage
 * of the maximum value of 255, to an ARGB.
 *
 * @param[in] r value for red
 * @param[in] g value for green
 * @param[in] b value for blue
 *
 * @return resulting ARGB
 */
inline struct ARGB
float_to_ARGB(float r, float g, float b)
{
    struct ARGB rgb = {0};
    rgb.r = r * UINT8_MAX;
    rgb.g = g * UINT8_MAX;
    rgb.b = b * UINT8_MAX;
    return rgb;
}

/**
 * Converts a single uint32_t into an ARGB. Each byte corresponds to a value of
 * ARGB starting at the most-significant byte.
 *
 * @param[in] u uint32_t to convert into an ARGB
 *
 * @return resulting ARGB
 */
inline struct ARGB
u32_to_ARGB(uint32_t u)
{
    uint32_t tmp = u;
    struct ARGB rgb = {0};
    rgb.a = (tmp / UINT32_C(0x01000000));
    tmp = tmp % 0x01000000;
    rgb.r = (tmp / UINT32_C(0x00010000));
    tmp = tmp % 0x00010000;
    rgb.g = (tmp / UINT32_C(0x00000100));
    tmp = tmp % 0x00000100;
    rgb.b = (tmp / UINT32_C(0x00000001));
    return rgb;
}

/**
 * Converts an ARGB into a single uint32_t. Each byte corresponds to a value of
 * ARGB starting at the most-significant byte.
 *
 * @param[in] rgb ARGB to convert into an uint32_t
 *
 * @return resulting uint32_t
 */
inline uint32_t
ARGB_to_u32(struct ARGB rgb)
{
    uint32_t res = 0;
    res += rgb.a * UINT32_C(0x01000000);
    res += rgb.r * UINT32_C(0x00010000);
    res += rgb.g * UINT32_C(0x00000100);
    res += rgb.b * UINT32_C(0x00000001);
    return res;
}

/**
 * Auxiliary struct for the (A)HSV colour space
 */
struct AHSV {
    float a;
    uint16_t h;
    float s;
    float v;
};

/**
 * Converts a triplet of floats between 0 and 1, corresponding to the percentage
 * of the respective maximum value, to an AHSV.
 *
 * @param[in] h value for hue
 * @param[in] s value for saturation
 * @param[in] v value for value
 *
 * @return resulting AHSV
 */
inline struct AHSV
float_to_AHSV(float h, float s, float v)
{
    struct AHSV hsv = {0};
    hsv.h = h * HSV_H_MAX;
    hsv.s = s;
    hsv.v = v;
    return hsv;
}

/**
 * Converts an AHSV into an ARGB.
 *
 * @param[in] hsv AHSV to convert into an ARGB
 *
 * @return resulting ARGB
 */
struct ARGB
AHSV_to_ARGB(struct AHSV hsv);

/**
 * Converts an ARGB into an AHSV.
 *
 * @param[in] rgb ARGB to convert into an AHSV
 *
 * @return resulting AHSV
 */
struct AHSV
ARGB_to_AHSV(struct ARGB rgb);

/**
 * Converts an AHSV into a single uint32_t by first converting it into an ARGB.
 *
 * @param[in] hsv AHSV to convert into an uint32_t
 *
 * @return resulting uint32_t
 */
inline uint32_t
AHSV_to_u32(struct AHSV hsv)
{
    const struct ARGB rgb = AHSV_to_ARGB(hsv);
    return ARGB_to_u32(rgb);
}

#endif /* COLOR_H_INCLUDED */
