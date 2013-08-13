#ifndef _WINDOW_H
#define _WINDOW_H

#ifdef _DEBUG
// windows memleak detect facility
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

/**
 *  Base Window Iterator
 *  each window should implement an associate iterator
 */
class WindowIterator {
public:
    WindowIterator() {}
    /**
     * Copy constructor
     * must supply overload version in subclasses, and call the version inherrited from parents'     
     */
    WindowIterator (const WindowIterator& iter) {};
    
    virtual ~WindowIterator() {};

    /**
     * Overload assignment operator
     * Must supply overload version in subclasses, and call the version inherrited from parents'
     */
    virtual WindowIterator& operator =(const WindowIterator& rhs) {return *this;}

    virtual WindowIterator& operator ++() = 0 ;
    virtual WindowIterator& operator --() = 0;
    virtual WindowIterator& operator ++(int) = 0 ;
    virtual WindowIterator& operator --(int) = 0;
    
    virtual bool operator ==(const WindowIterator& rhs) const { return false; }
    virtual bool operator !=(const WindowIterator& rhs) const { return false; }
    virtual Event* operator*() = 0;
};



/**
 * Sliding Count Window operator interface
 */
class SlidingCountWindowOperator : public BaseOperator {
public:
    /**
     * Sliding Count Window iterator
     */
    class SlidingCountWindowIterator : public WindowIterator {
    public:
        SlidingCountWindowIterator(deque<Event*>::iterator iter) : it_(iter) {
        }

        SlidingCountWindowIterator(const SlidingCountWindowIterator& iter) : WindowIterator(iter), it_(iter.it_) {
        }

        virtual SlidingCountWindowIterator& operator =(const SlidingCountWindowIterator& rhs) {
            it_ = rhs.it_;
            return *this;
        }

        virtual SlidingCountWindowIterator& operator ++() {
            ++it_;
            return *this;
        }

        virtual SlidingCountWindowIterator& operator --() {
            --it_;
            return *this;
        }


        virtual SlidingCountWindowIterator& operator ++(int) {
            it_++;
            return *this;
        }

        virtual SlidingCountWindowIterator& operator --(int) {
            it_--;
            return *this;
        }

        virtual bool operator ==(const WindowIterator& rhs) const {
            const SlidingCountWindowIterator& rhs2 = *(reinterpret_cast<const SlidingCountWindowIterator*> (&rhs));
            return (it_ == rhs2.it_);
        }

        virtual bool operator !=(const WindowIterator& rhs) const {
            const SlidingCountWindowIterator& rhs2 = *(reinterpret_cast<const SlidingCountWindowIterator*> (&rhs));
            return (it_ != rhs2.it_);
        }

        virtual Event* operator*() {
            return (*it_);
        }
    private:
        deque<Event*>::iterator it_;
    };
public:
    SlidingCountWindowOperator(long windowSize, long slideStep) : strValue_("SlidingCountWindow"), windowSize_(windowSize), slideStep_(slideStep) {        
        needBuffer_ = true;   // window operator will buffer up events 
    }

    virtual ~SlidingCountWindowOperator() {
    }

    // @override
    virtual BaseOperator* clone() {
        BaseOperator* clonedOperator = new SlidingCountWindowOperator(windowSize_, slideStep_);
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    // @ovverride
    virtual string& toString() {
        return strValue_ ;
    }

    // @override
    virtual void execute(long64 seq, Event* pevent) {
        // pass through control events
        if (!(pevent->isDataOrSyncEvent())) {
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);

                 if (seq % 100000 == 0) {
                    tempFlushEvent_.update(NULL, Event::TYPE_FLUSH, pevent->getTimestamp());
                    pNextOperator_->execute(seq, &tempFlushEvent_);
                 }
            }
            return;
        }

        // add the event into this window
        addEvent(pevent);

        // process out events
        while (!outs_.empty()) {
            // form IntermeidateEvents, change type to TYPE_MINUS, timestamp to use the new incomming event timestamp
            Event* theEvent = outs_.front();  // this is the original PLUS event stored in window            
            tempOutEvent_.setEvent(theEvent->getPayload(), Event::TYPE_MINUS, pevent->getTimestamp());
            if (pNextOperator_) {
                
                /* note: not like other operators,  in window operator, copy event will never happen,
                   as it alwasy happen in front of window operator, thus we can skip event copy check here */
                
                pNextOperator_->execute(seq, &tempOutEvent_);
            }
            // remove and dereference the original event
            theEvent->decref();
            outs_.pop_front();
        }

        // process the incomming event and then send a flush event
        if (pNextOperator_) {
            pNextOperator_->execute(seq, pevent);
            if (seq % 200000 == 0) {
                tempFlushEvent_.update(NULL, Event::TYPE_FLUSH, pevent->getTimestamp());
                pNextOperator_->execute(seq, &tempFlushEvent_);
            }
        }      
    }


    /**
     * Add one event into the window
     * this method incref the event and so pin it in memory,
     * this method will also calc output events
     * 
     * @param pEvent  the event to be added
     */
    virtual void addEvent(Event* pEvent){
        // only add PLUS or SYNC event into window
        if (pEvent->getType() == Event::TYPE_PLUS ||
            pEvent->getType() == Event::TYPE_SYNC ) {
            events_.push_back(pEvent);
            pEvent->incref();

            long curSize = (long) events_.size();
            while (curSize > windowSize_) {
                // move one step
                long l =0;
                Event* theOutEvent;
                for (; l< slideStep_; l++) {
                    theOutEvent = events_.front();
                    if (theOutEvent->getType() != Event::TYPE_SYNC) {
                        outs_.push_back(theOutEvent);                        
                    }
                    else {
                        theOutEvent->decref();  // release the event here
                    }
                    events_.pop_front();
                    if(events_.empty())
                        break;
                }
                curSize -= l;
            }
        }
        else if (pEvent->getType() == Event::TYPE_MINUS) {
            // this will happen to downstream window operators when multiple windows exist in the pipeline (the only case is compositive window)
            // only need to erase the event and decref it, and then it will be directly passed to next operator to process
            removeEvent(pEvent);
        }
    }

    /**
     * decref an event and remove it from the window
     * note: this method will invalidate the Window iterator
     */
    virtual void removeEvent(Event* pEvent) {
        for (deque<Event*>::iterator iter = events_.begin(); iter < events_.end(); iter++) {
            if (*iter == pEvent) {
                events_.erase(iter);
                pEvent->decref();
                break;
            }
        }
    }

    virtual SlidingCountWindowIterator begin() {
        SlidingCountWindowIterator iter(events_.begin());
        return iter;
    }

    virtual SlidingCountWindowIterator end() {
        SlidingCountWindowIterator iter(events_.end());
        return iter;
    }    

private:
    string strValue_;
    long windowSize_;        // window size
    long slideStep_;         // window sliding step

    deque<Event*> events_;   // the container for events inside window
    deque<Event*> outs_;     // the container for events moving out of the window

    IntermediateEvent tempOutEvent_;  // temp object used for sending OUT events to downstream
    Event tempFlushEvent_;            // temp object used for sending FLUSH events to downstream
};


/**
 * Sliding Time based Window operator interface
 */
class SlidingTimeWindowOperator : public BaseOperator {
public:
    /**
     * Sliding Time Window iterator
     */
    class SlidingTimeWindowIterator : public WindowIterator {
    public:
        SlidingTimeWindowIterator(deque<Event*>::iterator iter) : it_(iter) {
        }

        SlidingTimeWindowIterator(const SlidingTimeWindowIterator& iter) : WindowIterator(iter), it_(iter.it_) {
        }

        virtual SlidingTimeWindowIterator& operator =(const SlidingTimeWindowIterator& rhs) {
            it_ = rhs.it_;
            return *this;
        }

        virtual SlidingTimeWindowIterator& operator ++() {
            ++it_;
            return *this;
        }

        virtual SlidingTimeWindowIterator& operator --() {
            --it_;
            return *this;
        }


        virtual SlidingTimeWindowIterator& operator ++(int) {
            it_++;
            return *this;
        }

        virtual SlidingTimeWindowIterator& operator --(int) {
            it_--;
            return *this;
        }

        virtual bool operator ==(const WindowIterator& rhs) const {
            const SlidingTimeWindowIterator& rhs2 = *(reinterpret_cast<const SlidingTimeWindowIterator*> (&rhs));
            return (it_ == rhs2.it_);
        }

        virtual bool operator !=(const WindowIterator& rhs) const {
            const SlidingTimeWindowIterator& rhs2 = *(reinterpret_cast<const SlidingTimeWindowIterator*> (&rhs));
            return (it_ != rhs2.it_);
        }

        virtual Event* operator*() {
            return (*it_);
        }
    private:
        deque<Event*>::iterator it_;
    };
public:
    SlidingTimeWindowOperator(long64 windowSize, long64 slideStep) :  strValue_("SlidingTimeWindow"), windowSize_(windowSize), slideStep_(slideStep) {
        needBuffer_ = true;   // window operator will buffer up events 
    }

    virtual ~SlidingTimeWindowOperator() {
    }

    // @override
    virtual BaseOperator* clone() {
        BaseOperator* clonedOperator = new SlidingTimeWindowOperator(windowSize_, slideStep_);
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    // @ovverride
    virtual string& toString() {
        return strValue_ ;
    }

    //@override
    // to get 
    virtual void setExecutorContext(ExecutorContext* ctx) {
        BaseOperator::setExecutorContext(ctx);
        isSyncByInterval_ = ctx->getExecutor()->isSyncByInterval();
    }

    // @override
    virtual void execute(long64 seq, Event* pevent) {
        // pass through control events
        if (!(pevent->isDataOrSyncEvent())) {
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);

                 if (isSyncByInterval_ && (seq % 200000 == 0)) {
                    tempFlushEvent_.update(NULL, Event::TYPE_FLUSH, pevent->getTimestamp());
                    pNextOperator_->execute(seq, &tempFlushEvent_);
                 }
            }
            return;
        }

        // add the event into this window
        addEvent(pevent);

        // process out events
        while (!outs_.empty()) {
            // form IntermeidateEvents, change type to TYPE_MINUS, timestamp to use the new incomming event timestamp
            Event* theEvent = outs_.front();  // this is the original PLUS event stored in window            
            tempOutEvent_.setEvent(theEvent->getPayload(), Event::TYPE_MINUS, theEvent->getTimestamp() + windowSize_);
            if (pNextOperator_) {
                
                /* note: not like other operators,  in window operator, copy event will never happen,
                   as it alwasy happen in front of window operator, thus we can skip event copy check here */
                
                pNextOperator_->execute(seq, &tempOutEvent_);
                if ( !isSyncByInterval_ ) {
                    tempFlushEvent_.update(NULL, Event::TYPE_FLUSH, tempOutEvent_.getTimestamp());
                    pNextOperator_->execute(seq, &tempFlushEvent_);
                }

            }
            // remove and dereference the original event
            theEvent->decref();
            outs_.pop_front();
        }

        // process the incomming event and then send a flush event
        if (pNextOperator_) {
            pNextOperator_->execute(seq, pevent);
             if (!isSyncByInterval_ || (seq % 200000 == 0)) {
                tempFlushEvent_.update(NULL, Event::TYPE_FLUSH, pevent->getTimestamp());
                pNextOperator_->execute(seq, &tempFlushEvent_);
            }
        }
    }

    /**
     * Add one event into the window
     * this method incref the event and so pin it in memory,
     * this method will also calc output events
     * 
     * @param pEvent  the event to be added
     */
    virtual void addEvent(Event* pEvent){
        // only add PLUS or SYNC event into window
        if (pEvent->getType() == Event::TYPE_PLUS ||
            pEvent->getType() == Event::TYPE_SYNC ) {
            events_.push_back(pEvent);
            pEvent->incref();
            
            long64 tFront = events_.front()->getTimestamp();            
            long64 tBack = events_.back()->getTimestamp();
            long64 tCandidateFront = (tBack - windowSize_) / slideStep_ * slideStep_;
            long64 tTargetWindowFront = (tCandidateFront + windowSize_ > tBack) ? tCandidateFront : tCandidateFront + slideStep_ ;
            // only allow events between [tTargetWindowFront, tTargetWindowFront + slideStep_) stay in the window 
            while (tFront < tTargetWindowFront) {
                Event* theOutEvent = events_.front();
                if (theOutEvent->getType() != Event::TYPE_SYNC) {
                    outs_.push_back(theOutEvent);
                }
                else {
                    // release the event here
                    theOutEvent->decref();  
                }
                events_.pop_front();

                // no need to do empty check, it should not happen
                //if(events_.empty())
                //    break;
                tFront = events_.front()->getTimestamp();   
            }
        }
        else if (pEvent->getType() == Event::TYPE_MINUS) {
            // this will happen to downstream window operators when multiple windows exist in the pipeline (the only case is compositive window)
            // only need to erase the event and decref it, and then it will be directly passed to next operator to process
            removeEvent(pEvent);
        }
    }

    /**
     * decref an event and remove it from the window
     * note: this method will invalidate the Window iterator
     */
    virtual void removeEvent(Event* pEvent) {
        for (deque<Event*>::iterator iter = events_.begin(); iter < events_.end(); iter++) {
            if (*iter == pEvent) {
                events_.erase(iter);
                pEvent->decref();
                break;
            }
        }
    }
private:
    string strValue_;        // operator name
    long64 windowSize_;      // window size in microseconds(us)
    long64 slideStep_;       // window sliding step in microseconds(us)

    deque<Event*> events_;   // the container for events inside window
    deque<Event*> outs_;     // the container for events moving out of the window

    IntermediateEvent tempOutEvent_;  // temp object used for sending OUT events to downstream
    Event tempFlushEvent_;            // temp object used for sending FLUSH events to downstream
    bool isSyncByInterval_;           // control the sync frequence, obtain the value setExecutorContext() 
};

#endif