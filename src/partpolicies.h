#ifndef _PART_POLICIES_H
#define _PART_POLICIES_H

#include <string>
#include "GeneralHashFunctions.h"
#include "event.h"

using namespace std;

/**
 * abstract PartitionPolicy base class
 */
class PartitionPolicy {
public:
    /**
     * apply the partition policy
     *
     * @parIndex the parallel instance index
     * @nParallelism the number of parallel instances
     * @seq the incomming event sequence number
     * @pEvent pointer to the incomming event object
     * @return true if the policy satisfies, false for otherwise.
     */
    virtual bool evalPolicy(int parIndex, int nParallelism, long64 seq, Event* pEvent) = 0;

    /**
     * Property of a policy
     *
     * @return true if evaluating the policy requires event content, or the policy requires all events
     *         false on otherwise.
     */
    virtual bool needAllEvent() = 0;

    virtual PartitionPolicy* clone() = 0;
};


/**
 * A partition policy for copying every event to each parallel instance. 
 */
class PartitionAll : public PartitionPolicy {
public:
    // @override
    virtual bool evalPolicy(int parIndex, int nParallelism, long64 seq, Event* pEvent) {
        return true;
    }

    // override
    virtual bool needAllEvent() {
        return true;
    }

    // override
    virtual PartitionPolicy* clone() {
        return new PartitionAll();
    }
};


/**
 * A roundrobin partition policy. 
 */
class PartitionRoundrobin : public PartitionPolicy {
public:
    // @override
    virtual bool evalPolicy(int parIndex, int nParallelism, long64 seq, Event* pEvent) {
        if ((seq/10) % nParallelism == parIndex)
            return true;
        else
            return false;
    }

    // override
    virtual bool needAllEvent() {
        return false;
    }

    // override
    virtual PartitionPolicy* clone() {
        return new PartitionRoundrobin();
    }
};



/**
 * A partition policy according to the hash value of an expr result. 
 */
class PartitionPerValue : public PartitionPolicy {
private:
    Expr* partitionExpr_;
    BaseExecutionContext context_;
    JsonStringOutputStream serializer_;
public:
    PartitionPerValue(Expr* expr) : partitionExpr_(expr) {
    }

    virtual ~PartitionPerValue() {
        if (partitionExpr_)
            delete partitionExpr_;
    }

    // @override
    virtual bool evalPolicy(int parIndex, int nParallelism, long64 seq, Event* pEvent) {
        // setup context
        context_.reset();
        context_.setCurrentEvent(pEvent);
        Value& v = partitionExpr_->eval(&context_).getValue();
        // serialize result into a string and then calc its hash value
        serializer_.Reset();
        const string& vstr = serializer_.serialize(&v);
        unsigned int vhash = SDBMHash(vstr);
        return (vhash % nParallelism == parIndex);
    }

    // override
    virtual bool needAllEvent() {
        return true;
    }

    // override
    virtual PartitionPolicy* clone() {
        if (partitionExpr_)
            return new PartitionPerValue(partitionExpr_->clone());
        else
            return new PartitionPerValue(NULL);
    }
};


#endif