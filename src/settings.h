/* settings.h
 *
 * Header for (global) settings
 *
 */

#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#include <stdint.h>

/**
 * Struct for settings
 */
typedef struct {
    unsigned int width;
    unsigned int height;
    double max_absval;
    uint16_t max_itrs;
    double max_re;
    double min_re;
    double cntr_re;
    double cntr_im;
    double zoom_spd;
    double scrl_spd;
    unsigned long int prec;
} Settings;

/**
 * Pointer to global settings.
 */
extern const Settings *const GLOBAL_SETTINGS;

#endif /* SETTINGS_H_INCLUDED */
