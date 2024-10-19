#include <stdlib.h>

#include "video.h"

int
main(void)
{
    Video_init();
    Video_loop();
    Video_quit();

    return EXIT_SUCCESS;
}
