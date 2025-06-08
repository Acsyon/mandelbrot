#include "unity.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <util/sys.h>

static void
_should_sleepCorrectAmount_when_provideTime(void)
{
    /* Arrange */
    const unsigned int sleeptimes[] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
    const size_t num = (sizeof sleeptimes) / (sizeof *sleeptimes);

    for (size_t i = 0; i < num; ++i) {
        const unsigned int sleeptime = sleeptimes[i];

        /* Act */
        const uint64_t start_time = SDL_GetTicks64();
        msleep(sleeptime);
        const uint64_t end_time = SDL_GetTicks64();

        /* Assert */
        const uint64_t delta = (end_time - start_time);
        TEST_ASSERT_GREATER_OR_EQUAL(sleeptime, delta);
        TEST_ASSERT_LESS_OR_EQUAL(sleeptime + 2, delta);
    }
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

    RUN_TEST(_should_sleepCorrectAmount_when_provideTime);

    return UNITY_END();
}
