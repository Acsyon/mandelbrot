#include "unity.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <app/key.h>

static void
_should_returnCorrectCooldown_when_provideKey(void)
{
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_ZOOM_IN));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_ZOOM_OUT));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_UP));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_DOWN));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_LEFT));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_RIGHT));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_RESET));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_VIEW_SAVE));
    TEST_ASSERT_EQUAL_UINT(500U, Key_get_cooldown(KEY_VIEW_LOAD));

    TEST_ASSERT_EQUAL_UINT(0U, Key_get_cooldown(KEY_QUIT));
    TEST_ASSERT_EQUAL_UINT(0U, Key_get_cooldown(KEY_TRIP_MODE));

    TEST_ASSERT_EQUAL_UINT(UINT_MAX, Key_get_cooldown(KEY_INVALID));
    TEST_ASSERT_EQUAL_UINT(UINT_MAX, Key_get_cooldown(KEY_COUNT));
    TEST_ASSERT_EQUAL_UINT(UINT_MAX, Key_get_cooldown(-1));
    TEST_ASSERT_EQUAL_UINT(UINT_MAX, Key_get_cooldown(-500));
    TEST_ASSERT_EQUAL_UINT(UINT_MAX, Key_get_cooldown(1000));
}

static void
_should_returnCorrectKeys_when_provideDefaultKeymap(void)
{
    const KeyMap *const map = DEFAULT_KEYMAP;

    TEST_ASSERT_EQUAL_INT(KEY_QUIT, KeyMap_map(map, SDLK_ESCAPE));
    TEST_ASSERT_EQUAL_INT(KEY_QUIT, KeyMap_map(map, SDLK_q));

    TEST_ASSERT_EQUAL_INT(KEY_ZOOM_IN, KeyMap_map(map, SDLK_KP_PLUS));
    TEST_ASSERT_EQUAL_INT(KEY_ZOOM_IN, KeyMap_map(map, SDLK_PLUS));

    TEST_ASSERT_EQUAL_INT(KEY_ZOOM_OUT, KeyMap_map(map, SDLK_KP_MINUS));
    TEST_ASSERT_EQUAL_INT(KEY_ZOOM_OUT, KeyMap_map(map, SDLK_MINUS));

    TEST_ASSERT_EQUAL_INT(KEY_UP, KeyMap_map(map, SDLK_w));
    TEST_ASSERT_EQUAL_INT(KEY_UP, KeyMap_map(map, SDLK_UP));

    TEST_ASSERT_EQUAL_INT(KEY_DOWN, KeyMap_map(map, SDLK_s));
    TEST_ASSERT_EQUAL_INT(KEY_DOWN, KeyMap_map(map, SDLK_DOWN));

    TEST_ASSERT_EQUAL_INT(KEY_LEFT, KeyMap_map(map, SDLK_a));
    TEST_ASSERT_EQUAL_INT(KEY_LEFT, KeyMap_map(map, SDLK_LEFT));

    TEST_ASSERT_EQUAL_INT(KEY_RIGHT, KeyMap_map(map, SDLK_d));
    TEST_ASSERT_EQUAL_INT(KEY_RIGHT, KeyMap_map(map, SDLK_RIGHT));

    TEST_ASSERT_EQUAL_INT(KEY_RESET, KeyMap_map(map, SDLK_r));
    TEST_ASSERT_EQUAL_INT(KEY_RESET, KeyMap_map(map, SDLK_KP_0));
    TEST_ASSERT_EQUAL_INT(KEY_RESET, KeyMap_map(map, SDLK_0));

    TEST_ASSERT_EQUAL_INT(KEY_VIEW_SAVE, KeyMap_map(map, SDLK_F5));
    TEST_ASSERT_EQUAL_INT(KEY_VIEW_LOAD, KeyMap_map(map, SDLK_F9));
    TEST_ASSERT_EQUAL_INT(KEY_CHANGE_PALETTE, KeyMap_map(map, SDLK_c));
    TEST_ASSERT_EQUAL_INT(KEY_TRIP_MODE, KeyMap_map(map, SDLK_t));

    TEST_ASSERT_EQUAL_INT(KEY_INVALID, KeyMap_map(map, SDLK_UNKNOWN));
    TEST_ASSERT_EQUAL_INT(KEY_INVALID, KeyMap_map(map, SDLK_F13));
    TEST_ASSERT_EQUAL_INT(KEY_INVALID, KeyMap_map(map, -500));
    TEST_ASSERT_EQUAL_INT(KEY_INVALID, KeyMap_map(map, 1000));
}

static void
_should_returnCorrectCategory_when_provideKey(void)
{
    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_DATA, KeyCategory_categorize(KEY_ZOOM_IN)
    );
    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_DATA, KeyCategory_categorize(KEY_ZOOM_OUT)
    );
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_DATA, KeyCategory_categorize(KEY_UP));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_DATA, KeyCategory_categorize(KEY_DOWN));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_DATA, KeyCategory_categorize(KEY_LEFT));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_DATA, KeyCategory_categorize(KEY_RIGHT));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_DATA, KeyCategory_categorize(KEY_RESET));
    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_DATA, KeyCategory_categorize(KEY_VIEW_SAVE)
    );
    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_DATA, KeyCategory_categorize(KEY_VIEW_LOAD)
    );

    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_VIDEO, KeyCategory_categorize(KEY_QUIT));
    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_VIDEO, KeyCategory_categorize(KEY_CHANGE_PALETTE)
    );
    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_VIDEO, KeyCategory_categorize(KEY_TRIP_MODE)
    );

    TEST_ASSERT_EQUAL_UINT(
      KEYCATEGORY_MISC, KeyCategory_categorize(KEY_INVALID)
    );
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_MISC, KeyCategory_categorize(KEY_COUNT));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_MISC, KeyCategory_categorize(-1));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_MISC, KeyCategory_categorize(-500));
    TEST_ASSERT_EQUAL_UINT(KEYCATEGORY_MISC, KeyCategory_categorize(1000));
}

static void
_should_allocKeyBufferCorrectly(void)
{
    /* Arrange */
    /* Act */
    KeyBuffer *const keybuf = KeyBuffer_alloc();

    /* Assert */
    TEST_ASSERT_NOT_NULL(keybuf);
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_MISC)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_VIDEO)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_DATA)
    );

    /* Cleanup */
    KeyBuffer_free(keybuf);
}

static void
_should_registerKeyDownCorrectly_when_haveDataAction(void)
{
    /* Arrange */
    KeyBuffer *const keybuf = KeyBuffer_alloc();

    /* Act */
    KeyBuffer_register_key_down(keybuf, KEY_ZOOM_IN);
    KeyBuffer_register_key_down(keybuf, KEY_ZOOM_OUT);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(
      KEY_ZOOM_OUT, KeyBuffer_pop_key(keybuf, KEYCATEGORY_DATA)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_DATA)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_VIDEO)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_MISC)
    );

    /* Cleanup */
    KeyBuffer_free(keybuf);
}

static void
_should_registerKeyDownCorrectly_when_haveVideoAction(void)
{
    /* Arrange */
    KeyBuffer *const keybuf = KeyBuffer_alloc();

    /* Act */
    KeyBuffer_register_key_down(keybuf, KEY_CHANGE_PALETTE);
    KeyBuffer_register_key_down(keybuf, KEY_TRIP_MODE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(
      KEY_TRIP_MODE, KeyBuffer_pop_key(keybuf, KEYCATEGORY_VIDEO)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_CHANGE_PALETTE, KeyBuffer_pop_key(keybuf, KEYCATEGORY_VIDEO)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_DATA)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_MISC)
    );

    /* Cleanup */
    KeyBuffer_free(keybuf);
}

static void
_should_registerKeyDownCorrectly_when_haveMultipleActions(void)
{
    /* Arrange */
    KeyBuffer *const keybuf = KeyBuffer_alloc();

    /* Act */
    KeyBuffer_register_key_down(keybuf, KEY_ZOOM_IN);
    KeyBuffer_register_key_down(keybuf, KEY_CHANGE_PALETTE);
    KeyBuffer_register_key_down(keybuf, KEY_ZOOM_OUT);
    KeyBuffer_register_key_down(keybuf, KEY_TRIP_MODE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(
      KEY_TRIP_MODE, KeyBuffer_pop_key(keybuf, KEYCATEGORY_VIDEO)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_CHANGE_PALETTE, KeyBuffer_pop_key(keybuf, KEYCATEGORY_VIDEO)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_ZOOM_OUT, KeyBuffer_pop_key(keybuf, KEYCATEGORY_DATA)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_DATA)
    );
    TEST_ASSERT_EQUAL_INT(
      KEY_INVALID, KeyBuffer_pop_key(keybuf, KEYCATEGORY_MISC)
    );

    /* Cleanup */
    KeyBuffer_free(keybuf);
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    
    UNITY_BEGIN();

    RUN_TEST(_should_returnCorrectCooldown_when_provideKey);
    RUN_TEST(_should_returnCorrectKeys_when_provideDefaultKeymap);
    RUN_TEST(_should_returnCorrectCategory_when_provideKey);
    RUN_TEST(_should_allocKeyBufferCorrectly);
    RUN_TEST(_should_registerKeyDownCorrectly_when_haveDataAction);
    RUN_TEST(_should_registerKeyDownCorrectly_when_haveVideoAction);
    RUN_TEST(_should_registerKeyDownCorrectly_when_haveMultipleActions);
    
    return UNITY_END();
}
