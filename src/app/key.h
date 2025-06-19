/* app/key.h
 *
 * Header for key mapping and key buffering
 *
 */

#ifndef MANDELBROT_APP_KEY_H_INCLUDED
#define MANDELBROT_APP_KEY_H_INCLUDED

/**
 * Enumerator for key inputs
 */
enum Key {
    KEY_INVALID,
    KEY_QUIT,
    KEY_ZOOM_IN,
    KEY_ZOOM_OUT,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_RESET,
    KEY_VIEW_SAVE,
    KEY_VIEW_LOAD,
    KEY_CHANGE_PALETTE,
    KEY_TRIP_MODE,
    KEY_COUNT,
};

/**
 * Returns the cooldown of Key `key`, i.e., the time period during which a key
 * that remains pressed does not activate the same action again.
 *
 * @param[in] key Key to get cooldown of
 *
 * @returns cooldown of Key `key`
 */
unsigned int
Key_get_cooldown(enum Key key);

/**
 * Opaque keymap object
 */
typedef struct _keyMap KeyMap;

/**
 * Default Keymap
 */
extern const KeyMap *const DEFAULT_KEYMAP;

/**
 * Maps raw key `raw_key` to a Key according to `keymap`.
 *
 * @param[in] keymap KeyMap to use for mapping
 * @param[in] raw_key code of input key to map
 *
 * @returns mapped input key
 */
enum Key
KeyMap_map(const KeyMap *keymap, int raw_key);

/**
 * Enumerator for categories of key inputs
 */
enum KeyCategory {
    KEYCATEGORY_MISC,
    KEYCATEGORY_DATA,
    KEYCATEGORY_VIDEO,
};

/**
 * Maps Key `key` to its KeyCategory.
 *
 * @param[in] key Key to categorize
 *
 * @returns KeyCategory of `key`
 */
enum KeyCategory
KeyCategory_categorize(enum Key key);

/**
 * Opaque key buffer object
 */
typedef struct _keyBuffer KeyBuffer;

/**
 * Initializes and returns newly malloc'd KeyBuffer object.
 *
 * @return newly malloc'd KeyBuffer object
 */
KeyBuffer *
KeyBuffer_alloc(void);

/**
 * Frees memory pointed to by `keybuf`.
 *
 * @param[in] keybuf pointer to KeyBuffer object to be freed
 */
void
KeyBuffer_free(KeyBuffer *keybuf);

/**
 * Registers in KeyBuffer `buffer` that Key `key` has been pressed.
 *
 * @param[in] keybuf KeyBuffer to register pressed key in
 * @param[in] key Key to register press of
 */
void
KeyBuffer_register_key_down(KeyBuffer *keybuf, enum Key key);

/**
 * Registers in KeyBuffer `buffer` that Key `key` has been released.
 *
 * @param[in] keybuf KeyBuffer to register released key in
 * @param[in] key Key to register release of
 */
void
KeyBuffer_register_key_up(KeyBuffer *keybuf, enum Key key);

/**
 * Returns next key in `keybuf` for KeyCategory `category` or KEY_INVALID if
 * there is no key left for this category.
 *
 * @param[in] keybuf KeyBuffer to pop key of
 * @param[in] category KeyCategory to pop key of
 *
 * @return next key in `keybuf` for KeyCategory `category` or KEY_INVALID if
 * there is no key left for this category
 */
enum Key
KeyBuffer_pop_key(KeyBuffer *keybuf, enum KeyCategory category);

#endif /* MANDELBROT_APP_KEY_H_INCLUDED */
