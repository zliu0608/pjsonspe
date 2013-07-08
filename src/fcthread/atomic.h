#ifndef __X86_ATOMIC_H
#define __X86_ATOMIC_H

#include <windows.h>

long atomic_increment(long volatile * addend ) {
	return InterlockedIncrement(addend);
}

long atomic_decrement(long volatile * addend ) {
	return InterlockedDecrement(addend);
}


#endif
