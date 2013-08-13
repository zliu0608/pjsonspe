#include "common.h"
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#include <stdlib.h>  // for _atoi64

#define INT64_C(val) (val##LL)
#define DELTA_EPOCH_IN_USEC   INT64_C(11644473600000000)

#else
  // TODO
#include <cstdlib>  // for atoll
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

/**
 * convert microsecond number since 1970-1-1 midnight(UTC) to a readable time string
 * in format YYYY-MM-DD hh:mm:ss.sss, 
 * Note: this time string represents local time.
 * 
 * @param time  the microsecond number
 * @str the receive char array,  at least has 24 chars.
 */
char* microSecondsToTimeStr(int64_t time, char* str) {
#ifdef WIN32
    FILETIME ftime;
    FILETIME lftime;
    SYSTEMTIME st;
    time += DELTA_EPOCH_IN_USEC;
    time *= 10;
    ftime.dwLowDateTime = time & 0x00000000FFFFFFFF;
    ftime.dwHighDateTime = time >> 32;
    // convert UTC time to local FileTime
    FileTimeToLocalFileTime(&ftime, &lftime);
    FileTimeToSystemTime(&lftime, &st);
    sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return str;
#else
    // TODO
#endif
};

/**
 * convert a readable time string to microsecond number since 1970-1-1 midnight(UTC)
 * time str uses format YYYY-MM-DD hh:mm:ss.sss
 * Note: this time string represents local time.
 * 
 * @param str the date time char array,  at least has 24 chars.
 */
int64_t timeStrToMicroSeconds(char* str) {
#ifdef WIN32
    int64_t time = 0;
    FILETIME lftime;
    FILETIME ftime;

    SYSTEMTIME st;
    sscanf(str, "%04d-%02d-%02d %02d:%02d:%02d.%03d", &st.wYear, &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond, &st.wMilliseconds);
    SystemTimeToFileTime(&st, &lftime);
    LocalFileTimeToFileTime(&lftime, &ftime);

    /* Convert FILETIME one 64 bit number so we can work with it. */
    time = ftime.dwHighDateTime;
    time = time << 32;
    time |= ftime.dwLowDateTime;
    time /= 10;             // Convert from 100 nano-sec periods to micro-seconds.
    time -= DELTA_EPOCH_IN_USEC;  // Convert from Windows epoch to Unix epoch 
    return time;
#else
    // TODO
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

long64 atolong64(const char *str )  {
#ifdef WIN32
    return _atoi64(str);
#else
    return std::atoll(str);
#endif

};
