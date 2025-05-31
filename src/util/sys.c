#include <util/sys.h>

#include <cutil/std/stdio.h>

#if _POSIX_C_SOURCE >= 199309L
    #include <time.h>
#elif defined(__unix__)
    #include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
    #include <windows.h>
#else
    #error "Unsupported system"
#endif

void
msleep(unsigned int mseconds)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = mseconds / 1000;
    ts.tv_nsec = (mseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#elif defined(__unix__)
    sleep(mseconds / 1000);
    usleep((mseconds % 1000) * 1000);
#elif defined(_WIN32) || defined(WIN32)
    Sleep(mseconds);
#endif
}
