#ifndef COMMON_H_
#define COMMON_H_

typedef __int64 int64_t; 
typedef unsigned __int64 uint64_t;

typedef __int64 long64;


// return current time in micro-seconds since the Epoch time 1970-1-1 mid-night
int64_t currentMicroSeconds();

// convert a datetime string (YYYY-MM-DD hh::mm:ss.sss) into a 64bit integer representing microseconds since Epoch time.
int64_t timeStrToMicroSeconds(char* str);

// convert a microseconds number since Epoch time to a datetime string (YYYY-MM-DD hh::mm:ss.sss).
char* microSecondsToTimeStr(int64_t time, char* str);

long atomicIncrement(long volatile *p) ;

long atomicDecrement(long volatile *p);


long64 atolong64(const char *str );

#endif
