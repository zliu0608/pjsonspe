#ifndef TOKEN_DEF_H_
#define TOKEN_DEF_H_

#include <string>
#include <vector>
#include <deque>
#include <stack>
#include <map>
#include <assert.h>
#include "event.h"
#include "inteventpool.h"
#include "expr.h"
#include "builtinfuncs.h"

#include "partpolicies.h"
#include "rbstream.h"
#include "eventsink.h"

using namespace std;


struct Token {
    const char *z;
    char  s[256];
    unsigned int line;      // tracking ling no.
    unsigned int column;    // tracking the column no.
};


class AbstractCommandsExecutor;
class SimpleCommandsExecutor;
class ThreadedCommandsExecutor;

/**
 * Context object provide access to Executor facility
 */
class ExecutorContext {
public:
    ExecutorContext() : eventPool_(new InternalEventPool()), parallelIndex_(0), nParallelism_(1), executor_(NULL) {        
    }

    ~ExecutorContext() {
        delete eventPool_;
    }

    AbstractEventPool* getEventPool() {
        return eventPool_;
    }

    void setParallelism(int nParallelism) {
        nParallelism_ = nParallelism;
    }

    void setparallelIndex(int parallelIndex) {
        parallelIndex_ = parallelIndex;
    }

    int getParallelism() {
        return nParallelism_;
    }

    int getparallelIndex() {
        return parallelIndex_;
    }

    void setExecutor(AbstractCommandsExecutor* executor) {
        executor_ = executor;
    }

    AbstractCommandsExecutor* getExecutor(){
        return executor_;
    }
private:
    AbstractEventPool* eventPool_;     // the internal event pool used by a executor
    int parallelIndex_;    // the index number among multiple parallel instances
    int nParallelism_;     // total number of parallel instances
    AbstractCommandsExecutor* executor_;   // the owning cmmands executor
};

/**
 * All operators should inherit from BaseOperator
 */
class BaseOperator {
    friend class AbstractCommandsExecutor;
protected:
    int operatorId_;                    // an non-negative integer which is unique within a EPN
    BaseOperator* pNextOperator_;       // point to the next operator at downstream
    bool isMutable_;                    // a boolean flag indicating whether this operator can output event different than its input
    bool needBuffer_;                   // a boolean flag indicating whether this operator needs to buffer up events
    bool needCopyEvent_;                // a computed flag indicating whether this operator should make a copy of event prior to pushing to next operator
                                        // never change this flag value

    ExecutorContext*  executorCtx_;     // point to the executor context object, which provides access to internal event pool, etc.


public:
    BaseOperator() : operatorId_(-1), pNextOperator_(NULL), isMutable_(false), needBuffer_(false) , needCopyEvent_(false), executorCtx_(NULL) {
    }

    virtual std::string & toString() = 0;
    
    virtual BaseOperator* clone() = 0;

    void cloneProtectedAttibutes(BaseOperator* src) {
        isMutable_ = src->isMutable_;
        needBuffer_ = src->needBuffer_;
        needCopyEvent_ = src->needCopyEvent_;
        operatorId_ = src->operatorId_;
    }


    virtual void prepare(BaseOperator* pNextOp) {
        pNextOperator_ = pNextOp;
    }

    virtual void execute(long64 seq, Event* pevent) = 0;

    virtual void completeBatch() {};

    bool isMutable() { return isMutable_; }
    bool needBuffer() {return needBuffer_; }
    bool needCopyEvent() {return needCopyEvent_; }

    /**
     * executor context should be setup in preparation stage
     */
    virtual void setExecutorContext(ExecutorContext* ctx) {
        executorCtx_ = ctx;
    }

    /**
     * will be set during static analysis
     */ 
    void setOperatorId(int id) {
        operatorId_ = id;
    }

    int getOperatorId() {
        return operatorId_;
    }
};


typedef vector<BaseOperator*> OperatorPipeline;

/**
 *  All window operator should inherite from WindowBaseOperator
 */
class WindowBaseOperator : public BaseOperator {
};

/**
 * A symbolic operator, it doesn't process event, instead it's used to connect two or more piplelines together
 * it is also used to wire multiple sources to a Join operator.
 */
class ConnectPointOperator : public BaseOperator {
    friend class AbstractCommandsExecutor;
    friend class ThreadedCommandsExecutor;
private:
    string strValue_;
    string pointName_; 
    OperatorPipeline* fromPipeline_;
    vector<OperatorPipeline*> toPipelines_;
public:
    ConnectPointOperator(const char* pszPointName, OperatorPipeline* srcPipeline) :
      strValue_("ConnectPoint"), pointName_(pszPointName), fromPipeline_(srcPipeline) {
      }

    virtual ~ConnectPointOperator() {
          // no need to destruct fromPipeline_ and toPipelines_ in this operator          
    }

    void addTargetPipeline(OperatorPipeline* oneTarget) {
        if (oneTarget) {
            toPipelines_.push_back(oneTarget);
        }
    }

    string& getConnectPointName() {
        return pointName_;
    }

    bool isFromWindow() {
        // find the operator in front of the ConnectOperator in fromPipeline
        // check if it's a window operator
        OperatorPipeline::reverse_iterator rit = fromPipeline_->rbegin();
        rit++;
        WindowBaseOperator* winOp = dynamic_cast<WindowBaseOperator*> (*rit);
        return (winOp != NULL);
    }

    // @override
    virtual BaseOperator* clone() {
        BaseOperator* clonedOperator = new ConnectPointOperator(pointName_.c_str(), fromPipeline_);
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
        if (!(pevent->isDataEvent())) {
            for (vector<OperatorPipeline*>::const_iterator iter=toPipelines_.begin(); iter<toPipelines_.end(); iter++) {
                OperatorPipeline* thePipeline = *iter;

                // only need to call execute() on the first operator, it will execute through the whole pipeline
                thePipeline->front()->execute(seq, pevent);  
            }

            //if (pNextOperator_) {
            //     pNextOperator_->execute(seq, pevent);
            //}
            return;
        }

        // do event copy if necessary
        if (needCopyEvent()) {
            pevent = executorCtx_->getEventPool()->copyEvent(seq, pevent);
            pevent->incref();
        }

        // execute each of  its connecting pipelines.
        for (vector<OperatorPipeline*>::const_iterator iter=toPipelines_.begin(); iter<toPipelines_.end(); iter++) {
            OperatorPipeline* thePipeline = *iter;

            // only need to call execute() on the first operator, it will execute through the whole pipeline
            thePipeline->front()->execute(seq, pevent);  
        }

        if (needCopyEvent()) {
            pevent->decref();
        }
    }

};

/**
 * Context object used in parsing a query
 */
struct ParseContext {
    int errorCode;             // 0 if no error    
    OperatorPipeline* curPipeline;
    vector<OperatorPipeline*> allPipelines;
    vector<ConnectPointOperator*> connectOperators;
    int nextStateContextKey;   // next available stateful context key

    ConnectPointOperator* getConnectPointOperator(const char* pointName) {
        for (vector<ConnectPointOperator*>::const_iterator iter = connectOperators.begin(); 
             iter < connectOperators.end();
             iter++) {
            if (0 == (*iter)->getConnectPointName().compare(pointName))
                return (*iter);
        }
        return NULL;
    }

    /**
     * add a new ConnectPoint operator to the end of the curPipeline
     * 
     * @param pointName the name of this operator
     * @return the newly added operator only when the same name operator doesn't exist,  
     *  otherwise return NULL
     */
    ConnectPointOperator* addConnectPointOperator(const char* pointName) {
        if (getConnectPointOperator(pointName))
            return NULL;

        ConnectPointOperator* aNewConnectPoint = new ConnectPointOperator(pointName, curPipeline);
        curPipeline->push_back(aNewConnectPoint);
        connectOperators.push_back(aNewConnectPoint);
        return aNewConnectPoint;
    }

    ParseContext(): errorCode(0), curPipeline(NULL), nextStateContextKey(0) {        
    }

    ~ParseContext() {
        for (vector<OperatorPipeline*>::const_iterator iter = allPipelines.begin(); iter < allPipelines.end(); iter++) {
            OperatorPipeline* thePipeline = *iter;
            for (OperatorPipeline::const_iterator iter2 = thePipeline->begin(); iter2 < thePipeline->end(); iter2++) {
                delete (*iter2);
            }
            delete thePipeline;
        }
    }
};


class StreamOperator : public BaseOperator {
private:
    std::string strValue_;
    std::string streamName_;
    int streamType_;
    int parallelIndex_;
    int nParallelism_; 

    PartitionPolicy* inputPartitionPolicy_;          // default use Roundrobin parition policy

    JsonStringOutputStream payloadSerializer_;
    EventSink* sink_;

    deque<Event*> buffer_;
public:
    StreamOperator(const char* pszStreamName, int streamType) :
    streamName_(pszStreamName), 
    streamType_(streamType),
    sink_(NULL),
    parallelIndex_(0), 
    nParallelism_(1), 
    inputPartitionPolicy_ (NULL) {
        if (streamType_ == 0) {
            strValue_ = "input_stream(";
            strValue_ += streamName_;
            strValue_ += ")";
            isMutable_ = true ;   // source stream is deemed as a mutable operator

            inputPartitionPolicy_ = new PartitionRoundrobin();  // only set default partition policy for input stream operator
        }
        else {
            strValue_ = "output_stream(";
            strValue_ += streamName_;
            strValue_ += ")";
            sink_ = new EventSink();
        }        
    }

    StreamOperator(const char* pszStreamName, int streamType, PartitionPolicy* policy ) :
    streamName_(pszStreamName), 
    streamType_(streamType),
    sink_(NULL),
    parallelIndex_(0), 
    nParallelism_(1), 
    inputPartitionPolicy_ (policy) {    
        if (streamType_ == 0) {
            strValue_ = "input_stream(";
            strValue_ += streamName_;
            strValue_ += ")";
            isMutable_ = true ;   // source stream is deemed as a mutable operator
        }
        else {
            strValue_ = "output_stream(";
            strValue_ += streamName_;
            strValue_ += ")";
            sink_ = new EventSink();
        }            
    }

    virtual ~StreamOperator() {
        if (sink_) 
            delete sink_;
        if (inputPartitionPolicy_)
            delete inputPartitionPolicy_;
    }

    PartitionPolicy* getPartitionPolicy() {
        return inputPartitionPolicy_;
    }

    virtual BaseOperator* clone() {
        PartitionPolicy* clonedPolicy = (inputPartitionPolicy_ == NULL) ? NULL : inputPartitionPolicy_->clone();
        BaseOperator* clonedOperator = new StreamOperator(streamName_.c_str(), streamType_, clonedPolicy);
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    void setParallelIndex(int nParallelism, int index) {
        nParallelism_ = nParallelism;
        parallelIndex_ = index;
    }

    int getParallelIndex() {
        return parallelIndex_;
    }

    int getParallelism() {
        return nParallelism_;
    }

    const std::string& getStreamName() {
        return streamName_;    
    };

    int getStreamType() {
        return streamType_;
    }

    EventSink* getEventSink() {
        return sink_;
    }

    virtual std::string& toString() {
        return strValue_ ;
    }

    virtual void execute(long64 seq, Event* pevent) {
        // pass through control events
        if (!(pevent->isDataEvent())) {
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);
            }
            return;
        }

        // directly pass to next operator
        if (pNextOperator_ != NULL) {
            // do event copy if necessary
            if (needCopyEvent()) {
                pevent = executorCtx_->getEventPool()->copyEvent(seq, pevent);
                pevent->incref();
            }
            
            pNextOperator_->execute(seq, pevent);

            if (needCopyEvent()) {
                pevent->decref();
            }
        }

        if (streamType_ == 1) {
            // only output plus events through output stream
           if (pevent->getType() != Event::TYPE_PLUS)
               return;

            const string& str = payloadSerializer_.serialize(*pevent, false);
            sink_->storeEvent(seq, str, pevent->getTimestamp(), pevent->getType());
            // TODO: send the event into evnet sink or console            
            // printf(">>> output 1 event: TS = %lld \n", pevent->getTimestamp());        
            // printf("%s\n",str.c_str());
        }
    }


    virtual void prepare(BaseOperator* pNextOp) {
        BaseOperator::prepare(pNextOp);

        if (streamType_ == 0) {
           /*
            char evt[] = " { \"symbol\" : \"IBM\", \"value\" : 110 , \"quantity\" : 2000 } "; 
            char* p = (char*) malloc(strlen(evt) + 1);
            
      
            for (int i=0; i<10; i++) {
                strcpy(p, evt);
                // populate 10 sample events
                Event* pEvent = new Event(p);
                pEvent->incref();
                buffer_.push_back(pEvent);
            }
            */
        }
    }

    virtual Event* getNextEvent(){
        if (buffer_.empty())
            return NULL;
        else {
            Event* ret = buffer_.front();
            buffer_.pop_front();
            return ret;
        }
    }

    virtual Event* peekNextEvent() {
        if (buffer_.empty())
            return NULL;
        else
            return buffer_.front();
    }

    void addInputEvent(Event* pEvent) {
        pEvent->incref();
        buffer_.push_back(pEvent);
    }
};


class FilterOperator : public BaseOperator {
private:
    std::string strValue_;
    Expr* condition_;
    BaseExecutionContext context_;
public : 
    FilterOperator(Expr* condition) : strValue_("filter"), condition_(condition) {
        
    }

    virtual ~FilterOperator() {
        if (condition_) 
            delete condition_;
    }

    virtual BaseOperator* clone() {
        BaseOperator* clonedOperator = new FilterOperator(condition_->clone());
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    virtual std::string& toString() {
        return strValue_;
    }

    virtual void execute(long64 seq, Event* pevent) {
        // pass through control events
        if (!(pevent->isDataEvent())) {
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);
            }
            return;
        }

        // setup context
        context_.reset();
        context_.setCurrentEvent(pevent);

        // invoke the next operator if condition is evaluated to true
        if (condition_->eval(&context_).getBool()) 
        {
            if (pNextOperator_) {
                // do event copy if necessary
                if (needCopyEvent()) {
                    pevent = executorCtx_->getEventPool()->copyEvent(seq, pevent);
                    pevent->incref();
                }

                pNextOperator_->execute(seq, pevent);

                if (needCopyEvent()) {
                    pevent->decref();
                }
            }   // end of execute next operator
        } // end condition check
    }
};


class TransformOperator : public BaseOperator {
private:
    std::string strValue_;
    Expr* mapExpr_;
    BaseExecutionContext context_;
    IntermediateEvent resultEvent_;
public:
    TransformOperator(Expr* mapExpr) : strValue_("transform"), mapExpr_(mapExpr) {
        isMutable_ = true;   // Transform is a mutable operator
    }
    
    virtual ~TransformOperator(){
        if (mapExpr_)
            delete mapExpr_;
    }

    virtual BaseOperator* clone() {
        BaseOperator* clonedOperator = new TransformOperator(mapExpr_->clone());
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    virtual std::string& toString() {
        return strValue_;
    }

    virtual void execute(long64 seq, Event* pevent) {
        // pass through control events
        if (!(pevent->isDataEvent())) {
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);
            }
            return;
        }

        // setup context
        context_.reset();
        context_.setCurrentEvent(pevent);
        
        // evalute the transformation expression
        // wrap its result into a Event object for downstream operators
        Value& v = mapExpr_->eval(&context_).getValue();
        resultEvent_.setEvent(&v, pevent->getType(), pevent->getTimestamp());
        pevent = &resultEvent_;
        if (pNextOperator_) {
            // do event copy if necessary
            if (needCopyEvent()) {
                pevent = executorCtx_->getEventPool()->copyEvent(seq, pevent);
                pevent->incref();
            }            
            
            pNextOperator_->execute(seq, pevent);

            if (needCopyEvent()) {
                pevent->decref();
            }
        } // end of executing next operator
    }
};

class AbstractCommandsExecutor {
    typedef map<int,void*> OperatorSharedDataMap;
protected:
    /**
     * Find a pipeline which directly connect to this pipeline
     *
     * @return NULL if this is already the heading pipeline,  otherwise returns its connecting upper-stream pipeline
     */
    OperatorPipeline* findHeadPipeline(vector<ConnectPointOperator*>& connectOperators, OperatorPipeline* thePipeline ) {
        OperatorPipeline* headPipeline = NULL;
        for (vector<ConnectPointOperator*>::const_iterator iter = connectOperators.begin(); iter < connectOperators.end() ; iter++) {
            ConnectPointOperator* theConnectOperator = *iter;
            for (vector<OperatorPipeline*>::const_iterator iter2 = theConnectOperator->toPipelines_.begin(); iter2 < theConnectOperator->toPipelines_.end(); iter2++) {
                if ((*iter2) == thePipeline) {
                    headPipeline = theConnectOperator->fromPipeline_;
                    break;
                }
            }
        }
        return headPipeline;
    }
    
    void doStaticAnalysis(vector<OperatorPipeline*>* pipelines) {
        // set operator id
        int id = 0;
        for (vector<OperatorPipeline*>::const_iterator iter = pipelines->begin(); iter < pipelines->end(); iter++) {
            OperatorPipeline* thePipeline = *iter;

            for (vector<BaseOperator*>::const_iterator iter2 = thePipeline->begin(); iter2 < thePipeline->end(); iter2++) {
                BaseOperator* theOperator = *iter2;
                theOperator->setOperatorId(id++);
            }
        }

        // find out all connect operators
        vector<ConnectPointOperator*> connectOperators;
        for (vector<OperatorPipeline*>::const_iterator iter = pipelines->begin(); iter < pipelines->end(); iter++) {
            OperatorPipeline* thePipeline = *iter;
            BaseOperator* theOperator = thePipeline->back();
            ConnectPointOperator* theConnectOperator = dynamic_cast<ConnectPointOperator*> (theOperator);
            if (theConnectOperator) {
                connectOperators.push_back(theConnectOperator);
            }
        }

        // loop every pipeline
        for (vector<OperatorPipeline*>::const_iterator iter = pipelines->begin(); iter < pipelines->end(); iter++) {
            OperatorPipeline* thePipeline = *iter;

            // extend the operators link
            deque<BaseOperator*> operatorsLink;
            OperatorPipeline* headPipeline = findHeadPipeline(connectOperators, thePipeline);
            while (headPipeline) {
                for (vector<BaseOperator*>::reverse_iterator riter = headPipeline->rbegin(); riter < headPipeline->rend(); riter++) {
                    operatorsLink.push_front(*riter);
                }
                headPipeline = findHeadPipeline(connectOperators, headPipeline);
            }

            // analyze one pipeline
            for (vector<BaseOperator*>::const_iterator iter2 = thePipeline->begin(); iter2 < thePipeline->end(); iter2++) {
                BaseOperator* theOperator = *iter2;                               
                stack<BaseOperator*>  operatorStack(operatorsLink);

                if (theOperator->needBuffer()) {
                    if (operatorStack.empty()) {
                        // when a window operator is placed at the begining of the entire EPN
                        assert(false);
                    }
                    BaseOperator* candidateOperator = operatorStack.top();
                    // look back till to the previous needBuffer operator or the first operator
                    // along the path, if there is any mutable operator, then a copy should be made
                    while ( !operatorStack.empty()) {
                        if ( operatorStack.top()->needCopyEvent()) {
                            // no mutable operator is found but found a window operator
                            break;
                        }
                        if (operatorStack.top()->isMutable()) {
                            candidateOperator->needCopyEvent_ = true;
                            break;
                        }
                        operatorStack.pop();
                    }
                }
                operatorsLink.push_back(theOperator);                
            } // end processing a pipeline
        } // end processing all pipelines        
    }

public:
    virtual void start() {};
    virtual void stop() {};
    virtual vector<BaseOperator*>* getOperatorChain() {return NULL;}

    void* getOperatorSharedData(int id) {
        OperatorSharedDataMap::iterator iter = operatorSharedDataMap_.find(id);
        if (iter == operatorSharedDataMap_.end()) {
            return NULL;
        }
        else {
            return iter->second;
        }
    }

    void setOperatorSharedData(int id, void* data) {
        OperatorSharedDataMap::iterator iter = operatorSharedDataMap_.find(id);
        if (iter == operatorSharedDataMap_.end()) {
            operatorSharedDataMap_.insert(OperatorSharedDataMap::value_type(id, data));
        }
        else {
            iter->second = data;
        }        
    }

    bool isSyncByInterval() {
        return useIntervalSync_;
    }

    void setSyncByInterval(bool value) {
        useIntervalSync_ = value;
    }
protected:
    map<int,void*> operatorSharedDataMap_;
    bool useIntervalSync_ ;        // control sync frequency, only used by window operators, false: sync every event; true sync every 200000 by default.
};

/**
 * Executor classes
 * each executor will evaluate operators in sequential manner within the same thread
 */
class CommandsExecutor : public AbstractCommandsExecutor {
private:
    vector<BaseOperator*> *pOperatorList_;
    vector<StreamOperator*> *pSourceList_;

    // disallow copy constructor
    CommandsExecutor(const CommandsExecutor&); 
    
    // fina a source which has events ready to execute
    StreamOperator* getAvailableSource() {
        if (! pSourceList_) 
            return NULL;

        if (pSourceList_->size() == 1) {           
                return (*pSourceList_)[0];
        }


        // in multiple sources case, find out the source with least timestamp
        StreamOperator* retSource = NULL;
        int64_t nextTs = TIMESTAMP_MAX;
        Event* nextEvent = NULL;

        for (size_t i=0; i< pSourceList_->size(); i++) {
            nextEvent = (*pSourceList_)[i]->peekNextEvent();
            if (nextEvent && nextEvent->getTimestamp()<nextTs) {
                retSource = (*pSourceList_)[i];
                nextTs = nextEvent->getTimestamp();
            }
        }
        return retSource;
    }

public:
    CommandsExecutor(vector<BaseOperator*> *pOperators) : pOperatorList_(pOperators), 
        pSourceList_(new vector<StreamOperator*>() ){
    }

    virtual ~CommandsExecutor() {
        delete pSourceList_;
    }

    void prepare() {
        if (! pOperatorList_ )
            return;

        size_t n = pOperatorList_->size();
        if (n == 0)
            return;

        // setup the operator chain
        int i = (int) n-1;
        BaseOperator* pOp = (*pOperatorList_)[i];
        pOp->prepare(NULL);
        i--;
        while (i >= 0) {
            BaseOperator* curOp = (*pOperatorList_)[i];
            curOp-> prepare(pOp);
            pOp = curOp;

            StreamOperator* pStreamOp = dynamic_cast<StreamOperator*>(curOp);
            if (pStreamOp!=NULL && pStreamOp->getStreamType()==0) {
                pSourceList_->push_back(pStreamOp);
            }
            i--;
        }

        // prepare source streams and their subscribers
        for (size_t i=0; i< pSourceList_->size(); i++) {
            StreamOperator* pStreamOp = (*pSourceList_)[i];
            g_rbSSMgr.getStream(pStreamOp->getStreamName())->addSubscriber(this, pStreamOp);
        }
        // initialize publisher for each source streams
        // TODO: this needs to be moved to out side of the Executor, should be at network level.
        for (size_t i=0; i< pSourceList_->size(); i++) {
            StreamOperator* pStreamOp = (*pSourceList_)[i];
            g_rbSSMgr.getStream(pStreamOp->getStreamName())->initProducer();
        }
    }

    virtual void start() {
        // TODO: this needs to be moved to out side of the Executor, should be at network level.
        for (size_t i=0; i< pSourceList_->size(); i++) {
            StreamOperator* pStreamOp = (*pSourceList_)[i];
            g_rbSSMgr.getStream(pStreamOp->getStreamName())->start();
        }
    }

    virtual void stop() {
        // TODO: this needs to be moved to out side of the Executor, should be at network level.
        for (size_t i=0; i< pSourceList_->size(); i++) {
            StreamOperator* pStreamOp = (*pSourceList_)[i];
            g_rbSSMgr.getStream(pStreamOp->getStreamName())->stop();
        }
    }

    virtual vector<BaseOperator*>* getOperatorChain() {
        return pOperatorList_;
    }

    /**
     * This is deprecated after use RingBuffer based Streams
     */
    void execute() {
        StreamOperator* pSource;
        Event* pEvent;
        long64 seq = 0;
        while (true) {
            pSource = getAvailableSource();
            if (pSource) 
                pEvent = pSource->getNextEvent();
            else
                pEvent = NULL;

            if (pEvent) {
                pSource->execute(seq++, pEvent);
                pEvent->decref();
            }
            else
                break;  // wait for being notified
        }
        // printf("no more events\n");
    }
};


typedef vector<StreamOperator*> SourceStreamEntries;
typedef vector<EventSink*> ResultStreamEntries;

/**
 * SimpleCommandsExecutor classes
 * each executor will evaluate operators in sequential manner within the same thread
 * return result events if there are
 */
class SimpleCommandsExecutor : public AbstractCommandsExecutor {
private:
    vector<OperatorPipeline*> *pPipelineList_;
    vector<StreamOperator*> *pSourceList_;
    vector<StreamOperator*> *pOuputStreamList_;
    long64 seq_;

    ExecutorContext executorCtx_;

    // disallow copy constructor
    SimpleCommandsExecutor(const SimpleCommandsExecutor&); 
    

    /**
     *  setup one operator chain, also find out all source streams in pSourceList
     */
    void prepareOnePipeline(OperatorPipeline* thePipeline) {
        if (! thePipeline )
            return;

        size_t n = thePipeline->size();
        if (n == 0)
            return;

        // setup the operator chain
        int i = (int) n-1;
        BaseOperator* pOp = (*thePipeline)[i];
        pOp->prepare(NULL); 
        pOp->setExecutorContext(& executorCtx_);
        StreamOperator* pStreamOp = dynamic_cast<StreamOperator*>(pOp);
        if (pStreamOp!=NULL && pStreamOp->getStreamType()==1) {
            pOuputStreamList_->push_back(pStreamOp);
        }
        i--;

        while (i >= 0) {
            BaseOperator* curOp = (*thePipeline)[i];
            curOp-> prepare(pOp);
            curOp->setExecutorContext(& executorCtx_);
            pOp = curOp;

            pStreamOp = dynamic_cast<StreamOperator*>(curOp);
            if (pStreamOp!=NULL && pStreamOp->getStreamType()==0) {
                pSourceList_->push_back(pStreamOp);
            }
            else if (pStreamOp!=NULL && pStreamOp->getStreamType()==1) {
                pOuputStreamList_->push_back(pStreamOp);
            }
            i--;
        }

        // no need to setup ringbuffer for source stream
    }

    /**
     *  setup operator chain, also find out all source streams in pSourceList
     */
    void prepare() {
        if (! pPipelineList_ )
            return;

        // setup the operator chain
        for (vector<OperatorPipeline*>::const_iterator iter =  pPipelineList_->begin(); iter < pPipelineList_->end(); iter++) {
            OperatorPipeline* thePipeline =  *iter;
            prepareOnePipeline(thePipeline);
        }
    }


public:
    SimpleCommandsExecutor(vector<OperatorPipeline*>* pipelines) : pPipelineList_(pipelines), 
        pSourceList_(new vector<StreamOperator*>() ),
        pOuputStreamList_(new vector<StreamOperator*>()),
        seq_(0) {
            // this executor by default just sync every event
            setSyncByInterval(false);  
            executorCtx_.setExecutor(this);
            prepare();
    }

    SimpleCommandsExecutor(vector<OperatorPipeline*>* pipelines, bool syncByInterval) : pPipelineList_(pipelines), 
        pSourceList_(new vector<StreamOperator*>() ),
        pOuputStreamList_(new vector<StreamOperator*>()),
        seq_(0) {
            // this executor by default just sync every event
            setSyncByInterval(syncByInterval);  
            executorCtx_.setExecutor(this);
            prepare();
    }

    virtual ~SimpleCommandsExecutor() {
        delete pSourceList_;
        delete pOuputStreamList_;
    }


    void doStaticAnalysis() {
        AbstractCommandsExecutor::doStaticAnalysis(pPipelineList_);
    }

    virtual void start() {
        // nothing is needed
    }

    virtual void stop() {
        // nothing is needed
    }

    virtual vector<OperatorPipeline*>* getOperatorChains() {
        return pPipelineList_;
    }

    /**
     * Find a source stream operators by name
     * 
     * @param streamName the name of source stream to get
     * @param entries to receive entries of the source stream which could have multiple occurences within one epn
     * @return a vector containing the source stream operators matching this name
     */
    SourceStreamEntries& getSourceEntries(const char* streamName, SourceStreamEntries& entries) {
        entries.clear();
        if (pSourceList_->empty()) 
            return entries;        

        for (size_t i=0; i< pSourceList_->size(); i++) {
            // strip off '"' at both side of the name if there is
            string s = (*pSourceList_)[i]->getStreamName();
            if (s[0] == '"' && s[s.length()-1] == '"') {
                s = s.substr(1, s.length()-2);
            }
            if (0 == s.compare(streamName)) {
                entries.push_back((*pSourceList_)[i]);
            }
        }
        return entries;
    }

    /**
     * Find event sinks for one result stream by its name
     * 
     * @param streamName the name of result stream
     * @param entries to receive entries of the result stream event sink which could have multiple occurences within one epn
     * @return all event sinks found for the result stream
     */
    ResultStreamEntries& getEventSinks(const char* streamName, ResultStreamEntries& entries) {
        entries.clear();
        if (pOuputStreamList_->empty()) 
            return entries;

        StreamOperator* retStream = NULL;

        for (size_t i=0; i< pOuputStreamList_->size(); i++) {
            // strip off '"' at both side of the name if there is
            string s = (*pOuputStreamList_)[i]->getStreamName();
            if (s[0] == '"' && s[s.length()-1] == '"') {
                s = s.substr(1, s.length()-2);
            }

            if (0 == s.compare(streamName)) {
                retStream = (*pOuputStreamList_)[i];
                entries.push_back(retStream->getEventSink());                
            }
        }
        return entries;
    }

    /**
     * evaluate one event on a source stream
     */
    void execute(SourceStreamEntries& entries, Event* pEvent) {       
        for (SourceStreamEntries::const_iterator iter = entries.begin(); iter < entries.end(); iter++) {
            (*iter)->execute(seq_++, pEvent);        
        }
    }

    long64 getSequence() {
        return seq_;
    }
};


/**
 *  A struct used for grouping same name source stream operators together
 */
struct SourceStreamOperatorGroup {
    string streamName;
    vector<StreamOperator*> operators;
    bool needAllEvents;               // true:  these stream operators require all events, 
                                      //        imply at least one operator exploits non-roundrobin policy
                                      // false: all these stream operators exploit roundrobin policy.
};

/**
 * A struct to store all source stream operator instances within one EPN
 * these source stream operators are grouped together by their names.
 */
struct SourceStreamOperatorGroupList {
    vector<SourceStreamOperatorGroup*> theList;

    SourceStreamOperatorGroup* addSourceStreamOperator(StreamOperator* op){
        if (! op) 
            return NULL;
        
        // if the corresponding group doesn't exist,  create it
        SourceStreamOperatorGroup* retGroup = getGroup(op->getStreamName());
        if (! retGroup) {
            retGroup = new SourceStreamOperatorGroup();
            retGroup->streamName = op->getStreamName();
            retGroup->needAllEvents = op->getPartitionPolicy()->needAllEvent();
            theList.push_back(retGroup);
        }
        else {
            retGroup->needAllEvents = retGroup->needAllEvents || op->getPartitionPolicy()->needAllEvent();
        }
        retGroup->operators.push_back(op);
        return retGroup;
    }

    SourceStreamOperatorGroup* getGroup(const string& name) {
        for (vector<SourceStreamOperatorGroup*>::const_iterator iter = theList.begin(); iter < theList.end(); iter++) {
            if (0 == (*iter)->streamName.compare(name)) {
                return *iter;
            }
        }
        return NULL;
    }

    SourceStreamOperatorGroup* getGroup(int idx) { 
        return theList[idx];
    }

    int getGroupIndex(const string& name) {
        int i = -1;
        for (vector<SourceStreamOperatorGroup*>::const_iterator iter = theList.begin(); iter < theList.end(); iter++) {
            i++;
            if (0 == (*iter)->streamName.compare(name)) {
                break;
            }
        }
        return i;
    }


    ~SourceStreamOperatorGroupList() {
        for (vector<SourceStreamOperatorGroup*>::const_iterator iter = theList.begin(); iter < theList.end(); iter++) {
            delete (*iter);
        }
    }

    // delegate the call to one of its containing stream operator
    // since they should all be same
    int getParallelIndex() {
        return theList[0]->operators[0]->getParallelIndex();
    }

    // delegate the call to one of its containing stream operator
    // since they should all be same
    int getParallelism() {
        return theList[0]->operators[0]->getParallelism();
    }
};

/**
 * Multithreaded Executor classes
 * this executor allows to use multiple threads to execute a series commands
 */
 class ThreadedCommandsExecutor : public AbstractCommandsExecutor, public Watcher {
    typedef vector<OperatorPipeline*>* PipelineGroupPtr;
private:
    // operator list passed in by parser
    vector<OperatorPipeline*>* pPipelineList_;
    
    // cloned PipelineGroups(EPNs) for parallel run
    vector<PipelineGroupPtr> runnablePipelineGroups_;

    vector<SourceStreamOperatorGroupList*> *pSourceList_;
    vector<EventSink*> *pSinkList_;

    RbSourceStream* theRBStream_;

    // a list of executor context objects,  one for each parallel instance
    vector<ExecutorContext*> executorCtxList_;

    // disallow copy constructor
    ThreadedCommandsExecutor(const ThreadedCommandsExecutor &); 

private:
    /**
     * clone one operator chain instance from the compiled operator chain
     *
     * @ thePipeline one operator chain to be clined
     * @return the cloned pipeline (operator chain)
     */
    OperatorPipeline* cloneOnePipeline(OperatorPipeline* thePipeline) {
        OperatorPipeline* clonedPipeline = new OperatorPipeline();
        for (OperatorPipeline::const_iterator iter = thePipeline->begin(); iter < thePipeline->end(); iter++) {
            clonedPipeline->push_back((*iter)->clone());
        }
        return clonedPipeline;
    }

    /**
     * clone all pipelines within a EPN (PipelineGroup)
     *
     * @return the cloned PipelineGroup
     */
    PipelineGroupPtr clonePipelines() ;

    /**
     * destroy one operator chain instance
     */
    void destroyOnePipeline(OperatorPipeline* chain) {
        if (!chain) 
            return;
        for (OperatorPipeline::const_iterator iter = chain->begin(); iter < chain->end(); iter++) {
            delete (*iter);
        }
        delete chain;
    }

    /**
     * destroy all pipelines within a EPN (PipelineGroup)
     * 
     */
    void destroyPipelines(PipelineGroupPtr theGroup) {
        if (! theGroup)
            return;
        for (vector<OperatorPipeline*>::const_iterator iter = theGroup->begin(); iter < theGroup->end(); iter++) {
            destroyOnePipeline(*iter);
        }
        delete theGroup;
    }

    /**
     * get one single operator chain prepared
     */
    void prepare(OperatorPipeline* chain, int nParallelism, int index) {
        if (! chain)
            return;
        if (chain->empty())
            return;

        ExecutorContext* theCtxObj = executorCtxList_[index];

        // setup the operator chain
        OperatorPipeline::const_iterator iter = chain->begin();
        BaseOperator* curOp = *iter;
        iter++;
        for (; iter < chain->end(); iter++) {
            curOp->prepare(*iter);
            curOp->setExecutorContext(theCtxObj);

            // collect source stream operators and event sinks
            StreamOperator* pStreamOp = dynamic_cast<StreamOperator*>(curOp);
            if (pStreamOp!=NULL) {
                if (pStreamOp->getStreamType()==0) {
                    pStreamOp->setParallelIndex(nParallelism, index);
                    //pSourceList_->push_back(pStreamOp);
                    SourceStreamOperatorGroupList* theSSGroupList = (*pSourceList_)[index];
                    theSSGroupList->addSourceStreamOperator(pStreamOp);
                } else {
                    pSinkList_->push_back(pStreamOp->getEventSink());
                }
            }
            curOp = *iter;
        }
        // don't forget the last operator
        curOp->prepare(NULL);
        curOp->setExecutorContext(theCtxObj);
        StreamOperator* pStreamOp = dynamic_cast<StreamOperator*>(curOp);
        if (pStreamOp!=NULL && pStreamOp->getStreamType()==1) {
            pSinkList_->push_back(pStreamOp->getEventSink());
        }
    }
    

    // @override,  reap result events in this method
    virtual void doWatch(long64 sequence) {
        typedef vector<EventSinkEntry*> EventBatch;
        vector<EventBatch> availableEventBatches;
        vector<int> eventNums;

        availableEventBatches.resize(pSinkList_->size(),  EventBatch());  // init a number of empty batches
        int i = 0;        
        int num ;
        for (vector<EventSink*>::const_iterator iter = pSinkList_->begin(); iter < pSinkList_->end(); iter++) {
            num = (*iter)->getAvailableEvents(sequence, availableEventBatches[i]);
            eventNums.push_back(num);
            if (num >0) {
                i++;
            };
        }
        // remove the empty batches sitting at the end of the vector
        availableEventBatches.resize(i);

        while (! availableEventBatches.empty() ) {
            vector<EventBatch>::iterator minimal_iter = availableEventBatches.begin();
            long64 minimal_seq = minimal_iter->front()->getSequence();
            vector<EventBatch>::iterator iter = minimal_iter + 1;
            long64 seq1;
            for (; iter < availableEventBatches.end(); iter++) {
                seq1 = iter->front()->getSequence();
                if ( seq1 < minimal_seq) {
                    minimal_iter = iter;
                    minimal_seq = seq1;
                }
            }
            
            // output one event,  also erase it from the bag
            EventSinkEntry* entry = minimal_iter->front();
            //if (entry->getSequence() % 100000 == 0) {
                // only output events every 10000th
                // printf(">>> output 1 event: seq= %d, TS = %lld \n", entry->getSequence(), entry->ts_);
                printf(">>> output 1 event: (%c) seq= %d, TS = %lld \n", ((entry->type_== Event::TYPE_PLUS) ? '+' : '-'), entry->getSequence(), entry->ts_);
                printf("%s\n", entry->jsonText_.c_str());
            //}
            minimal_iter->erase(minimal_iter->begin());
            if (minimal_iter->empty()) {
                // if this bag has become empty, remove it
                availableEventBatches.erase(minimal_iter);
            }
        }

        // free events from the sinks
        vector<int>::const_iterator numsIter = eventNums.begin();
        for (vector<EventSink*>::const_iterator iter = pSinkList_->begin(); iter < pSinkList_->end(); iter++) {
            if ((*numsIter) > 0) {
                (*iter)->freeEvents(*numsIter);
            }
            numsIter++;
        }
    }
public:
    /**
     *  Construct a parallel executor
     *
     * @param pOperators the compiled command (operator chain)
     * @param nParallelism number of parallel instances, it must be great than 0
     */
    ThreadedCommandsExecutor(vector<OperatorPipeline*>* pipelines, unsigned int nParallelism) :
      pPipelineList_(pipelines),
      pSourceList_(new vector<SourceStreamOperatorGroupList*>()), 
      pSinkList_(new vector<EventSink*>()),
      theRBStream_(NULL) {
        // by default enable sync by interval (every 200000 events)
        setSyncByInterval(true); 
        doStaticAnalysis();
        for (unsigned int i = 0; i< nParallelism; i++) {
            runnablePipelineGroups_.push_back(clonePipelines());
            // prepare executor context objects
            ExecutorContext* aExecutorContext = new ExecutorContext(); 
            aExecutorContext->setExecutor(this);
            aExecutorContext->setParallelism(nParallelism);
            aExecutorContext->setparallelIndex(i);            
            executorCtxList_.push_back(aExecutorContext);
        }
    }

    /**
     * Destructor
     */
    virtual ~ThreadedCommandsExecutor() {
        // clear all cloned Pipeline groups
        for (vector<PipelineGroupPtr>::const_iterator iter = runnablePipelineGroups_.begin(); iter< runnablePipelineGroups_.end(); iter++) {
            destroyPipelines(*iter);
        }
        runnablePipelineGroups_.clear();
        
        for (vector<SourceStreamOperatorGroupList*>::const_iterator iter = pSourceList_->begin(); iter < pSourceList_->end(); iter++) {
            delete (*iter);
        }
        pSourceList_->clear();
        delete pSourceList_;
        delete pSinkList_;

        if (theRBStream_) {
            delete theRBStream_;
        }

        // destroy the executor context objects
        for (vector<ExecutorContext*>::const_iterator iter = executorCtxList_.begin(); iter < executorCtxList_.end(); iter++) {
            delete (*iter);
        }
    }

    void doStaticAnalysis() {
        AbstractCommandsExecutor::doStaticAnalysis(pPipelineList_);
    }

    // @ override
    virtual void prepare() {
        // setup the chainning for each operator chain 
        int nParallelism = getParallelism();
        int i = 0;
        for (vector<PipelineGroupPtr>::const_iterator iter = runnablePipelineGroups_.begin(); iter< runnablePipelineGroups_.end(); iter++) {
            // each parallel epn instance has one SourceStreamOperatorGroupList
            SourceStreamOperatorGroupList* theSSGroupList = new SourceStreamOperatorGroupList();
            pSourceList_->push_back(theSSGroupList);

            // loop each pipeline within the EPN, get it prepared.
            PipelineGroupPtr thePipelineGroup = *iter;
            for (vector<OperatorPipeline*>::const_iterator iter2 = thePipelineGroup->begin(); iter2 < thePipelineGroup->end(); iter2++) {
                prepare(*iter2, nParallelism, i);
            }            
            i++;
        }

        // prepare one RBStream and one subscriber
        if (theRBStream_) {
            delete theRBStream_;
        }
        theRBStream_ = new RbSourceStream(string(""), 8192);
        for (size_t i=0; i< pSourceList_->size(); i++) {
            SourceStreamOperatorGroupList* theSSGroupList = (*pSourceList_)[i];
            theRBStream_->addSubscriber(this, theSSGroupList);
        }

        // setup the watcher thread (result reap thread)
        theRBStream_->addSubscribersWatcher(this);

        // initialize publisher for each source streams
        // TODO: this needs to be moved to out side of the Executor, should be at network level.
        theRBStream_->initProducer();
    }

    // @ override
    virtual void start() {
        // start the ring buffer stream and its corresponding threads
        theRBStream_->start();
    }

    // @ override
    virtual void stop() {
        theRBStream_->stop();
    }

    virtual vector<OperatorPipeline*>* getOperatorChains() {
        return pPipelineList_;
    }

    /**
     * return current parallelism
     */
    int getParallelism() {
        return (int) runnablePipelineGroups_.size();
    }

    /**
     * get stream id by the stream name, it will be used in publish api
     *
     * @param streamName the name of stream
     * @return  a non-negative number will be returned if the stream exists, otherwise -1 will be returned
     */
    int getStreamId(const string& streamName) {
        if (pSourceList_->empty())
            return -1;
        SourceStreamOperatorGroupList* oneSSGroupList = (*pSourceList_)[0];
        return oneSSGroupList->getGroupIndex(streamName);
    }

    /**
     * publish one event with system time on to the specified stream
     *
     * @param streamId the id of stream where events should be published on to
     * @param stringEvent the string content of event
     * @return the number of event being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publishEvent(int streamId, char* stringEvent) {
        return theRBStream_->publish(streamId, stringEvent);
    }

    /**
     * publish one event on to the specified stream
     *
     * @param streamId the id of stream where events should be published on to
     * @param ts the timestamp of event
     * @param stringEvent the string content of event
     * @return the number of event being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publishEvent(int streamId, long64 ts, char* stringEvent) {
        return theRBStream_->publish(streamId, ts, stringEvent);
    }

    /**
     * publish one batch if events with system time on to the specified stream
     *
     * @param streamId the id of stream where events should be published on to
     * @param events the array of the events payloads
     * @param num the batch size
     * @return the number of event being published on success, 
     *         on fail, a negative error code will be returned
     */
    int publishEvent(int streamId, char** events, int num) {
        return theRBStream_->publish(streamId, events, num);
    }
};


// global variables
extern vector<BaseOperator*> * g_pCommands;


#endif