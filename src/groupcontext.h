#ifndef _GROUP_CONTEXT_H
#define _GROUP_CONTEXT_H

#include <unordered_map>

using namespace std;
using namespace std::tr1;



/**
 *  base function context class
 */
class FunctionContext {
public:
    FunctionContext() {
    }

    virtual ~FunctionContext() {
    }
};


/** 
 * Group context is used to maintain the state info for each group.
 * A group context consists of 2 parts:
 *   (1) last resulting event being kept for the group
 *   (2) aggregation/stateful functions states
 *
 * This group context object will be passed to expr evalation through BaseExecutionContext
 */
class GroupContext {
typedef unordered_map <int, FunctionContext*> FunctionsContextMap; 
public:
    GroupContext() : resultEvent_(NULL), eventNumInGroup_(0) {
    }

    virtual ~GroupContext() {
        clear();
    }

    virtual void clear() {
        if (resultEvent_) 
            delete resultEvent_;

        for (FunctionsContextMap::const_iterator iter = functionsContext_.begin(); iter != functionsContext_.end(); iter++) {
            delete (iter->second);
        }
    }

    Event* getResultEvent() {
        return resultEvent_;
    }

    void setResultEvent(Event* theEvent) {
        if (resultEvent_)
            delete resultEvent_;
        resultEvent_ = theEvent;
    }

    FunctionContext* getFunctionContext(int id) {
        FunctionsContextMap::const_iterator iter = functionsContext_.find(id);
        if (iter == functionsContext_.end()) {
            return NULL;
        }
        else {
            return iter->second;
        }        
    }

    bool addFunctionContext(int id, FunctionContext* funcCtx) {
        // not allowing to add a null context into the map
        if (! funcCtx )
            return false ;   

        bool rt = (functionsContext_.insert(FunctionsContextMap::value_type(id, funcCtx))).second;
        return rt;
    }

    void removeFunctionContext(int id) {
        FunctionsContextMap::const_iterator iter = functionsContext_.find(id);
        if (iter == functionsContext_.end())
            return;

        delete (iter->second);
        functionsContext_.erase(iter);
    }

    /**
     * Get current evnet number in the group
     */
    long getEventNumInGroup() {
        return eventNumInGroup_;
    }

    /**
     * Add one event into the group
     */
    long addOneEvent() {
        return (++ eventNumInGroup_);
    }

    /**
     * Remove one event from the group
     */
    long removeOneEvent() {
        return (-- eventNumInGroup_);
    }
private:
    Event* resultEvent_;
    unordered_map <int, FunctionContext*> functionsContext_;
    long eventNumInGroup_;
};

#endif