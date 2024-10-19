/* data.h
 *
 * Header for data objects
 *
 */

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

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
    KEY_VIEW_SAVE,
    KEY_VIEW_LOAD,
};

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
 * Registers action according to keypress in `key` to global ImageData object.
 *
 * @param[in] key pressed key
 */
void
ImageData_register_action(enum Key key);

/**
 * Performs registered action on global ImageData object for `mseconds`
 * milliseconds. Returns nonzero value if an action has been performed.
 *
 * @param[in] mseconds pressed key
 *
 * @return nonzero value if an action has been performed
 */
int
ImageData_perform_action(unsigned int mseconds);

/**
 * Returns global array of pixel data.
 *
 * @return global array of pixel data
 */
const float *
ImageData_get_pixel_data(void);

#endif /* DATA_H_INCLUDED */
