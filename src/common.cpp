#include "common.h"

#ifdef WIN32
#include <windows.h>

#define INT64_C(val) (val##LL)
#define DELTA_EPOCH_IN_USEC   INT64_C(11644473600000000)

#else
  // TODO
#endif


int64_t currentMicroSeconds() {
#ifdef WIN32
    int64_t time = 0;
    FILETIME ftime;
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ftime);

    /* Convert FILETIME one 64 bit number so we can work with it. */
    time = ftime.dwHighDateTime;
    time = time << 32;
    time |= ftime.dwLowDateTime;
    time /= 10;             // Convert from 100 nano-sec periods to micro-seconds.
    time -= DELTA_EPOCH_IN_USEC;  // Convert from Windows epoch to Unix epoch 
    return time;
#else
    //TODO
#endif
};

long atomicIncrement(long volatile *p) {
#ifdef WIN32
    return InterlockedIncrement(p);
#else
    //TODO
#endif
};

long atomicDecrement(long volatile *p) {
#ifdef WIN32
    return InterlockedDecrement(p);
#else
    //TODO
#endif
};