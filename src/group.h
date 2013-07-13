#ifndef _GROUP_H
#define _GROUP_H

#include <string>
#include <map>
#include "common.h"
#include "groupcontext.h"

using namespace std;

/**
 * A single field consisting of group key
 */
struct GroupKeyField {
    struct Data {
        union {
            long long l;
            unsigned long long ul;
            double d;
            bool b;
        };
        string s;  // to workarund Compiler Error C2621
    };

    enum DataType {
        gk_Null,    // null
        gk_Bool,    // boolean
        gk_Int,     // integer number
        gk_Uint,    // unsigned integer number
        gk_Double,  // double
        gk_String   // string
    };
    
    GroupKeyField() : type_(gk_Null) {        
    }

    /**
     * override < operator and so it can be used as key of a map
     */
    bool operator<(GroupKeyField const& other) const {
        if (type_ == other.type_) {
            bool rt;
            switch (type_) {
                case gk_Null :
                    rt = false;  // as it's equal
                    break;
                case gk_Bool :
                    rt = (data_.b ? 1 : 0) < (other.data_.b ? 1 : 0);
                    break;
                case gk_Int :
                    rt = (data_.l < other.data_.l);
                    break;
                case gk_Uint :
                    rt = (data_.ul < other.data_.ul);
                    break;
                case gk_Double :
                    rt = (data_.d < other.data_.d);
                    break;
                case gk_String :
                    rt = (data_.s.compare(other.data_.s) < 0);
                    break;
            }
            return rt;
        }
        else if (type_ < other.type_)
            return true;
        else  
            return false;
    }

    // data members
    DataType type_;
    Data data_;
};

/**
 * GroupKey consists of zero or multiple GroupKeyFields
 */
class GroupKey {
public:
    GroupKey() {
    }

    GroupKey(const GroupKey& other) : keyFields_(other.keyFields_) {
    }

    GroupKey(const vector<GroupKeyField>& keyFields) : keyFields_(keyFields) {
    }

    virtual ~GroupKey() {
        keyFields_.clear();
    }

    vector<GroupKeyField>& getKeyFields() {
        return keyFields_;
    }

    bool operator<(GroupKey const& other) const {
        int nSize = (int) keyFields_.size();
        
        for (int i = 0; i< nSize; i++) {
            if (keyFields_[i] < other.keyFields_[i]) {
                return true;
            }
            else if ( other.keyFields_[i] < keyFields_[i] ) {
                return false;
            }
        }
        return false;
    }
private:
    vector<GroupKeyField> keyFields_;
};


/**
 * group by exprssion parsed from a query
 * each group by expression consists of a expr and an optional varable name bound to the expr.
 */
class GroupExpr {
public:
    GroupExpr(Expr* expr, const char* varName) : expr_(expr), varName_(varName) {
    }

    ~GroupExpr() {
        delete expr_;
    }

    GroupExpr* clone() {
        Expr* clonedExpr = expr_->clone();
        return new GroupExpr(clonedExpr, varName_.c_str());
    }

    Variant& eval(BaseExecutionContext* pCtx) {
        return expr_->eval(pCtx);
    }
private:
    Expr*  expr_;
    string varName_;
};


/**
 *  Group Operator
 */
class GroupOperator : public BaseOperator {
    typedef map<GroupKey, GroupContext*> GroupContextMap;
    typedef map<GroupContext*, Document*> TempResultMap;
public:
    /**
     *  Group operator constructor. 
     *
     *  @param groupExprList a list of GroupExpr objects for computing group key fields, 
     *                       each GroupExpr consists of one expr (will be eval against input event), 
     *                       and one optinal variable name bound to the group expr result;
     *                       NULL or an empty list means aggregate into a single resulting group
     *  @param outExpr  aggregation result expr     
     */
    GroupOperator(vector<GroupExpr*>* groupExprList,  Expr* outExpr) : strValue_("Group"), groupExprList_(groupExprList), outExpr_(outExpr) {
        if (groupExprList_) {
            // init group field values
            GroupKeyField dumyValue;
            for (int i=0; i< (int) groupExprList_->size(); i++) {
                groupFieldValues_.push_back(dumyValue);
            }
            groupKey_ = new GroupKey(groupFieldValues_);
        }
        else {
            // init an empty group key
            groupKey_ = new GroupKey();
        }

    }

    virtual ~GroupOperator() {
        if (groupExprList_) {
            for (vector<GroupExpr*>::const_iterator iter = groupExprList_->begin(); iter < groupExprList_->end(); iter++) {
                delete (*iter);
            }
            groupExprList_->clear();
        }
        if (groupKey_)
            delete groupKey_;
        if (outExpr_) 
            delete outExpr_;

        // clean group context map
        GroupContextMap::iterator iter;
        for (iter= groupContextMap_.begin(); iter != groupContextMap_.end();)  {
            delete (iter->second);
            iter = groupContextMap_.erase(iter);
        }
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

        BaseOperator* clonedOperator = new GroupOperator(theClonedList, clonedOutExpr);
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    // @ovverride
    virtual string& toString() {
        return strValue_ ;
    }

    // @override
    virtual void execute(long64 seq, Event* pevent) {
        if (pevent->isFlushEvent()) {
            flushOutputs(seq, pevent);
            return;
        }
        else if (! pevent->isDataEvent()) {
            // directly pass down
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);
            }
            return;
        }

        // setup context
        context_.reset();
        context_.setCurrentEvent(pevent);

        // eval group expr  
        if (groupExprList_) {
            evalCurrentGroupKey(& context_);            
        }

        // find group context according to the current key which is just evaluated, 
        // add a new group context if nothing is found
        GroupContextMap::iterator iter = groupContextMap_.find(*groupKey_);
        GroupContext* theGroupContext;
        if (iter == groupContextMap_.end()) {
            theGroupContext = new GroupContext();
            groupContextMap_.insert(GroupContextMap::value_type(*groupKey_, theGroupContext));
        }
        else {
            theGroupContext = iter->second;
        }
        
        // update event count in the group
        if (pevent->getType() == Event::TYPE_PLUS) {
            theGroupContext->addOneEvent();
        }
        else if (pevent->getType() == Event::TYPE_MINUS) {
            theGroupContext->removeOneEvent();
        }
        
        // evaluate the result expr
        context_.setGroupContext(theGroupContext);
        
        /* 
         bug: in reducing a bag of events, temporarily group event number may become zero, it can cause the reduce operator produce wrong result if 
              it just return a null value when event number is zero.   This special case of "dealing with a group disapperance" should be delayed 
              until a flush event arrives, emit a special event to clean the group,  also clean the group context.
        */
        // Value& vout = (theGroupContext->getEventNumInGroup() > 0) ? outExpr_->eval(& context_).getValue() : nullValue_;
        Value& vout = outExpr_->eval(& context_).getValue() ;

        // copy this result document, save it into a temporary resulting map until a flush event arrives
        TempResultMap::iterator tempResultIter = tempResultMap_.find(theGroupContext);
        Document* theDoc;
        if (tempResultIter == tempResultMap_.end()) {
            theDoc = new Document();
            tempResultMap_.insert(TempResultMap::value_type(theGroupContext, theDoc));
        }
        else {
            theDoc = tempResultIter->second;
        }
        
        // need to call destructor first to avoid memleak
        theDoc->~Document();
        new (theDoc) Document();
        vout.CopyTo(*theDoc);
    }

private:
    /**
     * upon flush events,  output events to downstream operator
     *
     * @param seq driving event sequence
     * @param pevent driving event
     */
    void flushOutputs(long64 seq, Event* pevent) {
        IntermediateEvent theOutEvent;
        for (TempResultMap::iterator iter = tempResultMap_.begin(); iter != tempResultMap_.end();) {
            // output MINUS event first
            Event* lastResultEvent = iter->first->getResultEvent();
            if (lastResultEvent && !(lastResultEvent->getPayload()->IsNull())) {
                theOutEvent.setEvent(lastResultEvent->getPayload(), Event::TYPE_MINUS, pevent->getTimestamp());
                if (pNextOperator_) {
                    pNextOperator_->execute(seq, &theOutEvent);
                }
            }

            // output PLUS event
            Document* theDoc = iter->second;
            if (! theDoc->IsNull()) {
                theOutEvent.setEvent(iter->second, Event::TYPE_PLUS, pevent->getTimestamp());
                if (pNextOperator_) {
                    pNextOperator_->execute(seq, &theOutEvent);
                }
            }

            // update last result event in group context
            // note: the document ownership is moved into last result event  
            if (lastResultEvent) {
                lastResultEvent->update(theDoc, Event::TYPE_PLUS, pevent->getTimestamp());
            }
            else {
                lastResultEvent = new Event(theDoc, Event::TYPE_PLUS, pevent->getTimestamp());
                iter->first->setResultEvent(lastResultEvent);
            }

            if (theDoc == NULL || theDoc->IsNull()) {
                // TODO: the group context becoming empty,  should remove it
                // for now just leave it in there.
            }
            // remove this entry and prepare to process the next entry
            iter = tempResultMap_.erase(iter); 
        }
        // send one flush out event to end the bag
        theOutEvent.setEvent(NULL, Event::TYPE_FLUSH, pevent->getTimestamp());
        if (pNextOperator_) {
            pNextOperator_->execute(seq, &theOutEvent);
        }
    }

    void evalCurrentGroupKey(BaseExecutionContext* ctx) {
        if (!groupExprList_) 
            return;
        
        int i = 0;
        for (vector<GroupExpr*>::const_iterator iter = groupExprList_->begin(); iter < groupExprList_->end(); iter++, i++) {
            GroupExpr* theGroupExpr = *iter;
            Value& v = theGroupExpr->eval(ctx).getValue();
            GroupKeyField& theFieldValue = groupFieldValues_[i];
            switch (v.GetType()) {
            case kNullType:
                theFieldValue.type_ = GroupKeyField::gk_Null;
                break;
            case kFalseType:
            case kTrueType:
                theFieldValue.type_ = GroupKeyField::gk_Bool;
                theFieldValue.data_.b = v.GetBool();
                break;
            case kNumberType:
                if (v.IsInt()) {
                    theFieldValue.type_ = GroupKeyField::gk_Int;
                    theFieldValue.data_.l = v.GetInt();
                }
                else if (v.IsInt64()) {
                    theFieldValue.type_ = GroupKeyField::gk_Int;
                    theFieldValue.data_.l = v.GetInt64();
                }
                else if(v.IsUint()) {
                    theFieldValue.type_ = GroupKeyField::gk_Uint;
                    theFieldValue.data_.ul = v.GetUint();
                }
                else if(v.IsUint64()) {
                    theFieldValue.type_ = GroupKeyField::gk_Uint;
                    theFieldValue.data_.ul = v.GetUint64();
                }
                else if (v.IsDouble()) {
                    theFieldValue.type_ = GroupKeyField::gk_Double;
                    theFieldValue.data_.d = v.GetDouble();
                }
                break;
            case kStringType:
                theFieldValue.type_ = GroupKeyField::gk_String;
                theFieldValue.data_.s = v.GetString();
                break;
            default:
                // TODO:  other complex objects, serialze into string
                ;
            } // end switch

            // update groupKey_
            vector<GroupKeyField>& theKeyFields = groupKey_->getKeyFields();
            theKeyFields[i] = theFieldValue;
        } // end of loop        
    }
private:
    string strValue_;
    vector<GroupExpr*>* groupExprList_;
    Expr* outExpr_;
    BaseExecutionContext context_;
    
    vector<GroupKeyField> groupFieldValues_;   // store group by field values against the current event
    GroupKey* groupKey_;                       // store the current group key against current event
    map<GroupKey, GroupContext*> groupContextMap_;  // maintain an entry for each group
    map<GroupContext*, Document*> tempResultMap_;   // temporary result event map
    Value nullValue_;                               // dummy null value used when a group becoming empty
};

#endif