#ifndef INT_EVENT_POOL_H
#define INT_EVENT_POOL_H

#ifdef _DEBUG
// windows memleak detect facility
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <unordered_map>
#include <deque>
#include <vector>
#include "event.h"

using namespace std;
using namespace std::tr1;


class InternalEvent;


/**
 * Abstract event pool interface
 * A event pool is used for managing and recycling internal event objects within a execution engine.
 */
class AbstractEventPool {
public:
    /**
     *  clear the event pool
     */
    virtual void clear() = 0;

    /**
     * allocate a event object and copy src content in
     */
    virtual InternalEvent* copyEvent(long seq, Event* srcEvent) = 0;

    /**
     * release a event object back to the pool
     */
    virtual bool freeEvent(long long eid) = 0;

    /**
     * release a event object back to the pool
     */
    virtual bool freeEvent(InternalEvent* pEvent) = 0;
};


/**
 * Event that needs to be buffered along the pipeline
 */
class InternalEvent : public Event {
public:
    InternalEvent(AbstractEventPool* eventPool, long long eid, long seq, Event* srcEvent) :
      pool_(eventPool),
      refCount_(0), 
      eid_(eid),
      seq_(seq),
      ts_(srcEvent->getTimestamp()),
      type_(srcEvent->getType()), 
      pPayload_(new Document()) {
        // copy event payload
       Value* srcPayload = srcEvent->getPayload();
       if (srcPayload) {
        //   srcPayload->CopyTo(*pPayload_);
       }
       update(eid, seq, srcEvent);
    }

    virtual ~InternalEvent() {
        if (pPayload_)
            delete pPayload_;
    }

    void update(long long eid, long seq, Event* srcEvent) {
        eid_ = eid;
        seq_ = seq;
        ts_ = srcEvent->getTimestamp();
        type_ = srcEvent->getType();

        // need to call destructor, otherwise reuse document object would cause memleak
        pPayload_->~Document();
        new(pPayload_) Document();

        Value* srcPayload = srcEvent->getPayload();
        if (srcPayload) {
           srcPayload->CopyTo(*pPayload_, true);
        }         
    }

    virtual void incref() {
        refCount_ ++;
    }

    virtual void decref() {
        refCount_ --;
        if (0 == refCount_ ) {
            pool_->freeEvent(eid_);
        }
    }

    long long getEventId() {
        return eid_;
    }

    virtual Value* getPayload() {
        return pPayload_;
    }

    virtual int64_t getTimestamp() {
        return ts_;
    }

    virtual EventType getType() {
        return type_;
    }

private:
    AbstractEventPool*   pool_;   // the event pool
    int refCount_;        // free back to event pool upon decref to 0
    long long eid_;       // unique id within a event pool
    long seq_;            // event sequence number, same as the corresponding input event sequence   

    Document* pPayload_;   // event payload
    int64_t ts_;          // event timestamp
    EventType type_;      // event type
};


/**
 * A pool implementation class
 * For multi-threaded execution engine, each thread should use its own pool
 */
class InternalEventPool : public AbstractEventPool {
public:
    InternalEventPool() : nextId_(0) {
    }

    virtual ~InternalEventPool() {
        clear();
    }

    /**
     *  Free all event objects
     */
    virtual void clear() {
        InternalEvent* theEvent;
        while (!freeEvents_.empty()) {
            theEvent = freeEvents_.back();
            delete theEvent;
            freeEvents_.pop_back();
        }

        for (InUseEventMap::const_iterator iter = inUseEvents_.begin(); iter != inUseEvents_.end(); iter++) {
            theEvent = iter->second;
            delete theEvent;
        }
        inUseEvents_.clear();
    }

    /**
     * From the event pool, retrieve a free event object if available, or allocate a new event object otherwise,
     * copy src event into this event object
     *
     * @param seq  source event sequence
     * @param srcEvent the source event object pointer
     */
    virtual InternalEvent* copyEvent(long seq, Event* srcEvent) {
        nextId_++;
        InternalEvent* theCopy;
        if (freeEvents_.empty()) {
            theCopy = new InternalEvent(this, nextId_, seq, srcEvent);
        }
        else {
            theCopy = freeEvents_.back();            
            theCopy->update(nextId_, seq, srcEvent);            
            freeEvents_.pop_back();
        }
        inUseEvents_.insert(InUseEventMap::value_type(nextId_, theCopy));
        return theCopy;
    }

    /**
     * release a in-use event object
     * 
     * @param eid the event id (which is unique within the pool)
     * @return true if the event id is in this pool, otherwise return false
     */
    virtual bool freeEvent(long long eid) {
        InUseEventMap::const_iterator iter = inUseEvents_.find(eid);
        if (iter == inUseEvents_.end()) {
            return false;
        }
        
        InternalEvent* theEvent = iter->second;
        inUseEvents_.erase(iter);
        freeEvents_.push_back(theEvent);
        return true;
    }

    virtual bool freeEvent(InternalEvent* pEvent) {
        return freeEvent(pEvent->getEventId());
    }
private:
    typedef unordered_map<unsigned long long, InternalEvent*> InUseEventMap; 
    unordered_map<unsigned long long, InternalEvent*> inUseEvents_;    // a map containing all events being referenced
    deque<InternalEvent*> freeEvents_;                                 // a deque containing all event objects can be reused or recycled
    unsigned long long nextId_;                                        // monotolically increasing event id
};

#endif