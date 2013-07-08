/*********************************************************************************************
 * Thread header file
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

#ifndef FC_THREAD_H_          // dup check
#define FC_THREAD_H_

#include "fccommon.h"
#include "fcconfig.h"

namespace fc {


/**
 * Get current time in sec
 */
double time();

/**
 *  Threading device
 *  void run() is the thread execution body, it needs to be overriden in subclass. 
 */
class Thread {
public:
	explicit Thread();
	virtual ~Thread();

	/**
	 *  Perform concrete execution, must be overriden
	 */
	virtual void run() = 0;
	
	/**
	 * Start a thread
	 */
	void start();

	/**
	 * Wait the thread to finish
	 */
	void join();

	/**
	 *  Terminate the running thread
	 */
	static void exit();

	/**
	 * Yield the processor from the current thread
	 */
	static void yield();

	/**
	 * Chill the processor by suspending execution for a quick moment.
	 */
	static void chill();

	/**
	 * Suspend execution of the current thread
	 * @param sec the interval of suspension in sec
	 * @return true on success, or false on failure
	 */ 
	static bool sleep(double sec);

	/**
	 * Get current thread hash value(threadid)
	 * @return current thread hash value
	 */
	static int64_t hash();
private:
	/** forbid to use copy constructor **/
	Thread(const Thread&);

	/** forbid to use = operator **/
	Thread& operator =(const Thread&);

	/** opaque pointer of thread data **/
	void * opq_;
};


/**
 *  Basic mutual exclusive access device.
 */
class Mutex {
  friend class CondVar;

 public:
  /**
   * Type of the behavior for double locking.
   * only available for non-windows platforms
   */
  enum Type {
    FAST,                ///< no operation
    ERRORCHECK,          ///< check error
    RECURSIVE            ///< allow recursive locking
  };

  /**
   * Default constructor.
   */
  explicit Mutex();

  /**
   * Constructor.
   * @param type the behavior for double locking.
   */
  explicit Mutex(Type type);

  /**
   * Destructor.
   */
  ~Mutex();

  /**
   * Get the lock.
   */
  void lock();

  /**
   * Try to get the lock.
   * @return true on success, or false on failure.
   */
  bool lock_try();

  /**
   * Try to get the lock.
   * @param sec the interval of the suspension in seconds.
   * @return true on success, or false on failure.
   */
  bool lock_try(double sec);

  /**
   * Release the lock.
   */
  void unlock();

 private:
  /** Dummy constructor to forbid the use. */
  Mutex(const Mutex&);

  /** Dummy Operator to forbid the use. */
  Mutex& operator =(const Mutex&);

  /** Opaque pointer. */
  void* opq_;
};


/**
 * Scoped mutex device.
 */
class ScopedMutex {
public:
	/**
	 * Constructor.
	 * @param mutex a mutex to lock the block.
	 */
	explicit ScopedMutex(Mutex* mutex) : pmutex_(mutex) {
		assert(mutex);
		pmutex_->lock();
	}

	/**
	 * Destructor.
	 */
	~ScopedMutex() {
		assert(true);
		pmutex_->unlock();
	}
private:
	/** Dummy constructor to forbid the use. */
	ScopedMutex(const ScopedMutex&);

	/** Dummy Operator to forbid the use. */
	ScopedMutex& operator =(const ScopedMutex&);
	
	Mutex* pmutex_;
};


/**
 *  Condition Variable device
 */
class CondVar {
public: 
	/**
	 * Default constructor
	 */
	explicit CondVar();

	/**
	 * Destuctor
	 */ 
	~CondVar();

	/**
	 * Wait for a signal
	 * @param mutex a locked mutex
	 */
	void wait(Mutex* mutex);

	/**
	 * Wait for a signal
	 * @param mutex a locked mutex
	 * @param sec the timeout in seconds.
	 * @return true on catching a signal, or false on timeout
	 */
	bool wait(Mutex* mutex, double sec);

	/**
	 * Send the wake-up signal to another waiting thread. (i.e. notify)
	 * @note The mutex used for the wait method should be locked by the caller.
	 */
	void signal();

	/**
	 * Send the wake-up signal to all waiting threads. (i.e. notifyAll)
	 * @note The mutex used for the wait method should be locked by the caller.
	 */
	void broadcast();
private: 
	CondVar(const CondVar&);
	CondVar& operator =(const CondVar&);
	
	/** Opaque pointer **/
	void* opq_;
};


class TaskQueue;

/**
 *  Task device
 */
class Task {
	friend class TaskQueue;
public:
	/**
	 *  Default constructor
	 */ 
	explicit Task() : id_(0), thid_(0), aborted_(false) {
	}

	/**
	 *  Destructor
	 */ 
	virtual ~Task() {
	}

	/**
	 *  Task execution body, it needs to be overriden in subclasses
	 */ 
	virtual void execute() = 0;

	/**
	 *  Get the task ID
	 *  @ return ID of the task, starting from 1, increment by 1
	 */ 
	uint64_t id() {
		return id_;
	}

	/**
	 *  Get the task's thread id when and after it's executed
	 *  @return thread id of the task
	 */ 
	uint32_t thread_id() {
		return thid_;	
	}

	/**
	 *  Check if the task is aborted.
	 *  @return true if the task is aborted, false otherwise.
	 */ 
	bool aborted() {
		return aborted_;
	}
private:
	/** The task id number **/
	uint64_t id_;
	/** The task thread id **/
	uint32_t thid_;
	/** The task aborted flag **/
	bool aborted_;
};


class TaskQueue {
private:
	class WorkerThread;
	typedef std::list<Task*> TaskList;

public:
	/**
	 * Default constructor
	 */
	TaskQueue():thary_(NULL), thnum_(0), tasklist_(), count_(0), mutex_(), cond_(), seed_(0) {		
	}

	/**
	 * Destructor
	 */
	virtual ~TaskQueue() {
	}

	/**
	 *  Process a task,  can be overriden in subclass
	 *  @param task a task to be executed.
	 */
	virtual void do_task(Task* task) {
		task->execute();
	}

	/**
	 * start a TaskQueue
	 */
	void start(size_t thnum) {
		assert(thnum>0);
		thary_ = new WorkerThread[thnum];
		thnum_ = thnum;
		for (int i=0; i < thnum; i++) {
			thary_[i].id_ = i;
			thary_[i].queue_ = this;
			thary_[i].start();
		}
	}

	/**
	 * Finish all the tasks
	 * note: this function will block until all tasks in the queue are popped.
	 */
	void finish() {
		mutex_.lock();
		TaskList::iterator it = tasklist_.begin();
		TaskList::iterator itend = tasklist_.end();
		while (it != itend) {
			Task* task = *it;
			task->aborted_ = true;
			++ it;
		}
		cond_.broadcast();
		mutex_.unlock();
		Thread::yield();
		for (double wsec = 1.0 / CLOCKTICK; true; wsec *= 2) {
			mutex_.lock();
			if (tasklist_.empty()) {
				mutex_.unlock();
				break;
			}
			mutex_.unlock();
			if (wsec>1.0) wsec=1.0;
			Thread::sleep(wsec);
		}

		mutex_.lock();
		for (size_t i=0; i<thnum_; i++) {
			thary_[i].aborted_ = true;
		}
		cond_.broadcast();
		mutex_.unlock();
		for (size_t i=0; i<thnum_; i++) {
			thary_[i].join();
		}
		
		delete[] thary_;
		thary_ = NULL;
	}

	/**
	 *  Add a task
	 *  @param task a task to be added into the TaskQueue
	 *  @return the number of tasks in the TaskQueue
	 */
	int64_t add_task(Task* task) {
		assert(task);
		mutex_.lock();
		task->id_ = ++seed_;
		tasklist_.push_back(task);
		int64_t count = count_++;
		cond_.signal();
		mutex_.unlock();
		return count;
	}

	/**
	 *  Get current number of tasks
	 *  @return the total number of tasks
	 */
	int64_t count() {
		mutex_.lock();
		int64_t count = count_;
		mutex_.unlock();
		return count;
	}

private:
	class WorkerThread : public Thread {
		friend class TaskQueue;
	public:
		explicit WorkerThread() : id_(0), queue_(NULL), aborted_(false) {
		};
	private:
		/**
		 *  WorkThread execution body.
		 */
		void run() {
			bool empty = false;
			while(true) {
				queue_->mutex_.lock();
				if (aborted_) {
					queue_->mutex_.unlock();
					break;
				}
				if (empty) queue_->cond_.wait(&queue_->mutex_, 1.0);
				Task* task = NULL;
				if (queue_->tasklist_.empty()) {
					empty = true;
				}
				else {
					task = queue_->tasklist_.front();
					task->thid_ = id_;
					queue_->tasklist_.pop_front();
					queue_->count_ --;
					empty = false;
				}
				queue_->mutex_.unlock();
				if (task) {
					queue_->do_task(task);
				}
			} // end while
		}

		uint32_t id_;
		TaskQueue* queue_;
		Task* task_;
		bool aborted_;
	};

	/** forbid to use copy constructor **/
	TaskQueue(const TaskQueue&);
	/** forbid to use assign operator **/
	TaskQueue& operator =(const TaskQueue&);
	
	/** work thread array **/
	WorkerThread* thary_;
	/** number of work threads **/
	size_t thnum_;
	/** task list **/
	TaskList tasklist_;
	/** number of tasks **/
	int64_t count_;
	/** the mutex of the task list **/
	Mutex mutex_;
	/** the condition variable of the task list **/
	CondVar cond_;
	/** seed of the id numbers **/
	uint64_t seed_;
};

}                             // end namespace

#endif                        // dup check