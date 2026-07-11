#include <time.h>
#include <unistd.h>
#include "i_timer.h"
#include "doomtype.h"

static struct timespec basetime;
static boolean basetime_initialized = false;

static void InitBaseTime(void)
{
    if (!basetime_initialized) {
        clock_gettime(CLOCK_MONOTONIC, &basetime);
        basetime_initialized = true;
    }
}

int I_GetTime(void)
{
    InitBaseTime();
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    long long ms = (ts.tv_sec - basetime.tv_sec) * 1000LL + (ts.tv_nsec - basetime.tv_nsec) / 1000000LL;
    return (int)((ms * TICRATE) / 1000);
}

int I_GetTimeMS(void)
{
    InitBaseTime();
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    long long ms = (ts.tv_sec - basetime.tv_sec) * 1000LL + (ts.tv_nsec - basetime.tv_nsec) / 1000000LL;
    return (int)ms;
}

void I_Sleep(int ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void I_WaitVBL(int count)
{
    I_Sleep((count * 1000) / 70);
}

void I_InitTimer(void)
{
    InitBaseTime();
}
