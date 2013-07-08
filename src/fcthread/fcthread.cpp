/*********************************************************************************************
 * Thread devices
 *                                                           Copyright (C) 2009-2011 MyFC Labs
 *
 * This file is part of foundation class library
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either version
 * 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
**********************************************************************************************/
#include "fcthread.h"
#include "fcconfig.h"

namespace fc {

const uint32_t LOCKBUSYLOOP = 8192;      /// threshold of busy loop and sleep for locking

/**
 * Get current time in sec
 */
double time() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  ::FILETIME ft;
  ::GetSystemTimeAsFileTime(&ft);
  ::LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  return li.QuadPart / 10000000.0;
#else
  struct ::timeval tv;
  if (::gettimeofday(&tv, NULL) != 0) return 0.0;
  return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
};

/**
 * Thread internal.
 */
struct ThreadCore {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	HANDLE th_;                          ///< handle
#else
    pthread_t th_;                        ///< identifier
    bool alive_;                          ///< alive flag
#endif
};


/**
 * Call the running thread.
 * @param arg the thread.
 * @return always NULL.
 */
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
static DWORD threadrun(LPVOID arg);
#else
static void* threadrun(void* arg);
#endif


/**
 * Default constructor.
 */
Thread::Thread() : opq_(NULL) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(true);
  ThreadCore* core = new ThreadCore;
  core->th_ = NULL;
  opq_ = (void*)core;
#else
  assert(true);
  ThreadCore* core = new ThreadCore;
  core->alive_ = false;
  opq_ = (void*)core;
#endif
}

/**
 * Destructor.
 */
Thread::~Thread() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(true);
  ThreadCore* core = (ThreadCore*)opq_;
  if (core->th_) join();
  delete core;
#else
  assert(true);
  ThreadCore* core = (ThreadCore*)opq_;
  if (core->alive_) join();
  delete core;
#endif
}


/**
 * Start the thread.
 */
void Thread::start() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(true);
  ThreadCore* core = (ThreadCore*)opq_;
  if (core->th_) throw std::invalid_argument("already started");
  DWORD id;
  core->th_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadrun, this, 0, &id);
  if (!core->th_) throw std::runtime_error("CreateThread");
#else
  assert(true);
  ThreadCore* core = (ThreadCore*)opq_;
  if (core->alive_) throw std::invalid_argument("already started");
  if (pthread_create(&core->th_, NULL, threadrun, this) != 0)
    throw std::runtime_error("pthread_create");
  core->alive_ = true;
#endif
}


/**
 * Wait for the thread to finish.
 */
void Thread::join() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(true);
  ThreadCore* core = (ThreadCore*)opq_;
  if (!core->th_) throw std::invalid_argument("not alive");
  if (WaitForSingleObject(core->th_, INFINITE) == WAIT_FAILED)
    throw std::runtime_error("WaitForSingleObject");
  CloseHandle(core->th_);
  core->th_ = NULL;
#else
  assert(true);
  ThreadCore* core = (ThreadCore*)opq_;
  if (!core->alive_) throw std::invalid_argument("not alive");
  core->alive_ = false;
  if (pthread_join(core->th_, NULL) != 0) throw std::runtime_error("pthread_join");
#endif
}

/**
 * Terminate the current thread
 */
void Thread::exit() {
#if defined(_SYS_MSVC_) || defined (_SYS_MINGW)
	assert(true);
	ExitThread(0);
#else
	assert(true);
	pthread_exit(NULL);
#endif
}


/**
 * Yield the processor from current thread
 */
void Thread::yield() {
#if defined(_SYS_MSVC_) || defined (_SYS_MINGW)
	assert(true);
	Sleep(0);	
#else
	assert(true);
	sched_yield();
#endif
}

/**
 * Chill the processor by suspending execution for a quick moment
 */
void Thread::chill() {
#if defined(_SYS_MSVC_) || defined (_SYS_MINGW)
	assert(true);
	Sleep(21);	
#else
	assert(true);
	struct ::timespec req;
	req.tv_sec = 0;
	req.tv_nsec = 21 * 1000 * 1000;
	nanosleep(&req, NULL);
#endif
}


/**
 * Suspend execution of the current thread.
 */
bool Thread::sleep(double sec) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
#pragma warning(push)
#pragma warning(disable:4244)	/* prevent compilation warning of converstion from double to DWORD */

	assert(sec >= 0.0);
  if (sec <= 0.0) {
    yield();
    return true;
  }
  if (sec > INT32MAX) sec = INT32MAX;
  Sleep(sec * 1000);
  return true;

#pragma warning(pop)
#else
  assert(sec >= 0.0);
  if (sec <= 0.0) {
    yield();
    return true;
  }
  if (sec > INT32MAX) sec = INT32MAX;
  double integ, fract;
  fract = std::modf(sec, &integ);
  struct ::timespec req, rem;
  req.tv_sec = (time_t)integ;
  req.tv_nsec = (long)(fract * 999999000);
  while (::nanosleep(&req, &rem) != 0) {
    if (errno != EINTR) return false;
    req = rem;
  }
  return true;
#endif
}

int64_t Thread::hash() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(true);
  return GetCurrentThreadId();
#else
  // todo: to add implementation for non-windows platforms
#endif
}

/**
 *  calling the thread function
 */
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
static DWORD threadrun(LPVOID arg) {
	assert(true);
	Thread* thread = (Thread*) arg;
	thread->run();
	return NULL;
}
#else
static void* threadrun(void* arg) {
  assert(true);
  Thread* thread = (Thread*)arg;
  thread->run();
  return NULL;
}
#endif

//--------------- Mutex Class implementation ----------------------//

/**
 *  Default constructor
 */
Mutex::Mutex() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CRITICAL_SECTION* pmutex = new CRITICAL_SECTION;
	InitializeCriticalSection(pmutex);
	opq_  = (void*) pmutex;
#else
	assert(true);
	pthread_mutex_t* pmutex = new pthread_mutex_t;
	if (pthread_mutex_init(pmutex, NULL) != 0) throw std::runtime_error("pthread_mutex_init");
	opq_ = (void*) pmutex;
#endif
}

/**
 *  Constructor with double lock behavior specifications.
 */
Mutex::Mutex(Type type) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CRITICAL_SECTION* pmutex = new CRITICAL_SECTION;
	InitializeCriticalSection(pmutex);
	opq_  = (void*) pmutex;
#else
	assert(true);
	pthread_mutexattr_t attr;
	  if (pthread_mutexattr_init(&attr) != 0) throw std::runtime_error("pthread_mutexattr_init");
	  switch (type) {
		case FAST: {
		  break;
		}
		case ERRORCHECK: {
		  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) != 0)
			throw std::runtime_error("pthread_mutexattr_settype");
		  break;
		}
		case RECURSIVE: {
		  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
			throw std::runtime_error("pthread_mutexattr_settype");
		  break;
		}
	  }
	  pthread_mutex_t* pmutex = new pthread_mutex_t;
	  if (pthread_mutex_init(pmutex, &attr) != 0) throw std::runtime_error("pthread_mutex_init");
	  pthread_mutexattr_destroy(&attr);
	  opq_ = (void*) pmutex;
#endif
}


/**
 * Destructor
 */
Mutex::~Mutex() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CRITICAL_SECTION* pmutex = (CRITICAL_SECTION*) opq_;
	DeleteCriticalSection(pmutex);
	delete pmutex;
	opq_  = 0;
#else
	assert(true);
	pthread_mutex_t* pmutex = (pthread_mutex_t*)opq_;
	pthread_mutex_destroy(pmutex);
	delete pmutex;	
#endif
}

/**
 * Get the lock()
 * This method won't return until the lock is obtained.
 */
void Mutex::lock() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CRITICAL_SECTION* pmutex = (CRITICAL_SECTION*) opq_;
	EnterCriticalSection(pmutex);
#else
	assert(true);
	pthread_mutex_t* pmutex = (pthread_mutex_t*)opq_;
	if (pthread_mutex_lock(pmutex) != 0) throw std::runtime_error("pthread_mutex_lock");
#endif
}


/**
 * Try to get the lock
 * This method will return immediately 
 */
bool Mutex::lock_try() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CRITICAL_SECTION* pmutex = (CRITICAL_SECTION*) opq_;
	if (! TryEnterCriticalSection(pmutex)) 
		return false;
	return true;
#else
	assert(true);
    pthread_mutex_t* pmutex = (pthread_mutex_t*)opq_;
	int32_t ecode = pthread_mutex_trylock(pmutex);
	if (ecode == 0) return true;
	if (ecode != EBUSY) throw std::runtime_error("pthread_mutex_trylock");
	return false;	
#endif
}


/**
 * Try to get the lock
 * This method will return immediately 
 */
bool Mutex::lock_try(double sec) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_) || defined(_SYS_CYGWIN_) || defined(_SYS_MACOSX_)
	assert(sec >= 0.0);
	if (lock_try())
		return true;
	double end = time() + sec;
	Thread::yield();
	uint32_t wcnt = 0;
	while(!lock_try()) {
		if (time() > end) return false;
		if (wcnt > LOCKBUSYLOOP) {
			Thread::chill();
		}
		else {
			Thread::yield();
			wcnt++;
		}
	}
	return true;
#else
	assert(sec >= 0.0);
    pthread_mutex_t* pmutex = (pthread_mutex_t*)opq_;
	struct ::timeval tv;
	struct ::timespec ts;
	if (gettimeofday(&tv, NULL) == 0) {
		double integ;
		double fract = std::modf(sec, &integ);
		ts.tv_sec = tv.tv_sec + (time_t)integ;
		ts.tv_nsec = (long)(tv.tv_usec * 1000.0 + fract * 999999000);
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_nsec -= 1000000000;
			ts.tv_sec++;
		}
	} else {
		ts.tv_sec = std::time(NULL) + 1;
		ts.tv_nsec = 0;
	}
	int32_t ecode = pthread_mutex_timedlock(pmutex, &ts);
	if (ecode == 0) return true;
	if (ecode != ETIMEDOUT) throw std::runtime_error("pthread_mutex_timedlock");
	return false;
#endif
}


/**
 * Release the lock()
 */
void Mutex::unlock() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CRITICAL_SECTION* pmutex = (CRITICAL_SECTION*) opq_;
	LeaveCriticalSection(pmutex);
#else
	assert(true);
	pthread_mutex_t* pmutex = (pthread_mutex_t*)opq_;
	if (pthread_mutex_unlock(pmutex) != 0) throw std::runtime_error("pthread_mutex_lock");
#endif
}


//--------------- CondVar Class implementation ----------------------//

/**
 *  Internal struct used by CondVar
 */
struct CondVarCore {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	CRITICAL_SECTION mutex;			// mutex
	uint32_t wait;					// wait counter
	uint32_t wake;					// wake counter
	HANDLE   sev;					// signal event handle
	HANDLE   fev;					// finish event handle
#else
	pthread_cond_t cond;            // condition
#endif
};

/**
 *  Default Constructor
 */
CondVar::CondVar() : opq_(NULL) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CondVarCore* pcore = new CondVarCore;
	InitializeCriticalSection(&pcore->mutex);
	pcore->wait = 0;
	pcore->wake = 0;
	pcore->sev = CreateEvent(NULL,true,false,NULL);
	if (!pcore->sev) throw std::runtime_error("CreateEvent");
	pcore->fev = CreateEvent(NULL,false,false,NULL);
	if (!pcore->fev) throw std::runtime_error("CreateEvent");
	opq_ = (void*) pcore;
#else
	assert(true);
	CondVarCore* pcore = new CondVarCore;
	if (pthread_cond_init(&pcore->cond, NULL) != 0)
		throw std::runtime_error("pthread_cond_init");
	opq_ = (void*) pcore;
#endif
}

/**
 * Destructor
 */
CondVar::~CondVar() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CondVarCore* pcore = (CondVarCore*) opq_;
	CloseHandle(pcore->sev);
	CloseHandle(pcore->fev);
	DeleteCriticalSection(&pcore->mutex);
	delete pcore;
#else
	assert(true);
	CondVarCore* pcore = (CondVarCore*) opq_;
	pthread_cond_destroy(&pcore->cond);
	delete pcore;
#endif
}

/**
 * Wait for the singnal
 */
void CondVar::wait(Mutex *mutex) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(mutex);
	CondVarCore* pcore = (CondVarCore*) opq_;
	CRITICAL_SECTION *mymutex = (CRITICAL_SECTION*) mutex->opq_;
	pcore->wait++;
	LeaveCriticalSection(mymutex);
	while (true) {
		WaitForSingleObject(pcore->sev, INFINITE);
		EnterCriticalSection(&pcore->mutex);
		if (pcore->wake > 0) {
			pcore->wait--;
			pcore->wake--;
			if (pcore->wake < 1) {
				ResetEvent(pcore->sev);
				SetEvent(pcore->fev);
			}
			LeaveCriticalSection(&pcore->mutex);
			break;
		}
		LeaveCriticalSection(&pcore->mutex);
	}
	EnterCriticalSection(mymutex);
#else
	assert_(mutex);
	CondVarCore* pcore = (CondVarCore*)opq_;
	pthread_mutex_t* mymutex = (pthread_mutex_t*)mutex->opq_;
	if (pthread_cond_wait(&pcore->cond, mymutex) != 0)
		throw std::runtime_error("pthread_cond_wait");
#endif
}

/**
 * Wait for the singnal
 */
bool CondVar::wait(Mutex *mutex, double sec) {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(mutex);
	CondVarCore* pcore = (CondVarCore*) opq_;
	CRITICAL_SECTION *mymutex = (CRITICAL_SECTION*) mutex->opq_;
	pcore->wait++;
	LeaveCriticalSection(mymutex);
	while (true) {
		if (WaitForSingleObject(pcore->sev, sec*1000) == WAIT_TIMEOUT) {
			EnterCriticalSection(mymutex);
			pcore->wait--;
			return false;
		}
		EnterCriticalSection(&pcore->mutex);
		if (pcore->wake > 0) {
			pcore->wait--;
			pcore->wake--;
			if (pcore->wake < 1) {
				ResetEvent(pcore->sev);
				SetEvent(pcore->fev);
			}
			LeaveCriticalSection(&pcore->mutex);
			break;
		}
		LeaveCriticalSection(&pcore->mutex);
	}
	EnterCriticalSection(mymutex);
	return true;
#else
	assert(mutex && sec >= 0);
	if (sec <= 0) return false;
	CondVarCore* pcore = (CondVarCore*)opq_;
	pthread_mutex_t* mymutex = (pthread_mutex_t*)mutex->opq_;
	struct ::timeval tv;
	struct ::timespec ts;
	if (gettimeofday(&tv, NULL) == 0) {
		double integ;
		double fract = std::modf(sec, &integ);
		ts.tv_sec = tv.tv_sec + (time_t)integ;
		ts.tv_nsec = (long)(tv.tv_usec * 1000.0 + fract * 999999000);
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_nsec -= 1000000000;
			ts.tv_sec++;
		}
	} 
	else {
		ts.tv_sec = std::time(NULL) + 1;
		ts.tv_nsec = 0;
	}
	int32_t ecode = pthread_cond_timedwait(&pcore->cond, mymutex, &ts);
	if (ecode == 0) return true;
	if (ecode != ETIMEDOUT && ecode != EINTR)
		throw std::runtime_error("pthread_cond_timedwait");
	return false;
#endif
}


/**
 * Send the wake-up signal to another waiting thread. (i.e. notify)
 * @note The mutex used for the wait method should be locked by the caller.
 */
void CondVar::signal() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CondVarCore* pcore = (CondVarCore*) opq_;
	if (pcore->wait > 0) {
		pcore->wake = 1;  // wake up one thread
		SetEvent(pcore->sev);
		WaitForSingleObject(pcore->fev, INFINITE);
	}
#else
	assert(true);
	CondVarCore* pcore = (CondVarCore*)opq_;
	if (pthread_cond_signal(&pcore->cond) != 0)
		throw std::runtime_error("pthread_cond_signal");
#endif
}

/**
 * Broadcast the wake-up signal to all waiting threads. (i.e. notifyAll)
 * @note The mutex used for the wait method should be locked by the caller.
 */
void CondVar::broadcast() {
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
	assert(true);
	CondVarCore* pcore = (CondVarCore*) opq_;
	if (pcore->wait > 0) {
		pcore->wake = pcore->wait;  // wake up all threads
		SetEvent(pcore->sev);
		WaitForSingleObject(pcore->fev, INFINITE);
	}
#else
	assert(true);
	CondVarCore* pcore = (CondVarCore*)opq_;
	if (pthread_cond_broadcast(&pcore->cond) != 0)
		throw std::runtime_error("pthread_cond_broadcast");
#endif
}

}