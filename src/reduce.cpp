#include "tokendef.h"
#include "reduce.h"

PartialAggregationBag* PartialAggregationBagManager::openBag(long seq) {
    if (openBag_)
        return NULL;
    else {
        openBag_ = reducer_->getOneBagFromPool();
        openBag_->seq_ = seq;
        return openBag_;
    }
 }

void PartialAggregationBagManager::closeBag() {
    reducer_->mutex_.lock();
    
    PartialAggregationBag* theBag = openBag_;
    if (openBag_) {
        readySeq_ = openBag_->seq_;         
        readyBags_.push_back(openBag_);
        openBag_ = NULL;
    }
    // try to check if any bag can be reduced
    theBag = reducer_->getBagToReduce(theBag);
    reducer_->mutex_.unlock();

    while (theBag) {            
        reducer_->reduceOneBag(theBag);
        // try to see if the current batch finishes, whether there are more bags to reduce
        reducer_->mutex_.lock();
        reducer_->checkCurrentBatchCompletion();
        theBag = reducer_->getBagToReduce(NULL);
        reducer_->mutex_.unlock();
    }
}
