#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#ifdef _WIN32
#include <windows.h>
//#define sleep(A)  Sleep(1000 * A) 
#else
#endif
#include "common.h"
#include <time.h>
#include <vector>
#include <exception>

using namespace std;

class Consumer;
template <class T> class ConsumerBarrier;
template <class T> class RingBuffer;


/** Set -1 as sequence starting point */
#define RINGBUFFER_INITIAL_CURSOR_VALUE -1L


/**
 * Calculate the next power of 2, greater than or equal to x.
 *
 * @param x Value to round up
 * @return The next power of 2 from x inclusive
 */
int ceilingNextPowerOfTwo(const int & x); 
  

/**
 * Get the minimum sequence from an array of Consumers.
 * 
 * @param consumers to be checked
 * @return the minimum sequence found or Long.MAX_VALUE if the array is empty.
 */
long64 getMinimumSequence(const vector<Consumer*> & consumers);


/**
 * A base implementation for RingBuffer entries.
 *
 */
class AbstractEntry {
public:
    virtual const long64 getSequence() const {
        return sequence_;
    }

    void setSequence(long64 sequence) {
        sequence_ = sequence;
    }

private:
    long64 sequence_;          // sequence number stored in the entry
};


class ClaimStrategyOptions {
public:
    enum Option {
        MULTI_THREADED,            // use in multiple producers cases
        SINGLE_THREADED            // use in single producer case
    };
};

/**
 * Strategy used by Producers for claiming entries in RingBuffer.
 *
 */
class ClaimStrategy {
public:
    /** 
     * Claim the next sequence index in the RingBuffer and increment
     *
     * return Entry index which can be used by producer
     */
    virtual long64 incrementAndGet() = 0;


    /**
     * Claim by a delta
     */
    virtual long64 incrementAndGet(const int & delta) = 0;


    static ClaimStrategy* newInstance(ClaimStrategyOptions::Option option, long64 intialSequence);
};


class SingleThreadedClaimStrategy : public ClaimStrategy {
public:
    SingleThreadedClaimStrategy(long64 sequence) : sequence_(sequence) {
    }

    virtual long64 incrementAndGet() {
        return ++sequence_;
    }

    virtual long64 incrementAndGet(const int & delta) {
        sequence_ += delta;
        return sequence_;
    }
private:
    long64 sequence_;
};


class WaitStrategyOptions {
public: 
    enum Option {
        BLOCKING,
        YIELDING,
        BUSY_SPIN
    };
};

/**
 * Strategy used by Consumers to wait on a RingBuffer
 *
 */
template <class Entry>
class WaitStrategy {
public:

    /**
     * Wait for a given sequence to be available for consuption in a RingBuffer
     * 
     * @param consumers  the watching consumers which should advance first
     * @param ringBuffer on which to wait
     * @param barrier the consumer is waiting on
     * @param sequence to be waited for
     * @return the sequence that is available which may be greater than the requested sequence
     * @throws InterruptedException if the thread is interrupted.
     */
    virtual long64 waitFor(const std::vector<Consumer*> & consumers,
                         RingBuffer<Entry> & ringBuffer,
                         ConsumerBarrier<Entry> & barrier,
                         const long64 & sequence)  = 0;


    /**
     * Wait for a given sequence to be available for consuption in a RingBuffer
     * 
     * @param consumers  the watching consumers which should advance first
     * @param ringBuffer on which to wait
     * @param barrier the consumer is waiting on
     * @param sequence to be waited for
     * @param timeout_micros maximum time to be waited
     * @return the sequence that is available which may be greater than the requested sequence
     * @throws InterruptedException if the thread is interrupted.
     */
    virtual long64 waitFor(const std::vector<Consumer*> & consumers,
                         RingBuffer<Entry> & ringBuffer,
                         ConsumerBarrier<Entry> & barrier,
                         const long64 & sequence, 
                         const long & timeout_micros)  = 0;

    /**
     * Signal those waiting that the RingBuffer cursor has advanced.
     */
    virtual void signalAll() = 0;


    /**
     * WaitStrategy factory method
     */
    static WaitStrategy<Entry>* newInstance(WaitStrategyOptions::Option option) {
        // TODO: to further implement 2 other waiting strategies
        switch (option)  {
            case WaitStrategyOptions::BLOCKING:
                return NULL;
            case WaitStrategyOptions::YIELDING:
                return new YieldingWaitStrategy();
            case WaitStrategyOptions::BUSY_SPIN:
                return NULL;
            default:
                return NULL;
        }
    }

private:
    // concrete WaitStrategy implementations

    /**
     * Yielding strategy that uses a sleep(0) for Consumers waiting on a barrier.
     *
     * This strategy is a good compromise between performance and CPU resource.
     */
    class YieldingWaitStrategy : public WaitStrategy<Entry> {
    public:
        //@Override
        virtual long64 waitFor(const std::vector<Consumer*> & consumers,
                         RingBuffer<Entry> & ringBuffer,
                         ConsumerBarrier<Entry> & barrier,
                         const long64 & sequence) {                       
            long64 availableSequence = 0;
            if (0 == consumers.size()) {
                while ((availableSequence = ringBuffer.getCursor()) < sequence) {
#ifdef _WIN32
                    #if(_WIN32_WINNT >= 0x0400)
                         if(!SwitchToThread())
                            Sleep(0);
                    #else
                        Sleep(0);
                    #endif
#else
                    sleep(0);  // yield the thread
#endif
                }
            }
            else {
                while ((availableSequence = getMinimumSequence(consumers)) < sequence) {
#ifdef _WIN32
                    #if(_WIN32_WINNT >= 0x0400)
                         if(!SwitchToThread())
                            Sleep(0);
                    #else
                        Sleep(0);
                    #endif
#else
                    sleep(0);  // yield the thread
#endif
                }
            }
            return availableSequence;
        };

        //@Override
        virtual long64 waitFor(const std::vector<Consumer*> & consumers,
                         RingBuffer<Entry> & ringBuffer,
                         ConsumerBarrier<Entry> & barrier,
                         const long64 & sequence, 
                         const long & timeout_micros) {
            long64 availableSequence = 0;
            clock_t t1, t2;
            t1 = clock();
            t2 = t1;
            if (0 == consumers.size()) {
                while ((availableSequence = ringBuffer.getCursor()) < sequence) {
#ifdef _WIN32
                    for (int n =0; n<100; n++) {
                        if (ringBuffer.getCursor()>= sequence)
                            break;
                    #if(_WIN32_WINNT >= 0x0400)
                        if(!SwitchToThread())
                            Sleep(0);                        
                    #else
                        Sleep(0);
                    #endif
                    }
                    if (ringBuffer.getCursor()< sequence) {
                        Sleep(1);
                        t2 = clock();
                        if ( (t2-t1)/CLOCKS_PER_SEC * 1000000 > timeout_micros)
                            break;
                    }
                   
#else
                    sleep(0);  // yield the thread
#endif
                }
            }
            else {
                while ((availableSequence = getMinimumSequence(consumers)) < sequence) {
                    t2 = clock();
                    if ( (t2-t1)/CLOCKS_PER_SEC * 1000000 > timeout_micros)
                        break;

#ifdef _WIN32
                    #if(_WIN32_WINNT >= 0x0400)
                        if(!SwitchToThread())
                            Sleep(0);
                    #else
                        Sleep(0);
                    #endif
#else
                    sleep(0);  // yield the thread
#endif
                }
            }
#ifdef _TRACK_WAIT_TIMES
            if (t2-t1 > 1) 
                printf("consumer waited %lld clicks\n", t2-t1);
#endif
            return availableSequence;
        };

        //@Override
        virtual void signalAll() {}
    }; // end of YieldingWaitStrategy
};


/**
 * A holder for tracking a batch of claimed sequences in a RingBuffer
 * used by ProducerBarrier
 */
class SequenceBatch {
public:
    SequenceBatch(const int & size) :
      size_(size), end_(RINGBUFFER_INITIAL_CURSOR_VALUE) {
    }

    long64 getEnd() {
        return end_;
    }

    void setEnd(const long64 & end) {
        end_ = end;
    }

    int getSize() {
        return size_;
    }

    long64 getStart() {
        return end_ - (size_ - 1L);
    }
private:
    long64 end_;
    const int size_;
};

/**
 * Abstraction for claiming Entries in a RingBuffer while tracking dependent Consumers
 *
 * @param <T> AbstractEntry implementation stored in the RingBuffer
 */
template <class Entry>
class ProducerBarrier {
public:
    /**
     * Claim the next entry in sequence for a producer
     *
     * @return the claimed entry
     */
    virtual Entry* nextEntry() = 0;

    /**
     * Claim the next batch of entries from a RingBuffer
     *
     * @param sequenceBatch to be updated for the batch range
     * @return the updated SequenceBatch
     */
    virtual SequenceBatch nextEntries(SequenceBatch sequenceBatch) = 0;

    /**
     * Commit an entry to the RingBuffer, make it visible to consumers
     *
     * @param entry to be committed
     */
    virtual void commit(const Entry& entry) = 0;

    /**
     * Commit a batch of entries to the RingBuffer, make them visible to consumers
     *
     * @param sequenceBatch to be committed
     */
    virtual void commit(SequenceBatch sequenceBatch) = 0;

    /**
     * Get a entry given 
     */
    virtual Entry* getEntry(const long64 & sequence) = 0;

    /**
     * Delegate the call to RingBuffer and get its cursor value
     */
    virtual long64 getCursor() = 0;
};


/**
 * Abstraction of coordination barrier for tracking the cursor for producers and sequence of
 * dependent consumers for a RingBuffer 
 */
template <class Entry>
class ConsumerBarrier {
public:
    /**
     * Wait for a given sequence to be available for consumption
     *
     * @return the sequence up to which is available
     */
    virtual long64 waitFor(const long64 & sequence) = 0;

    /**
     * Wait for a given sequence to be available for consumption until timeout
     *
     * @return the sequence up to which is available
     */
    virtual long64 waitFor(const long64 & sequence, const long & timeout_micros) = 0;

    /**
     * Get a entry given 
     */
    virtual Entry * getEntry(const long64 & sequence) = 0;

    /**
     * Delegate the call to RingBuffer and get its cursor value
     */
    virtual long64 getCursor() = 0;
};



/**
 * A concrete ProducerBarrier implementation
 * it can track multiple consumers when trying to claim Entries in a RingBuffer
 */
template <class Entry>
class ConsumerTrackingProducerBarrier : public ProducerBarrier<Entry> {
public:
    ConsumerTrackingProducerBarrier(RingBuffer<Entry> * parent, vector<Consumer*> consumers) 
       : parent_(parent), 
         consumers_(consumers), 
         lastConsumerMinimum_(RINGBUFFER_INITIAL_CURSOR_VALUE) {
        // prevent from setting up a ring buffer only with producers
        if (0 == consumers_.size()) {
        #ifdef _WIN32
            #pragma message ("THROW AN APPROPRIATE EXCEPTION")
        #else
            #warning "THROW AN APPROPRIATE EXCEPTION"
        #endif
            exit(37);
        }        
    }

    //@Override
    virtual Entry* nextEntry() {
        const long64 sequence = parent_->claimStrategy_.incrementAndGet();
        // wait in this method
        ensureConsumersAreInRange(sequence);

        Entry * entry = &(parent_->entries_[(int)sequence & parent_->ringModMask_]);
        entry->setSequence(sequence);
        return entry;
    }

    //@Override
    virtual void commit(const Entry & entry) {
        commit(entry.getSequence(), 1);
    }
    
    // @Override
    virtual SequenceBatch nextEntries(SequenceBatch sequenceBatch) {
        const long64 sequence = parent_->claimStrategy_.incrementAndGet(sequenceBatch.getSize());
        sequenceBatch.setEnd(sequence);

        // wait in this method
        ensureConsumersAreInRange(sequence);
        Entry * entry;
        for (long64 i = sequenceBatch.getStart(), end = sequenceBatch.getEnd(); i <= end; i++) {
            entry = &(parent_->entries_[(int)i & parent_->ringModMask_]);
            entry->setSequence(i);            
        }
        return sequenceBatch;
    }

    // @Override
    virtual void commit(SequenceBatch sequenceBatch) {
        commit(sequenceBatch.getEnd(), sequenceBatch.getSize());
    }

    //@Override
    virtual Entry * getEntry(const long64 & sequence) {
        return &(parent_->entries_[(int)sequence & parent_->ringModMask_]);
    }

    //Override
    virtual long64 getCursor() {
        return parent_->cursor_;
    }

protected:
    void ensureConsumersAreInRange(const long64 & sequence) {
        const long64 wrapPoint = sequence - (long64) parent_->entries_.size();
        
#ifdef _TRACK_WAIT_TIMES
        clock_t t1, t2;
        t1 = clock();
        t2 = t1;
#endif        
        // wait until the sequence is available for write
        while (wrapPoint > lastConsumerMinimum_ &&
            wrapPoint > (lastConsumerMinimum_ = getMinimumSequence(consumers_))) {
            
            for (int n =0; n<100; n++) {
                if (!(wrapPoint > lastConsumerMinimum_ &&
                      wrapPoint > (lastConsumerMinimum_ = getMinimumSequence(consumers_))
                      ))
                    break;
#ifdef _WIN32
        #if(_WIN32_WINNT >= 0x0400)
            if(!SwitchToThread())
                Sleep(0);
        #else
            Sleep(0);
        #endif
#else
            sleep(0);  // yield the thread
#endif
            } // end for loop

            if (wrapPoint > lastConsumerMinimum_ &&
                  wrapPoint > (lastConsumerMinimum_ = getMinimumSequence(consumers_))
                )
                Sleep(1);

#ifdef _TRACK_WAIT_TIMES
             t2 = clock();
#endif
        } // end while loop

#ifdef _TRACK_WAIT_TIMES
        if (t2 - t1 > 1)
            printf("producer waited %lld clicks\n", t2-t1);
#endif
    }

    void commit(const long64 & sequence, const long & batchSize){
        if (ClaimStrategyOptions::MULTI_THREADED == parent_->claimStrategyOption_) {
            const long64 expectedSequence = sequence - batchSize;
            while (expectedSequence != parent_->cursor_) {
                // busy spin until other producers commit
            }
        }

        parent_->cursor_ = sequence;
        parent_->waitStrategy_.signalAll();
    }

private:
    vector<Consumer*> consumers_;
    RingBuffer<Entry> * parent_;
    long64 lastConsumerMinimum_;
};


/**
 * A concrete ConsumerBarrier implementation
 */
template <class Entry>
class ConsumerTrackingConsumerBarrier : public ConsumerBarrier<Entry> {
public:
    ConsumerTrackingConsumerBarrier(RingBuffer<Entry> * parent, vector<Consumer*> consumers)
        : parent_(parent), consumers_(consumers) {
    }

    virtual long64 waitFor(const long64 & sequence) {
        return parent_->waitStrategy_.waitFor(consumers_, *parent_, *this, sequence);
    }

    virtual long64 waitFor(const long64 & sequence, const long & timeout_micros) {
        return parent_->waitStrategy_.waitFor(consumers_, *parent_, *this, sequence, timeout_micros);
    }


    //@Override
    virtual Entry * getEntry(const long64 & sequence) {
        return &(parent_->entries_[(int)sequence & parent_->ringModMask_]);
    }

        //Override
    virtual long64 getCursor() {
        return parent_->cursor_;
    }

private:
    vector<Consumer*> consumers_;
    RingBuffer<Entry> * parent_;
};


/**
 * Abstraction of Consumer
 */
class Consumer {
public:
    /**
     * Get the sequence up to which this Consumer has consumed
     *
     * @return the sequence of the last consumed entry
     */
    virtual long64 getSequence() const = 0;

    /**
     * Signal that this Consumer should stop when it has finished consuming at the next clean break.
     */
    virtual void stop() = 0;

    /**
     * Thread entry function
     */
    virtual void run() = 0;
};


/**
 * Callback interface to be implemented for processing AbstractEntrys as they become available in the RingBuffer
 */
template <class Entry>
class BatchHandler {
public:
    virtual ~BatchHandler() {
    }

    /**
     * Called when BatchConsumer pick one Entry from RingBuffer for processing
     *
     * @param entry to be processed
     * @throws Exception if the BatchHandler would like the exception handled further up the chain.
     */
    virtual void onAvailable(Entry & entry) = 0;
  
    /**
     * Called after each batch of items has been have been processed before the next waitFor call on a ConsumerBarrier.
     * <p>
     * This can be taken as a hint to do flush type operations before waiting once again on the ConsumerBarrier.
     * The user should not expect any pattern or frequency to the batch size.
     * It's very easy to implement a fixed batch by leveraging this notification.
     *
     * @throws Exception if the BatchHandler would like the exception handled further up the chain.
    */
    virtual void onEndOfBatch() = 0;
};

/**
 * 
 */
template <class Entry>
class BatchConsumer : public Consumer {
private:
    ConsumerBarrier<Entry> * consumerBarrier_;
    BatchHandler<Entry>    * handler_;

    long64 p1, p2, p3, p4, p5, p6, p7;  // cache line padding
    volatile bool running_;
    long64 p8, p9, p10, p11, p12, p13, p14; // cache line padding
    volatile long64 sequence_;
    long64 p15, p16, p17, p18, p19, p20; // cache line padding
public:
    /**
    * Construct a batch consumer that will automatically track the progress by updating its sequence when
    * the BatchHandler#onAvailable(AbstractEntry) method returns.
    *
    * @param consumerBarrier on which it is waiting.
    * @param handler is the delegate to which AbstractEntrys are dispatched.
    */
    BatchConsumer(ConsumerBarrier<Entry> & consumerBarrier, BatchHandler<Entry> & handler)
        : running_(true),
          sequence_(RINGBUFFER_INITIAL_CURSOR_VALUE),
          consumerBarrier_(&consumerBarrier),
          handler_(&handler) {
    }

    //@Override
    virtual long64 getSequence() const {
        return sequence_;
    }

    //@Override
    virtual void stop() {
        running_ = false;
        // consumerBarrier_->alert();
    }

    /**
     * Get the ConsumerBarrier the Consumer is waiting on.
     *
     * @return the barrier used by this Consumer.
     */
    virtual ConsumerBarrier<Entry> & getConsumerBarrier() {
        return *consumerBarrier_;
    }

    //Override
    virtual void run() {
        long64 nextSequence = sequence_ + 1 ;
        Entry * entry;
        while (running_) {
            try {
                long64 availableSequence = consumerBarrier_->waitFor(nextSequence, 300);  // locally cached write position
                if (availableSequence < nextSequence) {
                    Sleep(1);
                    continue;  // timeout occured
                }
                if (availableSequence > nextSequence + 100)
                    availableSequence = nextSequence + 100;
                for (; nextSequence <= availableSequence; nextSequence++) {
                    entry = consumerBarrier_->getEntry(nextSequence);
                    handler_->onAvailable(*entry);
                }

                // it's still safe to access the batch of events in the end-of-batch callback
                // since the sequence is still not released until the callback is invoked
                handler_->onEndOfBatch(); 
                sequence_ = entry->getSequence();
            }
            catch (std::exception ex) {
                // TODO to invoke exception handler
                sequence_ = entry->getSequence();
                nextSequence = entry->getSequence() + 1;
            }
        } // end while loop
    }
};


/**
 *  RingBuffer class
 */
template <class Entry>
class RingBuffer {
    friend class ConsumerTrackingConsumerBarrier<Entry>;
    friend class ConsumerTrackingProducerBarrier<Entry>;
private:
    long int _p1_, _p2_, _p3_, _p4_, _p5_, _p6_, _p7_; // cache line padding
    volatile long64 cursor_;
    long int _p8_, _p9_, _p10_, _p11_, _p12_, _p13_, _p14_; // cache line padding

    std::vector<Entry> entries_;
    int ringModMask_;

    ClaimStrategy & claimStrategy_;
    WaitStrategy<Entry> & waitStrategy_;
    const ClaimStrategyOptions::Option claimStrategyOption_;
    const WaitStrategyOptions::Option waitStrategyOption_;

public:
    /**
     * Construct a RingBuffer
     * with default setting, only support single publisher, use YIELDING wait strategy
     *
     * @param size  the number of entries to be allocated in this RingBuffer
     */
    RingBuffer(const int & size) : 
        cursor_(RINGBUFFER_INITIAL_CURSOR_VALUE) ,
        claimStrategyOption_(ClaimStrategyOptions::SINGLE_THREADED),
        waitStrategyOption_(WaitStrategyOptions::YIELDING),
        claimStrategy_(* ClaimStrategy::newInstance(ClaimStrategyOptions::SINGLE_THREADED, cursor_)), 
        waitStrategy_ (* WaitStrategy<Entry>::newInstance(WaitStrategyOptions::YIELDING)),
        entries_(ceilingNextPowerOfTwo(size)) {
        int sizeAsPowerOfTwo = ceilingNextPowerOfTwo(size);
        ringModMask_ = sizeAsPowerOfTwo - 1;
    }

    /**
     * Destruct a RingBuffer
     */
    virtual ~RingBuffer() {
        delete &claimStrategy_ ;
        delete &waitStrategy_;
    }

    /**
     * Create a ConsumerBarrier that gates on the RingBuffer
     * and a list of Consumers
     *
     * @param consumersToTrack this barrier will track
     * @return the barrier gated as required
     */
    ConsumerBarrier<Entry> * createConsumerBarrier(vector<Consumer*> consumersToTrack) {
        return new ConsumerTrackingConsumerBarrier<Entry>(this, consumersToTrack);
    }

    /**
     * Create a ProducerBarrier on this RingBuffer that tracks dependent Consumers.
     *
     * @param consumersToTrack to be tracked to prevent wrapping.
     * @return a ProducerBarrier as required
     */
    ProducerBarrier<Entry> * createProducerBarrier(vector<Consumer*> consumersToTrack) {
        return new ConsumerTrackingProducerBarrier<Entry>(this, consumersToTrack);
    }

    /**
     * Get number of entries allocated in this RingBuffer
     *
     * @return the capacity
     */
    int getCapacity() {
        return (int) entries_.size();
    }

    /**
     * Get current write position
     *
     * @return the position
     */
    long64 getCursor() {
        return cursor_;
    }

    /**
     * Accept an entry via its sequence index
     *
     * @return the Entry
     */
    virtual Entry & getEntry(const long64 & sequence) {
        return entries_[(int)sequence & ringModMask_];
    }
};

#endif