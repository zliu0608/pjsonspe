#include "ringbuffer.h"
#include <limits.h>


int ceilingNextPowerOfTwo(const int & y) {
    int x = y;
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}


long getMinimumSequence(const vector<Consumer*> & consumers) {
    long minimum = LONG_MAX;
    for (std::vector<Consumer*>::const_iterator itr = consumers.begin();
        itr < consumers.end();
        ++itr) {
        long sequence = (*itr)->getSequence();
        minimum = minimum < sequence ? minimum : sequence;
    }  
  return minimum;    
}


ClaimStrategy* ClaimStrategy::newInstance(const ClaimStrategyOptions::Option option, long initialSequence) {
    if (ClaimStrategyOptions::SINGLE_THREADED == option) {
        return new SingleThreadedClaimStrategy(initialSequence);
    }
    else {
        // TODO :  to implement MultiThreadedClaimStrategy
        return NULL;
    }
}

