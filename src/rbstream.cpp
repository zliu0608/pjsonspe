#include "tokendef.h"
#include "rbstream.h"

RbSourceStream::SubscriberHandler::SubscriberHandler(AbstractCommandsExecutor* pQueryExecutor, StreamOperator* pInvokePoint) :
    pQueryExecutor_(pQueryExecutor),
    pInvokePoint_(pInvokePoint),
    index_(pInvokePoint->getParallelIndex()),
    nParallelism_(pInvokePoint->getParallelism()){
        num_ = 0;
}

void RbSourceStream::SubscriberHandler::onAvailable(EventBufferEntry & entry) {
    // execute this query
    // TODO : need to prevent multiple subscribers from executing the query concurrently
    static const long TRACE_EVENT_NUM = 1000000;

    long seq  = entry.getSequence();
    // exploit a hardcoded roundrobin partition strategy
    if (nParallelism_ > 1) {        
        if ((seq/10) % nParallelism_ != index_)
            return; 
    }

    // parse the event
    // update the entry with the event content on this entry.
    const char* stringText = entry.jsonText_->c_str();
    int r = entry.event_->update(stringText);
    assert (0 == r);

    Event* pEvent =  entry.event_;
    double duration;
    if (pEvent) {
        if (num_ == 0) 
            t2_ =  clock();

        pEvent->incref();
        pInvokePoint_->execute(seq, pEvent);
        pEvent->decref();
        num_++;

        if (num_ % TRACE_EVENT_NUM == 0) {
            t1_ = t2_;
            t2_ = clock();
            duration = (t2_-t1_);
            duration /= CLOCKS_PER_SEC;
            printf("[Instance %2d] %ld [%9ld ~ %9ld] : %6.2f sec.\n", index_, TRACE_EVENT_NUM, num_-TRACE_EVENT_NUM, num_, duration );
        }

    }
}



RbSourceStream::CompositeSubscriberHandler::CompositeSubscriberHandler(AbstractCommandsExecutor* pQueryExecutor, SourceStreamOperatorGroupList* pInvokePoints) :
    pQueryExecutor_(pQueryExecutor),
    pInvokePoints_(pInvokePoints),
    index_(pInvokePoints->getParallelIndex()),
    syncEvent_(NULL, Event::TYPE_SYNC, 0),
    nParallelism_(pInvokePoints->getParallelism()){
        num_ = 0;
}

void RbSourceStream::CompositeSubscriberHandler::onAvailable(EventBufferEntry & entry) {
    // execute this query
    // TODO : need to prevent multiple subscribers from executing the query concurrently
    static const long TRACE_EVENT_NUM = 1000000;

    long seq  = entry.getSequence();

    SourceStreamOperatorGroup* theGroup = pInvokePoints_->getGroup(entry.streamIdx_);
    int operatorsNum = (int) theGroup->operators.size();

    // exploit a hardcoded roundrobin partition strategy
    if ( nParallelism_ == 1 || 
        (seq/10) % nParallelism_ == index_) {
        // parse the event
        // update the entry with the event content on this entry.
        const char* stringText = entry.jsonText_->c_str();
        int r = entry.event_->update(stringText);
        assert (0 == r);

        Event* pEvent =  entry.event_;
        double duration;
        if (pEvent) {
            if (num_ == 0) 
                t2_ =  clock();

            pEvent->incref();
            
            // run this event from each source stream operator entry point (fan-out case)
            // SourceStreamOperatorGroup* theGroup = pInvokePoints_->getGroup(entry.streamIdx_);
            // int operatorsNum = (int) theGroup->operators.size();
            for (int i = 0; i<operatorsNum; i++ ) {
                theGroup->operators[i]->execute(seq, pEvent);
            }
            
            pEvent->decref();
            num_++;

            if (num_ % TRACE_EVENT_NUM == 0) {
                t1_ = t2_;
                t2_ = clock();
                duration = (t2_-t1_);
                duration /= CLOCKS_PER_SEC;
                printf("[Instance %2d] %ld [%9ld ~ %9ld] : %6.2f sec.\n", index_, TRACE_EVENT_NUM, num_-TRACE_EVENT_NUM, num_, duration );
            }
        }
    }
    else {

        //send the sync event in parallel execution case        
        syncEvent_.update(NULL, Event::TYPE_SYNC, entry.event_->getTimestamp());
        for (int i = 0; i<operatorsNum; i++ ) {
            theGroup->operators[i]->execute(seq, & syncEvent_);
        }
        
    }
}
