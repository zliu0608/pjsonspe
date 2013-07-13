#ifndef EVENT_SINK_H
#define EVENT_SINK_H

#include "event.h"
#include "fcthread/fcthread.h"
#include "ringbuffer.h"
#include <string>
#include <deque>

using namespace std;

/**
 * A entry to be stored in event sink
 */
class EventSinkEntry : public AbstractEntry {
public:
    EventSinkEntry() : type_(Event::TYPE_PLUS), ts_(0) {
        jsonText_.reserve(255);
        setSequence(RINGBUFFER_INITIAL_CURSOR_VALUE);
    }
public:
    string jsonText_;
    Event::EventType type_;
    int64_t ts_;
};



/**
 * The sink  will be used for temporarily storing result event objects during parallel process
 * Events stored in this sink is placed in storing order.
 */
class EventSink {
public:
    EventSink() {
        // preallocate some entries into the freeList
        for (int i = 0; i < 200; i++) {
            freeList_.push_back(new EventSinkEntry());
        }
    };

    virtual ~EventSink() {
        for (deque<EventSinkEntry*>::const_iterator iter = queue_.begin(); iter != queue_.end(); iter++) {
            delete (*iter);
        }
        queue_.clear();

        for (vector<EventSinkEntry*>::const_iterator iter = freeList_.begin(); iter < freeList_.end(); iter++) {
            delete (*iter);
        }
        freeList_.clear();
    }

    /**
     * Store a PLUS event into the sink
     *
     * @param seq  the sequence of this event
     * @param str  the json str content
     * @param ts   the timestamp
     * @param type event type, optional, default is a plus event
     */
    void storeEvent(long64 seq, const string& str, int64_t ts, Event::EventType type = Event::TYPE_PLUS ) {
        mutex_.lock();
        EventSinkEntry* pEntry;
        if (!freeList_.empty()) {
            pEntry = freeList_.back();
            freeList_.pop_back();            
        }
        else {
            pEntry = new EventSinkEntry();
        }
        pEntry->setSequence(seq);
        pEntry->jsonText_ = str;
        pEntry->type_ = type;
        pEntry->ts_ = ts;
        
        // only the queue needs to be protected
        // mutex_.lock();
        queue_.push_back(pEntry);
        mutex_.unlock();
    }

    /**
     * retrieve available events that their sequence is less or equal than the desired sequence
     * 
     * @param seq the desired sequence
     * @param events the available events to be returned
     * @return the number of events being returned
     */
    int getAvailableEvents(long64 seq, vector<EventSinkEntry*> & events) {
        events.clear();
        mutex_.lock();
        for (deque<EventSinkEntry*>::const_iterator iter = queue_.begin(); iter< queue_.end(); iter++) {
            if ((*iter)->getSequence() > seq) 
                break;
            events.push_back(*iter);
        }
        
        mutex_.unlock();
        return (int)events.size();
    }

    /**
     * Free events in the front of the queue, make them available in free list
     */
    void freeEvents(int num) {
        mutex_.lock();
        for (int i=0; i<num ; i++) {
            if (queue_.empty())
                break;
            freeList_.push_back(queue_.front());
            queue_.pop_front();            
        }
        mutex_.unlock();
    }

private:
    fc::Mutex mutex_;
    deque<EventSinkEntry*> queue_;
    vector<EventSinkEntry*> freeList_;
};

#endif