#include "key.h"

#include <limits.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include <cutil/log.h>

unsigned int
Key_get_cooldown(enum Key key)
{
    switch (key) {
    case KEY_ZOOM_IN:
    case KEY_ZOOM_OUT:
    case KEY_UP:
    case KEY_DOWN:
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_RESET:
    case KEY_VIEW_SAVE:
    case KEY_VIEW_LOAD:
        return 500U;
    case KEY_TRIP_MODE:
    case KEY_QUIT:
        return 0U;
    case KEY_CHANGE_PALETTE:
    default:
        return UINT_MAX;
    }
}

/**
 * Currently, the key map is just a function pointer to the map function
 */
struct KeyMap {
    enum Key (*map_fnc)(SDL_Keycode sdl_key);
};

static enum Key
_default_keymap_map(SDL_Keycode sdl_key)
{
    switch (sdl_key) {
    case SDLK_ESCAPE:
    case SDLK_q:
        return KEY_QUIT;
    case SDLK_KP_PLUS:
    case SDLK_PLUS:
        return KEY_ZOOM_IN;
    case SDLK_KP_MINUS:
    case SDLK_MINUS:
        return KEY_ZOOM_OUT;
    case SDLK_w:
    case SDLK_UP:
        return KEY_UP;
    case SDLK_s:
    case SDLK_DOWN:
        return KEY_DOWN;
    case SDLK_a:
    case SDLK_LEFT:
        return KEY_LEFT;
    case SDLK_d:
    case SDLK_RIGHT:
        return KEY_RIGHT;
    case SDLK_r:
    case SDLK_KP_0:
    case SDLK_0:
        return KEY_RESET;
    case SDLK_F5:
        return KEY_VIEW_SAVE;
    case SDLK_F9:
        return KEY_VIEW_LOAD;
    case SDLK_c:
        return KEY_CHANGE_PALETTE;
    case SDLK_t:
        return KEY_TRIP_MODE;
    default:
        return KEY_INVALID;
    }
}

const KeyMap DEFAULT_KEYMAP_OBJECT = {.map_fnc = &_default_keymap_map};
const KeyMap *const DEFAULT_KEYMAP = &DEFAULT_KEYMAP_OBJECT;

enum Key
KeyMap_map(const KeyMap *keymap, int raw_key)
{
    const SDL_Keycode sdl_key = (SDL_Keycode) raw_key;
    return keymap->map_fnc(sdl_key);
}

enum KeyCategory
KeyCategory_categorize(enum Key key)
{
    switch (key) {
    case KEY_ZOOM_IN:
    case KEY_ZOOM_OUT:
    case KEY_UP:
    case KEY_DOWN:
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_RESET:
    case KEY_VIEW_SAVE:
    case KEY_VIEW_LOAD:
        return KEYCATEGORY_DATA;
    case KEY_QUIT:
    case KEY_CHANGE_PALETTE:
    case KEY_TRIP_MODE:
        return KEYCATEGORY_VIDEO;
    default:
        return KEYCATEGORY_MISC;
    }
}

/**
 * Auxiliary struct for entries of the key buffer
 */
struct _keyBufferEntry {
    bool is_pressed;       /**< Is this key currently pressed? */
    uint64_t last_pressed; /**< Tick, the number was last pressed/updated */
    bool lock;             /**< Potential lock to prevent "stack overflow" */
};

static void
_keyBufferEntry_init(struct _keyBufferEntry *entry)
{
    entry->is_pressed = false;
    entry->lock = false;
}

/**
 * Auxiliary struct for a quick-and-dirty stack for key presses
 */
struct _keyStack {
    int size;
    enum Key *data;
    int top;
};

#define EMPTY_STACK_POS -1

static struct _keyStack *
_keyStack_alloc(int size)
{
    struct _keyStack *const stack = malloc(sizeof *stack);

    stack->size = size;
    stack->data = malloc(size * sizeof *stack->data);
    stack->top = EMPTY_STACK_POS;

    return stack;
}

static void
_keyStack_free(struct _keyStack *stack)
{
    if (stack == NULL) {
        return;
    }

    free(stack->data);

    free(stack);
}

static bool
_keyStack_push(struct _keyStack *stack, enum Key key)
{
    if (key == KEY_INVALID) {
        return false;
    }
    if (stack->top == stack->size - 1) {
        cutil_log_warn("Stack is full\n");
        return false;
    }
    stack->data[++stack->top] = key;
    return true;
}

static enum Key
_keyStack_pop(struct _keyStack *stack)
{
    if (stack->top == EMPTY_STACK_POS) {
        return KEY_INVALID;
    }
    return stack->data[stack->top--];
}

/**
 * Actual struct of the key buffer: We track for each key if and when it was
 * pressed using the entries in `keys`. Since we only ever allow for a single
 * data action to be registered at the same time, we just have a single Key
 * element `data_action`. For video actions, we allow for an unlimited number of
 * actions, i.e., three, and thus we have a _keyStack `stack_video`.
 */
struct KeyBuffer {
    struct _keyBufferEntry *keys;
    struct _keyStack *stack_video;
    enum Key data_action;
};

#define VIDEO_ACTION_COUNT 3

KeyBuffer *
KeyBuffer_alloc(void)
{
    KeyBuffer *const keybuf = malloc(sizeof *keybuf);

    keybuf->keys = malloc(KEY_COUNT * sizeof *keybuf->keys);
    for (size_t i = 0; i < KEY_COUNT; ++i) {
        _keyBufferEntry_init(&keybuf->keys[i]);
    }
    keybuf->stack_video = _keyStack_alloc(VIDEO_ACTION_COUNT);
    keybuf->data_action = KEY_INVALID;

    return keybuf;
}

void
KeyBuffer_free(KeyBuffer *keybuf)
{
    if (keybuf == NULL) {
        return;
    }

    _keyStack_free(keybuf->stack_video);
    free(keybuf->keys);

    free(keybuf);
}

/**
 * Auxiliary function that returns whether the Key `key` should be registered.
 * To this end, the function checks whether `key` is already pressed. If not,
 * the key is always registered and its timestamp is set to the current number
 * of ticks. If the key is pressed, it is checked whether the cooldown has
 * passed. If so, the key is registered and its timestamp is updated. Otherwise,
 * the key is not registered.
 *
 * @param[in] keybuf KeyBuffer to use for check
 * @param[in] key key to be checked
 *
 * @return should Key `key` should be registered?
 */
static bool
_keyBuffer_register_key_down_aux(KeyBuffer *keybuf, enum Key key)
{
    const unsigned int cooldown = Key_get_cooldown(key);
    const uint64_t current_tick = SDL_GetTicks64();
    struct _keyBufferEntry *const entry = &keybuf->keys[key];
    if (!entry->is_pressed) {
        entry->is_pressed = true;
        entry->last_pressed = current_tick;
        return true;
    } else {
        if (current_tick - entry->last_pressed > cooldown) {
            entry->last_pressed = current_tick;
            return true;
        }
    }
    return false;
}

void
KeyBuffer_register_key_down(KeyBuffer *keybuf, enum Key key)
{
    struct _keyBufferEntry *const entry = &keybuf->keys[key];
    const enum KeyCategory category = KeyCategory_categorize(key);
    switch (category) {
    case KEYCATEGORY_VIDEO:
        if (!entry->lock && _keyBuffer_register_key_down_aux(keybuf, key)) {
            _keyStack_push(keybuf->stack_video, key);
            entry->lock = true;
        }
        break;
    case KEYCATEGORY_DATA:
        if (_keyBuffer_register_key_down_aux(keybuf, key)) {
            keybuf->data_action = key;
        }
        break;
    case KEYCATEGORY_MISC:
    default:
        break;
    }
}

void
KeyBuffer_register_key_up(KeyBuffer *keybuf, enum Key key)
{
    keybuf->keys[key].is_pressed = false;
}

enum Key
KeyBuffer_pop_key(KeyBuffer *keybuf, enum KeyCategory category)
{
    switch (category) {
    case KEYCATEGORY_VIDEO: {
        const enum Key key = _keyStack_pop(keybuf->stack_video);
        keybuf->keys[key].lock = false;
        return key;
    } break;
    case KEYCATEGORY_DATA: {
        const enum Key key = keybuf->data_action;
        keybuf->data_action = KEY_INVALID;
        return key;
    } break;
    case KEYCATEGORY_MISC:
    default:
        break;
    }
    return KEY_INVALID;
}
