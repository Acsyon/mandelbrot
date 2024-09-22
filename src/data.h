/* data.h
 *
 * Header for data objects
 *
 */

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

/**
 * Initializes ImageData object according to default settings.
 */
void
ImageData_init(void);

/**
 * Frees memory of global ImageData object.
 */
void
ImageData_free(void);

/**
 * Enumerator for key inputs
 */
enum Key {
    KEY_INVALID = -1,
    KEY_ZOOM_IN,
    KEY_ZOOM_OUT,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_RESET,
};

/**
 * Perform global ImageData object according to keypress in `key`.
 *
 * @param[in] key pressed key
 */
void
ImageData_action(enum Key key);

/**
 * Returns global array of pixel data.
 *
 * @return global array of pixel data
 */
const float *
ImageData_get_pixel_data(void);

#endif /* DATA_H_INCLUDED */
