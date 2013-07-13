#ifndef _REDUCE_H
#define _REDUCE_H

#include <string>
#include <list>
#include <deque>
#include "fcthread/fcthread.h"
#include "inteventpool.h"
#include "group.h"

using namespace std;


class PartialAggregationBag {
public:    
    long64 seq_;
    int64_t ts_;
    deque<Event*> events_;
    InternalEventPool* pool_;

    PartialAggregationBag() : seq_(-1), ts_(0), pool_(new InternalEventPool()) {
    }

    ~PartialAggregationBag() {
        if (pool_)
            delete pool_;
        events_.clear();
    }

    void addEvent(Event* pevent) {
        InternalEvent* copied = pool_->copyEvent(seq_, pevent);
        events_.push_back(copied);
        copied->incref();    // it will be decref when the bag is reduced
    }

};

class AggregationReducer;
class ReduceOperator;

class PartialAggregationBagManager {
public:
    /**
     * Constructor
     */
    PartialAggregationBagManager(int index, AggregationReducer* reducer) : 
      index_(index),
      reducer_(reducer),
      readySeq_(-1),
      openBag_(NULL) {
    }

    /**
     * Destructor,  clean all bags, release events inside
     */
    ~PartialAggregationBagManager() {
        if (openBag_)
            delete openBag_;
        for (list<PartialAggregationBag*>::const_iterator iter = readyBags_.begin(); iter != readyBags_.end(); iter++) {
            delete (*iter);
        }
    }


    /**
     * open a new bag , make it as current bag for receiving events when no other bag is currently open
     * this method is supposed to be called by one thread only, in such no need use lock to protect
     *
     * @return the new bag on success,  NULL on failure which is usually because another bag is still in open state
     */
    PartialAggregationBag* openBag(long64 seq);

    /**
     * close current bag, add it into readyBags list, also update the readySeq
     * this method is protected by the locker in reducer 
     */
    void closeBag();

    /**
     * before calling this method, must aquire the lock first
     */
    PartialAggregationBag* getFirstReadyBag() {
        if (readyBags_.empty())
            return NULL;
        else
            return readyBags_.front();
    }

    /*
     * before calling this method, must aquire the lock first
     */
    void popFirstReadyBag() {
        if (!readyBags_.empty()) {
            readyBags_.pop_front();
        }
    }

private:
    int index_;                                // the index among all bag managers inside a reducer
    AggregationReducer* reducer_;              // point to its containing reducer

    long64 readySeq_;                          // bags equal or prior to this sequnce number are ready to reduce
    list<PartialAggregationBag*> readyBags_;   // the ready bags store    
    PartialAggregationBag* openBag_;           // the bag which is still open to receive more events, no need lock to protect this member as only one thread access it
};

class AggregationReducer {
    friend class PartialAggregationBagManager;
    friend class ReduceOperator;
public:
    AggregationReducer(int parallelism, vector<GroupExpr*>* groupExprList,  Expr* outExpr) :
      parallelism_(parallelism),
      inReducing_ (false),
      nextReduceBagIndex_(0),
      readyBagNumInReduceBatch_(0),
      groupOperator_ (new GroupOperator(groupExprList,outExpr)){
        for (int i=0; i<parallelism; i++) {
            bagManagers_.push_back(new PartialAggregationBagManager(i, this));
            bagsInReduceBatch_.push_back(NULL);
        }
    }

    ~AggregationReducer() {
        for (int i=0; i<parallelism_; i++) {
            delete bagManagers_[i];
        }

        while (!freeBags_.empty()) {
            PartialAggregationBag* theBag = freeBags_.back();
            delete theBag;
            freeBags_.pop_back();
        }

        delete groupOperator_;
    }

    PartialAggregationBagManager* getBagManager(int i) {
        return bagManagers_[i];
    }

    PartialAggregationBag* getOneBagFromPool() {
        PartialAggregationBag* rt = NULL;
        mutex_.lock();
        if (!freeBags_.empty()) {
            rt = freeBags_.back();
            freeBags_.pop_back();
        }            
        mutex_.unlock();
        if (!rt) {
            rt = new PartialAggregationBag();
        }
        return rt;
    }

private:
    /**
     * to get a bag that can be reduced right now, before calling this method, the lock should have been aquired first
     * there are 2 situations to call this method
     * (1) upon closing a bag
     * (2) upon finishing reduce one batch of bags
     *
     * @param oneReadyBag  it can be NULL or a bag that is just closed
     * @return a bag that can be reduced immediately
     */
    PartialAggregationBag* getBagToReduce(PartialAggregationBag* oneReadyBag) {
        // add the bag if it belongs to the same batch
        if (oneReadyBag && 
            nextReduceBagIndex_ > 0 && 
            oneReadyBag->seq_ == bagsInReduceBatch_[0]->seq_) {
            bagsInReduceBatch_[readyBagNumInReduceBatch_] = oneReadyBag;
            readyBagNumInReduceBatch_++;
        }

        if (inReducing_) {
            return NULL;
        }
        
        // move on to next bag
        PartialAggregationBag* theBag = NULL;
        if (nextReduceBagIndex_ < readyBagNumInReduceBatch_) {
            theBag = bagsInReduceBatch_ [nextReduceBagIndex_];
            nextReduceBagIndex_ ++;
            inReducing_ = true;
            return theBag;
        }

        // pick a bag for a new batch
        theBag = NULL;
        if (readyBagNumInReduceBatch_ == 0) {
            for (int i=0; i< parallelism_; i++) {
                if (theBag == NULL) {
                    theBag = bagManagers_[i]->getFirstReadyBag();
                }
                else if (bagManagers_[i]->getFirstReadyBag() != NULL) {
                    theBag = (theBag->seq_ > bagManagers_[i]->getFirstReadyBag()->seq_) ? bagManagers_[i]->getFirstReadyBag() : theBag;
                }
            }
            
            // found a bag with min seq, get all bags with same seq, put them in bagsInReduceBatch
            if (theBag) {
                for (int i=0; i< parallelism_; i++) {
                    if (bagManagers_[i]->getFirstReadyBag() && 
                        theBag->seq_ == bagManagers_[i]->getFirstReadyBag()->seq_) {
                        bagsInReduceBatch_[readyBagNumInReduceBatch_] = bagManagers_[i]->getFirstReadyBag();
                        readyBagNumInReduceBatch_++;
                    }
                }
                // prepare to reduce the first bag
                nextReduceBagIndex_ = 1;
                inReducing_ = true;
                return bagsInReduceBatch_[0];
            }
        }
        return NULL;
    }

    /**
     * check if the current batch of bags have been reduced.
     * before calling this method, the lock must be acquired.
     */
    bool checkCurrentBatchCompletion() {
        bool completed = readyBagNumInReduceBatch_ == parallelism_ &&
                         readyBagNumInReduceBatch_ == nextReduceBagIndex_;
        
        long64 theSeq = bagsInReduceBatch_[0]->seq_;
        int64_t ts = bagsInReduceBatch_[0]->ts_;

        // post processing on completion
        if (completed) {
            // reset some data
            readyBagNumInReduceBatch_ = 0;
            nextReduceBagIndex_ = 0;
            for (int i =0; i< parallelism_; i++) {
                // move the bag into free bags pool
                freeBags_.push_back(bagsInReduceBatch_[i]);
                bagsInReduceBatch_[i] = NULL;
                bagManagers_[i]->popFirstReadyBag();
            }

            // flush out result
            tempOutEvent_.setEvent(NULL, Event::TYPE_FLUSH, ts);
            groupOperator_->execute(theSeq, &tempOutEvent_);
        }
        return completed;
    }

   
    void reduceOneBag(PartialAggregationBag* aBag) {
        Event* theEvent;
        long64 theSeq = aBag->seq_;
        while (! aBag->events_.empty()) {
            theEvent = aBag->events_.front();
            groupOperator_->execute(theSeq, theEvent);
            theEvent->decref();     // free the event back into its pool
            aBag->events_.pop_front();
        }
        mutex_.lock();
        inReducing_ = false;
        mutex_.unlock();
    }

private:
    int parallelism_;
    fc::Mutex mutex_;                                      // protection lock
    vector<PartialAggregationBagManager*>   bagManagers_;  // each parallel instance uses one manager
    vector<PartialAggregationBag*> freeBags_;              // free bags pool

    bool inReducing_;                                      // flag indicator whether one thread is currently doing reducing
    int nextReduceBagIndex_;                               // the next bag to be reduced after finish current one
    int readyBagNumInReduceBatch_;                         // counter to check whether need to wait for more bags or just move to next batch
    vector<PartialAggregationBag*> bagsInReduceBatch_;     // the current batch of bags being reduced,  each parallel instance has one entry
                                                           // NULL entry means it's still not ready

    GroupOperator* groupOperator_;                         // the embeded group/aggregation operator to reduce final aggregation result
    IntermediateEvent tempOutEvent_;                       // temp object used for sending Flush OUT events to downstream
};

/**
 * Reduce operator
 * this operator is used for reducing results from multiple partitioned group operators
 * essentially, it's also an group operator
 */
class ReduceOperator : public BaseOperator {
public:
    /**
     *  ReduceOperator operator constructor. 
     *
     *  @param groupExprList same as Group operator
     *  @param outExpr  same as Group operator
     */
    ReduceOperator(vector<GroupExpr*>* groupExprList,  Expr* outExpr) : 
      strValue_("Reduce"), 
      groupExprList_(groupExprList), 
      outExpr_(outExpr), 
      reducer_(NULL), 
      needDestructReducer_(false),
      bagManager_(NULL),
      openBag_(NULL) {
    }

    virtual ~ReduceOperator() {
        if (groupExprList_) {
            for (vector<GroupExpr*>::const_iterator iter = groupExprList_->begin(); iter < groupExprList_->end(); iter++) {
                delete (*iter);
            }
            groupExprList_->clear();
            delete groupExprList_;
        }
        if (outExpr_) 
            delete outExpr_;

        if (needDestructReducer_)
            delete reducer_;
    }

    // @override
    virtual BaseOperator* clone() {
        vector<GroupExpr*>* theClonedList = NULL;
        Expr* clonedOutExpr = NULL;
        if (groupExprList_) {
            theClonedList = new vector<GroupExpr*>();
            for (vector<GroupExpr*>::const_iterator iter = groupExprList_->begin(); iter < groupExprList_->end(); iter++) {
                theClonedList->push_back((*iter)->clone());
            }
        }
        if (outExpr_) {
           clonedOutExpr = outExpr_->clone(); 
        }

        BaseOperator* clonedOperator = new ReduceOperator(theClonedList, clonedOutExpr);
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    // @ovverride
    virtual string& toString() {
        return strValue_ ;
    }

    // @override
    /**
     * executor context should be setup in preparation stage
     * in this method, also setup and prepare the embeded AggregationReducer
     */
    virtual void setExecutorContext(ExecutorContext* ctx) {
        executorCtx_ = ctx;        
        parallelIndex_ = executorCtx_->getparallelIndex();
        nParallelism_ = executorCtx_->getParallelism();
        
        // create and intialize the reducer if it's not there yet
        void* data = executorCtx_->getExecutor()->getOperatorSharedData(operatorId_);
        if (data) {
            reducer_ = reinterpret_cast<AggregationReducer*> (data);
        }
        else {
            vector<GroupExpr*>* theClonedList = NULL;
            Expr* clonedOutExpr = NULL;
            if (groupExprList_) {
                theClonedList = new vector<GroupExpr*>();
                for (vector<GroupExpr*>::const_iterator iter = groupExprList_->begin(); iter < groupExprList_->end(); iter++) {
                    theClonedList->push_back((*iter)->clone());
                }
            }
            if (outExpr_) {
               clonedOutExpr = outExpr_->clone(); 
            }
            reducer_ = new AggregationReducer(nParallelism_, theClonedList, clonedOutExpr);
            reducer_->groupOperator_->prepare(pNextOperator_);
            reducer_->groupOperator_->setExecutorContext(ctx);
            needDestructReducer_ = true;
            data = reinterpret_cast<void*> (reducer_);
            executorCtx_->getExecutor()->setOperatorSharedData(operatorId_, data);
        }

        // set bagManager
        bagManager_ = reducer_->getBagManager(parallelIndex_);
        openBag_ = NULL;
    }

    // @override
    virtual void execute(long64 seq, Event* pevent) {
        if ( pevent->isDataEvent() ) {

#ifdef _DEBUG
        JsonStringOutputStream payloadSerializer;
        const string& str = payloadSerializer.serialize(*pevent, false);
        printf("*** reduce (%c) seq= %d, TS = %lld, %s \n", ((pevent->getType() == Event::TYPE_PLUS) ? '+' : '-'), seq, pevent->getTimestamp(), str.c_str());
#endif
            if (openBag_) {
                // guard only put same batch events in one bag
                // this check is indeed redundant, it can be replaced with assert
                if(openBag_->seq_ == seq) {
                    openBag_->addEvent(pevent);
                }
            }
            else {
                openBag_ = bagManager_->openBag(seq);
                openBag_->addEvent(pevent);

            }
            return;
        }
        else if (pevent->isFlushEvent()) {
            if (!openBag_) {
                openBag_ = bagManager_->openBag(seq);
            }
            openBag_->ts_ = pevent->getTimestamp();
            bagManager_->closeBag();
            openBag_ = NULL;        
            return;
        }
        else {
            // directly pass down other type of events
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);
            }
            return;
        }
    }
private:
    string strValue_;
    vector<GroupExpr*>* groupExprList_;
    Expr* outExpr_;
    BaseExecutionContext context_;    

    int parallelIndex_;    // the index number among multiple parallel instances
    int nParallelism_;     // total number of parallel instances
    AggregationReducer* reducer_; 
    bool needDestructReducer_; 
    PartialAggregationBagManager* bagManager_ ; 
    PartialAggregationBag* openBag_;
};

#endif