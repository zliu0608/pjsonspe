#ifndef _RB_STREAM_H
#define _RB_STREAM_H

#include "event.h"
#include "fcthread/fcthread.h"
#include "ringbuffer.h"
#include <assert.h>
#include <string>
#include <vector>
#include <time.h>

using namespace std;

class Event;
class StreamOperator;
class AbstractCommandsExecutor;
struct SourceStreamOperatorGroupList;

/**
 * Entry class to be filled in the source stream RingBuffer
 */
class EventBufferEntry : public AbstractEntry {  
public:
    string* jsonText_;
    Event* event_;
    int streamIdx_;
};


class Watcher : public BatchHandler<EventBufferEntry> {
public:
        virtual void onAvailable(EventBufferEntry & entry) {
            doWatch(entry.getSequence());
        }

        virtual void doWatch(long64 sequence) = 0;

        virtual void onEndOfBatch() {
            // do nothing for now.
        }
};

/**
 * Ring buffer backed source stream implementation
 * you can specify the size in the construtor
 */
class RbSourceStream {
public:
    RbSourceStream(const string& name, int size) : 
      name_(name),
      running_(false),
      buffer_(size),
      emptyConsumerList_(0),
      pEmptyBarrier_(NULL), 
      producer_ (NULL),
      producerInitialized_(false), 
      pConsumersBarrier_ (NULL) {
      pEmptyBarrier_ = buffer_.createConsumerBarrier(emptyConsumerList_);

      // fill up the ring buffer with empty events
      for (int i=0; i<buffer_.getCapacity(); i++) {
        buffer_.getEntry(i).event_ = new Event();
        string* str = new string(); 
        str->reserve(256);
        *str = "";
        buffer_.getEntry(i).jsonText_  = str;
      }
    }

    virtual ~RbSourceStream() {
        // stop and then delete all subscribers
        stop();
        for (vector<SubscriberThread*>::const_iterator iter = subscribers_.begin(); 
            iter < subscribers_.end();
            ++iter) {
            delete (*iter);
        }
        
        if (producer_) {
            delete producer_;
        }

        if (pEmptyBarrier_)
            delete pEmptyBarrier_;

        if (pConsumersBarrier_)
            delete pConsumersBarrier_;

        // release memory occupied by the events
        for (int i=0; i<buffer_.getCapacity(); i++) {
            delete(buffer_.getEntry(i).event_);
            delete(buffer_.getEntry(i).jsonText_);
        }        
    }


    /**
     * add one subscriber
     * it's only allowed when the stream is in stop state    
     *
     * @param pQueryExecutor a executor of which will in turn process events for the subcriber
     * @param pInvokePoint the Stream operator
     * @return 0 on success, otherwise on failure
     */
    int addSubscriber(AbstractCommandsExecutor* pQueryExecutor, StreamOperator* pInvokePoint) {
        if (running_ || producerInitialized_)
            return -1;
        
        subscribers_.push_back(new SubscriberThread(this, pQueryExecutor, pInvokePoint));
        return 0;
    }

    /**
     * add one subscriber
     * it's only allowed when the stream is in stop state    
     *
     * @param pQueryExecutor a executor of which will in turn process events for the subcriber
     * @param pInvokePoints a group of streams within a EPN
     * @return 0 on success, otherwise on failure
     */
    int addSubscriber(AbstractCommandsExecutor* pQueryExecutor, SourceStreamOperatorGroupList* pInvokePoints) {
        if (running_ || producerInitialized_)
            return -1;
        
        subscribers_.push_back(new SubscriberThread(this, pQueryExecutor, pInvokePoints));
        return 0;
    }


    int addSubscribersWatcher(Watcher* pWatcher) {
        if (running_ || producerInitialized_)
            return -1;
        
        if (consumerList_.empty()) {
            for (vector<SubscriberThread*>::const_iterator iter = subscribers_.begin(); 
                iter < subscribers_.end();
                ++iter) {
                consumerList_.push_back((*iter)->getConsumer());
            }
            // init the barrier on all subscriber consumers
            pConsumersBarrier_ = buffer_.createConsumerBarrier(consumerList_);
        }
        
        watchers_.push_back(new WatcherThread(this, pWatcher));  
        return 0;
    }

    /**
     * Create producer from this ring buffer
     * after call this method, no more subscribers can be added.
     *
     */
    void initProducer() {
        if (producerInitialized_)
            return;

        if (watchers_.empty()) {
            for (vector<SubscriberThread*>::const_iterator iter = subscribers_.begin(); 
                iter < subscribers_.end();
                ++iter) {
                consumerList_.push_back((*iter)->getConsumer());
            }
            producer_ = buffer_.createProducerBarrier(consumerList_);
        }
        else {
            for (vector<WatcherThread*>::const_iterator iter = watchers_.begin(); 
                iter < watchers_.end();
                ++iter) {
                    watcherConsumerList_.push_back((*iter)->getConsumer()) ;
            }
            producer_ = buffer_.createProducerBarrier(watcherConsumerList_);
        }
        producerInitialized_ = true;
    }

    /**
     * publish one event with system time
     *
     * @param stringEvent the string content of event
     * @return the number of event being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publish(char* stringEvent) {
        if ( running_ && producerInitialized_) {
            EventBufferEntry* entry = producer_->nextEntry();
            // update the entry with the event content to be pulished.
            // int r = entry->event_->update(stringEvent);
            // assert (0 == r);
            *(entry->jsonText_) = stringEvent;
            producer_->commit(*entry);
            return 1;
        }
        else {
            return -1;
        }
    }

    /**
     * publish one event with system time on to the specified source stream
     *
     * @param streamId the id of source stream where events should be published through
     * @param stringEvent the string content of event
     * @return the number of event being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publish(int streamId, char* stringEvent) {
        if ( running_ && producerInitialized_) {
            EventBufferEntry* entry = producer_->nextEntry();
            // update the entry with the event content to be pulished.
            // int r = entry->event_->update(stringEvent);
            // assert (0 == r);
            *(entry->jsonText_) = stringEvent;
            entry->streamIdx_ = streamId;
            producer_->commit(*entry);
            return 1;
        }
        else {
            return -1;
        }
    }

    /**
     * publish one batch of event with system time
     *
     * @param streamId the id of source stream where events should be published through
     * @param events the array of string event
     * @param num size of the batch
     * @return the number of events being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publish(int streamId, char** events, int num ) {
        if ( running_ && producerInitialized_) {
            SequenceBatch sequenceBatch(num);
            SequenceBatch returnedBatch = producer_->nextEntries(sequenceBatch);
            EventBufferEntry* entry;
            char* stringEvent;
            int index = 0;
            for (long64 i = returnedBatch.getStart(), end = returnedBatch.getEnd(); i <= end; i++) {
                entry = producer_->getEntry(i);
                stringEvent = events[index];
                entry->streamIdx_ = streamId;
                index++;
                *(entry->jsonText_) = stringEvent;
            }
            producer_->commit(returnedBatch);
            return returnedBatch.getSize();
        }
        else {
            return -1;
        }
    }

    /**
     * publish one batch of event with system time
     *
     * @param events the array of string event
     * @param num size of the batch
     * @return the number of events being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publish(char** events, int num ) {
        if ( running_ && producerInitialized_) {
            SequenceBatch sequenceBatch(num);
            SequenceBatch returnedBatch = producer_->nextEntries(sequenceBatch);
            EventBufferEntry* entry;
            char* stringEvent;
            int index = 0;
            for (long64 i = returnedBatch.getStart(), end = returnedBatch.getEnd(); i <= end; i++) {
                entry = producer_->getEntry(i);
                stringEvent = events[index];
                index++;
                *(entry->jsonText_) = stringEvent;
            }
            producer_->commit(returnedBatch);
            return returnedBatch.getSize();
        }
        else {
            return -1;
        }
    }

    /**
     * start the stream and its subscribers
     */
    void start() {
        if ( running_ )
            return;

        for (vector<WatcherThread*>::const_iterator iter = watchers_.begin(); 
            iter < watchers_.end();
            ++iter) {
            (*iter)->start();
        }

        for (vector<SubscriberThread*>::const_iterator iter = subscribers_.begin(); 
            iter < subscribers_.end();
            ++iter) {
            (*iter)->start();
        }
        running_ = true;
    }

    /**
     * stop the stream and its subscribers
     */
    void stop() {
        if (! running_ )
            return;

        running_ = false;        
        for (vector<SubscriberThread*>::const_iterator iter = subscribers_.begin(); 
            iter < subscribers_.end();
            ++iter) {
            (*iter)->stop();
        }
        for (vector<SubscriberThread*>::const_iterator iter = subscribers_.begin(); 
            iter < subscribers_.end();
            ++iter) {
            (*iter)->join();
        }

        for (vector<WatcherThread*>::const_iterator iter = watchers_.begin(); 
            iter < watchers_.end();
            ++iter) {
            (*iter)->stop();
        }
        for (vector<WatcherThread*>::const_iterator iter = watchers_.begin(); 
            iter < watchers_.end();
            ++iter) {
            (*iter)->join();
        }
    }

    const string& getName() {
        return name_;
    }

private:
    /**
     * Subscriber callback handler
     * it is used in the case :  one subscriber per source stream operator within one parallel EPN instance
     */
    class SubscriberHandler : public BatchHandler<EventBufferEntry> {
    public:
        SubscriberHandler(AbstractCommandsExecutor* pQueryExecutor, StreamOperator* pInvokePoint);

        virtual ~SubscriberHandler() {};

        /*
         * a callback function, will be invoked when an event is subscribed
         */
        virtual void onAvailable(EventBufferEntry & entry);

        virtual void onEndOfBatch() {
            // do nothing for now.
        }
    private:
        AbstractCommandsExecutor* pQueryExecutor_;
        StreamOperator* pInvokePoint_;
        int index_;
        int nParallelism_;

        long64 num_;         // for performance tracing purpose.
        clock_t t1_;
        clock_t t2_;
    };


    /**
     * CompositeSubscriber callback handler
     * It is used in the case: one subscriber per one EPN parallel instance
     */
    class CompositeSubscriberHandler : public BatchHandler<EventBufferEntry> {
    public:
        CompositeSubscriberHandler(AbstractCommandsExecutor* pQueryExecutor, SourceStreamOperatorGroupList* pInvokePoints);

        virtual ~CompositeSubscriberHandler() {};

        /*
         * a callback function, will be invoked when an event is subscribed
         */
        virtual void onAvailable(EventBufferEntry & entry);

        virtual void onEndOfBatch() {
            // do nothing for now.
        }
    private:
        AbstractCommandsExecutor* pQueryExecutor_;
        SourceStreamOperatorGroupList* pInvokePoints_;
        int index_;
        int nParallelism_;

        long64 num_;         // for performance tracing purpose.
        clock_t t1_;
        clock_t t2_;

        Event syncEvent_;  // event used for sync patitioned windows
    };


    /**
     * Inner subscriber class
     */
    class SubscriberThread : public fc::Thread {
    public:
        /**
         * Construct a subscriber (thread)
         */
        SubscriberThread(RbSourceStream* parent, AbstractCommandsExecutor* pQueryExecutor, StreamOperator* pInvokePoint) :
            parent_(parent), 
            pHandler_(new SubscriberHandler(pQueryExecutor, pInvokePoint)),
            pConsumer_(NULL) {
            pConsumer_ = new BatchConsumer<EventBufferEntry>(*(parent_->pEmptyBarrier_), *pHandler_);
        }

        /**
         * Construct a subscriber (thread) with a CompositeSubscriberHandler
         */
        SubscriberThread(RbSourceStream* parent, AbstractCommandsExecutor* pQueryExecutor, SourceStreamOperatorGroupList* pInvokePoints) :
            parent_(parent), 
            pHandler_(new CompositeSubscriberHandler(pQueryExecutor, pInvokePoints)),
            pConsumer_(NULL) {
            pConsumer_ = new BatchConsumer<EventBufferEntry>(*(parent_->pEmptyBarrier_), *pHandler_);
        }


        /**
         * destruct a subscriber object
         */
        virtual ~SubscriberThread() {
            if (pConsumer_)
                delete pConsumer_;

            if (pHandler_)
                delete pHandler_;
        }

        // override
        void run() {
            if (pConsumer_)
                pConsumer_->run();
        }

        /*
         * Stop a subscriber
         */
        void stop() {
            if (pConsumer_)
                pConsumer_->stop();
        }

        BatchConsumer<EventBufferEntry>* getConsumer() {
            return pConsumer_;
        }
    private:
        RbSourceStream* parent_;
        // SubscriberHandler* pHandler_;
        BatchHandler<EventBufferEntry>* pHandler_; 
        BatchConsumer<EventBufferEntry>* pConsumer_;
    };


    // Inner WatcherThread class 
    class WatcherThread : public fc::Thread {
    public: 
        WatcherThread(RbSourceStream* parent, Watcher* pWatcher) : parent_(parent), pWatcher_(pWatcher), pConsumer_ (NULL)  {
            pConsumer_ = new BatchConsumer<EventBufferEntry>(*(parent_->pConsumersBarrier_), *pWatcher);
        }

        virtual ~WatcherThread() {
            if (pConsumer_)
                delete pConsumer_;
        }

        // override
        void run() {
            if (pConsumer_)
                pConsumer_->run();
        }

        /*
         * Stop a subscriber
         */
        void stop() {
            if (pConsumer_)
                pConsumer_->stop();
        }

        BatchConsumer<EventBufferEntry>* getConsumer() {
            return pConsumer_;
        }

    private:
        RbSourceStream* parent_;
        Watcher* pWatcher_;
        BatchConsumer<EventBufferEntry>* pConsumer_;
    };

private:
    string name_;
    bool running_;
    vector<SubscriberThread*> subscribers_;                 // subscribers
    RingBuffer<EventBufferEntry> buffer_;                   // ring buffer
    vector<Consumer *> emptyConsumerList_;                  // an empty list for creating a dummy barrier
    ConsumerBarrier<EventBufferEntry> * pEmptyBarrier_;     // a dummy barrier (no dependent consumers)
    
    vector<Consumer *> consumerList_;                       // consumers on the ring buffer of this stream
    bool producerInitialized_;                              // a flag to indicate whether producer has been initialized
    ProducerBarrier<EventBufferEntry> * producer_;          // producer barrier

    vector<WatcherThread*> watchers_;                       // watchers, each watcher depend on all subscriber consumers
    vector<Consumer *> watcherConsumerList_;                // a list of watchers' consumers
    ConsumerBarrier<EventBufferEntry> * pConsumersBarrier_; // a barrier for watchers
};


class RbSourceStreamManager {
public:
    RbSourceStreamManager() {
    };

    virtual ~RbSourceStreamManager() {
        for (vector<RbSourceStream*>::const_iterator iter = streams_.begin(); 
            iter < streams_.end();
            ++iter) {  
                delete (*iter);
        }
    }

    RbSourceStream* getStream(const string& name) {
        for (vector<RbSourceStream*>::const_iterator iter = streams_.begin(); 
            iter < streams_.end();
            ++iter) {  
                if (0 == (*iter)->getName().compare(name))
                    return (*iter);
        }

        // create a new stream object, add into this manager if not found
        // ring buffer size is very crucial to performance, better let it fit in L1 cache
        RbSourceStream* pNewStream = new RbSourceStream(name, 8000);
        streams_.push_back(pNewStream);
        return pNewStream;        
    }

    void removeStream(const string& name) {
        for (vector<RbSourceStream*>::const_iterator iter = streams_.begin(); 
            iter < streams_.end();
            ++iter) {  
                if (0 == (*iter)->getName().compare(name)) {
                    delete (*iter);
                    streams_.erase(iter);
                    return;
                }
        } 
    }

    void removeStream(RbSourceStream* stream) {
        for (vector<RbSourceStream*>::const_iterator iter = streams_.begin(); 
            iter < streams_.end();
            ++iter) {  
                if (stream == (*iter)) {
                    delete (*iter);
                    streams_.erase(iter);
                    return;
                }
        } 
    }
private:
    vector<RbSourceStream*> streams_;
};

// global variable
extern RbSourceStreamManager g_rbSSMgr;

#endif