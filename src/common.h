#ifndef COMMON_H_
#define COMMON_H_

typedef __int64 int64_t; 
typedef unsigned __int64 uint64_t;

typedef __int64 long64;


// return current time in micro-seconds since the Epoch time 1970-1-1 mid-night
int64_t currentMicroSeconds();

long atomicIncrement(long volatile *p) ;

long atomicDecrement(long volatile *p);


#endif
