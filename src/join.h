#ifndef _JOIN_H
#define _JOIN_H

#include <string>
#include <unordered_map>
#include <functional>
#include <set>

using namespace std;

#ifdef WIN32
using namespace std::tr1;
#endif


#define TAB_WIDTH 4u

/*
 * This engine initially supports 2-way inner join, later it will be enhanced to support
 *  + multi-way inner join
 *  + 2-way or multi-way outer join
 *
 * Join is performed against 2 or more collections, and so the operator will retain events
 * for window branches.
 *
 * Proper index will be automatically created and used for these join cases. 
 *
 */

/**
 * EventBag is used as a return type in doing index lookup
 * it contains the payload of a set of events
 */
class EventBag : public set<Value*> {
};

/**
 * VariableBinding is used in doing index lookup
 */
struct VariableBinding{
    string name_;
    Value* pvalue_;
};


/**
 * index base interface
 */
class Index {
public:
    virtual ~Index() {
    }

    /**
     * event enter or leave the source collection, therefore index should be updated accordingly
     *
     * @param updatePayload payload of an event object used to update this index
     * @param add operation type,  true : add;  false : delete
     */
    virtual void update(Value* updatePayload, bool add) = 0;

    /**
     * do index lookup
     *
     * @param varBindings  named variables bounded with corresponding events for evaluating the index loopup key expression
     * @param allEventsBag point to the bag which contains all events of the datasource to be looked for
     * @return pointer to a bag of events.
     */
    virtual EventBag* lookup(const vector<VariableBinding>& lookupVariables, EventBag* allEventsBag) = 0;

    /**
     * to use/lookup this index, dependent variables/sources must be evaluable
     */
    virtual const vector<string>& dependentVariables() = 0;

    /**
     * print the plan into a string
     */
    virtual string& explainPlan(string& planStr, size_t& depth) = 0;
};



class JoinDataSource {
public:
    JoinDataSource(const char* connectPointName, const char* sourceName, bool preserve, bool isStream) :
          connectPointName_(connectPointName),
          sourceName_(sourceName),
          preserve_(preserve),
          isStream_(isStream), 
          indexList_() {
    }

    virtual ~JoinDataSource() {
        for (vector<Index*>::const_iterator iter= indexList_.begin(); iter < indexList_.end(); iter++) {
            delete (*iter);
        }
    }

    /**
     * get the datasource name
     */
    const string& getDataSourceName() {
        return sourceName_;
    }

    const string& getConnectPointName() {
        return connectPointName_;
    }

    bool isStreamSource() {
        return isStream_;
    }

    bool isPreserveSource() {
        return preserve_;
    }

    void setPreserve(bool value) {
        preserve_ = value;
    }

    /**
     * add one index for this datasource
     *
     * @return -1 when theIndex is NULL, otherwise return the position which can be used to get the index
     */
    int addIndex(Index* theIndex) {
        if (theIndex == NULL)
            return -1;
        indexList_.push_back(theIndex);
        return ((int)indexList_.size() - 1);
    }

    /**
     * retrieve index through its id
     */
    Index* getIndex(int id) {
        if (id <0)
            return NULL;

        return indexList_[id];
    }

    void update(Event* pevent) {
        if (isStream_)
            return;

        // only update event bag and indexes for non-stream datasources
        Value* payload = pevent->getPayload();
        if(pevent->getType() == Event::TYPE_PLUS) {
            allEventsBag_.insert(payload);
            for (vector<Index*>::const_iterator iter = indexList_.begin(); iter < indexList_.end(); iter++) {
                (*iter)->update(payload, true);
            }
        }
        else if (pevent->getType() == Event::TYPE_MINUS) {
            allEventsBag_.erase(payload);
            for (vector<Index*>::const_iterator iter = indexList_.begin(); iter < indexList_.end(); iter++) {
                (*iter)->update(payload, false);
            }
        }
    }

    /**
     * lookup this datasource according to via a index
     *
     * @param indexId the id of the index to be used for this lookup.  
     *        -1 means no index can be used, i.e. fall back to full scan
     * @param lookupVariables a list of variables used for evaluating index lookup key expression
     *
     * @return a set of events, or a null pointer or an empty set if no events can be returned.
     */
    EventBag* doLookup(int indexId,  const vector<VariableBinding>& lookupVariables ) {
        EventBag* retBag = NULL;

        if (indexId == -1) {
            // do full scan
            retBag = &allEventsBag_;
        }
        else {
            // do index lookup
            Index* theIndex = indexList_[indexId];
            retBag = theIndex->lookup(lookupVariables, &allEventsBag_);
        }
        return retBag;
    }

    /**
     * clone the datasource
     */
    JoinDataSource* clone() {
        JoinDataSource* theClonedDatasource = new JoinDataSource(connectPointName_.c_str(), sourceName_.c_str(), preserve_, isStream_);
        // index needs to be rebuilt by the cloned JoinOperator
        return theClonedDatasource;
    }
private:
    string connectPointName_;  // the name of ConnectPoint where this datasource refers to
    string sourceName_;        // the name of the source
    bool preserve_;            // true means behave as outer join source
    bool isStream_;            // true means this is just a stream source instead of a window source
    vector<Index*> indexList_; // indecies associated with the datasource
    EventBag allEventsBag_;    // contain all events in this datasource
};


/**
 * Join plan definitions
 *
 * e.g. a 3 window join plan can be organized as below.
 * (+)SubJoinPlan for datasource1 when it's active
 *     |
 *     ---> Index lookup on datasource2 (IndexPlan)
 *           |
 *           ---> Index lookup on datasource3 (IndexPlan)
 *
 * (+)SubJoinPlan for datasource2 when it's active
 *     |
 *     ---> Index lookup on datasource1 (IndexPlan)
 *           |
 *           ---> Index lookup on datasource3 (WindowFullScan)
 *
 * (+)SubJoinPlan for datasource3 when it's active
 *     |
 *     ---> Index lookup on datasource2 (IndexPlan)
 *           |
 *           ---> Index lookup on datasource1 (WindowFullScan)
 *
 *
 */
struct IndexPlan {
    int datasourceId_;                 // id for accessing datasource, 0 based 
    int indexId_;                      // id for accessing index within this datasource,  0 based
};
struct SubJoinPlan {
    int activeDatasourceId_;           // id for access datasource,  0 based
    vector<IndexPlan> lookupPlans_;    // lookup plans for one active datasource
};
typedef vector<SubJoinPlan> JoinPlan;  // the complete index plan of a join operator


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////               Hash index and Union index implementation                                               ////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Key type for hash index
 */
struct HashIndexKey {
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
        kt_Null,    // null
        kt_Bool,    // boolean
        kt_Int,     // integer number
        kt_Uint,    // unsigned integer number
        kt_Double,  // double
        kt_String   // string
    };

    // data members
    DataType type_;
    Data data_;

    HashIndexKey() : type_(kt_Null) {
    }

    /**
     * override == operator so that it can be used as key of unordered_map
     */
    bool operator==(const HashIndexKey& other) const {
        if (type_ == other.type_) {
            bool rt;
            switch (type_) {
                case kt_Null :
                    rt = true;  // as it's equal
                    break;
                case kt_Bool :
                    rt = (data_.b  == other.data_.b);
                    break;
                case kt_Int :
                    rt = (data_.l == other.data_.l);
                    break;
                case kt_Uint :
                    rt = (data_.ul == other.data_.ul);
                    break;
                case kt_Double :
                    rt = (data_.d == other.data_.d);
                    break;
                case kt_String :
                    rt = (data_.s.compare(other.data_.s) == 0);
                    break;
            }
            return rt;            
        }
        else
            return false;
    }
};


class IndexKeyHasher {
public:
    size_t operator()(const HashIndexKey& k) const {
        size_t rt;
        switch (k.type_) {
            case HashIndexKey::kt_Null :
                rt = hash<int>()(0);
                break;
            case HashIndexKey::kt_Bool :
                rt = hash<bool>()(k.data_.b);
                break;
            case HashIndexKey::kt_Int :
                rt = hash<long long>()(k.data_.l);
                break;
            case HashIndexKey::kt_Uint :
                rt = hash<long long>()(k.data_.ul);
                break;
            case HashIndexKey::kt_Double :
                rt = hash<double>()(k.data_.d);
                break;
            case HashIndexKey::kt_String :
                rt = hash<string>()(k.data_.s);
                break;
        }  
        return rt;
    }
};

/**
 * a index built with an expression result hash value
 */
class HashIndex : public Index {
    typedef unordered_map<HashIndexKey, EventBag*, IndexKeyHasher> IndexDataMap;
public:
    HashIndex() : updateExpr_(NULL), lookupExpr_(NULL) {
    }

    HashIndex(Expr* updateExpr, Expr* lookupExpr, const string & datasourceName, const vector<string>& dependentVars) : 
        updateExpr_(updateExpr), 
        lookupExpr_(lookupExpr),
        datasourceName_(datasourceName),
        dependentVars_(dependentVars){
    }

    virtual ~HashIndex() {
        for (IndexDataMap::iterator iter = indexData_.begin(); iter != indexData_.end(); ) {
            delete (iter->second);
            iter = indexData_.erase(iter);
        }
    }

    void setUpdateExpression(Expr* expr) {
        updateExpr_ = expr;
    }

    void setLookupExpression(Expr* expr) {
        lookupExpr_ = expr;
    }

    // @override
    virtual const vector<string>& dependentVariables() {
        return dependentVars_;
    }

    // @override
    virtual void update(Value* updatePayload, bool add) {
        // evaluate the update key expression
        context_.reset();
        context_.setVariable(datasourceName_, updatePayload);
        Value& v = updateExpr_->eval(&context_).getValue();
        
        switch (v.GetType()) {
        case kNullType:
            indexKey_.type_ = HashIndexKey::kt_Null;
            break;
        case kFalseType:
        case kTrueType:
            indexKey_.type_ = HashIndexKey::kt_Bool;
            indexKey_.data_.b = v.GetBool();
            break;
        case kNumberType:
            if (v.IsInt()) {
                indexKey_.type_ = HashIndexKey::kt_Int;
                indexKey_.data_.l = v.GetInt();
            }
            else if (v.IsInt64()) {
                indexKey_.type_ = HashIndexKey::kt_Int;
                indexKey_.data_.l = v.GetInt64();
            }
            else if (v.IsUint()) {
                indexKey_.type_ = HashIndexKey::kt_Uint;
                indexKey_.data_.ul = v.GetUint();
            }
            else if (v.IsUint64()) {
                indexKey_.type_ = HashIndexKey::kt_Uint;
                indexKey_.data_.ul = v.GetUint64();
            }
            else if (v.IsDouble()) {
                indexKey_.type_ = HashIndexKey::kt_Double;
                indexKey_.data_.d = v.GetDouble();
            }
            break;
        case kStringType:
            indexKey_.type_ = HashIndexKey::kt_String;
            indexKey_.data_.s = v.GetString();
            break;
        default:
            // TODO:  other complex objects, serialze into string
            ;
        } // endd of switch

        EventBag* theBag;
        if (add) {
            // add one event into index
            IndexDataMap::iterator iter = indexData_.find(indexKey_);            
            if (iter == indexData_.end()) {
                theBag = new EventBag();
                theBag->insert(updatePayload);
                indexData_.insert(IndexDataMap::value_type(indexKey_, theBag));
            }
            else {
                theBag = iter->second;
                theBag->insert(updatePayload);
            }
        }
        else {
            // remove one event from index
            IndexDataMap::iterator iter = indexData_.find(indexKey_);
            if (iter != indexData_.end()) {
                theBag = iter->second;
                theBag->erase(updatePayload);
                if (theBag->empty()) {
                    indexData_.erase(iter);
                    delete theBag;
                }
            }
        }

    }

    // @override
    virtual EventBag* lookup(const vector<VariableBinding>& lookupVariables, EventBag* allEventsBag) {
        // evaluate the lookup key expression
        context_.reset();
        for (vector<VariableBinding>::const_iterator iter = lookupVariables.begin(); iter < lookupVariables.end(); iter++) {
            context_.setVariable(iter->name_, iter->pvalue_);
        }
        Value& v = lookupExpr_->eval(&context_).getValue();

        switch (v.GetType()) {
        case kNullType:
            indexKey_.type_ = HashIndexKey::kt_Null;
            break;
        case kFalseType:
        case kTrueType:
            indexKey_.type_ = HashIndexKey::kt_Bool;
            indexKey_.data_.b = v.GetBool();
            break;
        case kNumberType:
            if (v.IsInt()) {
                indexKey_.type_ = HashIndexKey::kt_Int;
                indexKey_.data_.l = v.GetInt();
            }
            else if (v.IsInt64()) {
                indexKey_.type_ = HashIndexKey::kt_Int;
                indexKey_.data_.l = v.GetInt64();
            }
            else if (v.IsUint()) {
                indexKey_.type_ = HashIndexKey::kt_Uint;
                indexKey_.data_.ul = v.GetUint();
            }
            else if (v.IsUint64()) {
                indexKey_.type_ = HashIndexKey::kt_Uint;
                indexKey_.data_.ul = v.GetUint64();
            }
            else if (v.IsDouble()) {
                indexKey_.type_ = HashIndexKey::kt_Double;
                indexKey_.data_.d = v.GetDouble();
            }
            break;
        case kStringType:
            indexKey_.type_ = HashIndexKey::kt_String;
            indexKey_.data_.s = v.GetString();
            break;
        default:
            // TODO:  other complex objects, serialze into string
            ;
        } // endd of switch

        EventBag* theBag = &emptyBag_;
        IndexDataMap::iterator iter = indexData_.find(indexKey_);
        if (iter != indexData_.end()) {
            theBag = iter->second;
        }
        return theBag;
    }


    // @override
    virtual string& explainPlan(string& planStr, size_t& depth) {
        string varList = dependentVars_[0];
        for (int i=1; i<dependentVars_.size(); i++){
            varList.append(", ").append(dependentVars_[i]);
        }
        planStr.append(depth * TAB_WIDTH, ' ').append("[").append(datasourceName_).append("] : ").append("HashIndex_Lookup(");
        planStr.append(varList).append(")\n");
        return planStr;
    }

private:
    Expr* updateExpr_;                     // expr used in maintaining the index
    Expr* lookupExpr_;                     // expr used in lookup the index
    string datasourceName_;                // name of datasource the index is associated with
    vector<string> dependentVars_ ;        // dependent datasource names

    BaseExecutionContext context_;         // expr evaluation context
    HashIndexKey indexKey_;                // index key
    IndexDataMap indexData_;               // index data container
    EventBag emptyBag_;                    // when index doesn't find any matching event, return this empty bag
};


/**
 *  A binary compositive index, it unior results from 2 index result, and remove the duplicates
 */
class UnionIndex : public Index {
public:
    UnionIndex() : left_(NULL), right_(NULL) {
    }

    UnionIndex(Index* left, Index* right, const vector<string>& dependentVars) : 
        left_(left), 
        right_(right),
        dependentVars_(dependentVars) {
    }

    virtual ~UnionIndex() {
        if (left_)
            delete left_;
        if (right_)
            delete right_;
    }

    // @override
    virtual const vector<string>& dependentVariables() {
        return dependentVars_;
    }

    // @override
    virtual void update(Value* updatePayload, bool add) {
        left_->update(updatePayload, add);
        right_->update(updatePayload, add);
    }

    // @override
    virtual EventBag* lookup(const vector<VariableBinding>& lookupVariables, EventBag* allEventsBag) {
        EventBag* leftBag_  = left_->lookup(lookupVariables, allEventsBag);
        EventBag* rightBag_ = right_->lookup(lookupVariables, allEventsBag);

        resultBag_.clear();
        if ( !leftBag_->empty() ) {
            resultBag_.insert(leftBag_->begin(), leftBag_->end());
        }
        if ( !rightBag_->empty() ) {
            resultBag_.insert(rightBag_->begin(), rightBag_->end());
        }
        return (&resultBag_);
    }

    // @override
    virtual string& explainPlan(string& planStr, size_t& depth) {
        planStr.append(depth * TAB_WIDTH, ' ').append("UnionIndex_Lookup").append("\n");
        depth++;
        left_->explainPlan(planStr, depth);
        right_->explainPlan(planStr, depth);
        depth--;

        return planStr;
    }
private:
    Index* left_;                          // one index to be unioned
    Index* right_;                         // the other index to be unioned
    vector<string> dependentVars_ ;        // dependent datasource names
    EventBag resultBag_;                   // put unioned result here
};


/**
 * A index which only be used for lookup when the conditional expression is evaluated to false
 * this type of index will be used in the pattern like (expr(win_var1) = expr(win_var2)) or expr(win_var2)
 * when win_var is known, a conditonal index can be used to lookup win_var1
 */
class ConditionalIndex : public Index {
public:
    ConditionalIndex() : realIndex_(NULL), condition_(NULL){
    }

    ConditionalIndex(Index* aIndex, Expr* condition, const vector<string>& dependentVars) :
        realIndex_(aIndex), 
        condition_(condition),
        dependentVars_(dependentVars) {
    }

    virtual ~ConditionalIndex() {
        if (realIndex_)
            delete realIndex_;        
    }

    // @override
    virtual const vector<string>& dependentVariables() {
        return dependentVars_;
    }

    // @override
    virtual void update(Value* updatePayload, bool add) {
        realIndex_->update(updatePayload, add);
    }

    // @override
    virtual EventBag* lookup(const vector<VariableBinding>& lookupVariables, EventBag* allEventsBag) {
        // evaluate the condition expression
        context_.reset();
        for (vector<VariableBinding>::const_iterator iter = lookupVariables.begin(); iter < lookupVariables.end(); iter++) {
            context_.setVariable(iter->name_, iter->pvalue_);
        }

        Value& v = condition_->eval(&context_).getValue();
        if (! v.GetBool()) {
            EventBag* theBag  = realIndex_->lookup(lookupVariables, allEventsBag);
            return theBag;
        }
        else {
            // return the entire datasource (full scan)
            return allEventsBag;
        }
    }

    // @override
    virtual string& explainPlan(string& planStr, size_t& depth) {
        planStr.append(depth * TAB_WIDTH, ' ').append("ConditionIndex_Lookup").append("\n");
        depth++;
        realIndex_->explainPlan(planStr, depth);
        depth--;

        return planStr;
    }
private:
    BaseExecutionContext context_;          // expr evaluation context

    Index* realIndex_;                      // the real index to be used
    Expr* condition_;                       // expression determing whether realIndex can be used
    vector<string> dependentVars_;          // dependent datasource names
};

class IndexPlanBuilder {
protected:
    /**
     * subset relationship check
     *
     * @return true if list1 is subset of list2
     */
    bool isSubset(const vector<string>& list1, const vector<string>& list2) {
        if (list1.empty())
            return true;
        
        for (vector<string>::const_iterator iter1 = list1.begin(); iter1 < list1.end(); iter1++) {
            bool found = false;
            for (vector<string>::const_iterator iter2 = list2.begin(); iter2 < list2.end(); iter2++) {
                if (iter2->compare(*iter1) == 0) {
                    found = true;
                    break;
                }
            }

            if (! found)
                return false;
        }

        // found a matching one from list2 for each item in list1 
        return true;
    }
public:
    /**
     * Analyze join condition, if possible generate appropriate index for the loopup window
     *
     * @param expr  pointer of the join condition expression
     * @param loopupWindowName the name of window the index analysis to be performed against
     * @param queryWindowList  the window name list of which events will be used to evaluate index key if possible.
     */
    Index* buildPlan(Expr* expr, const string& lookupWindowName,  const vector<string>& queryWindowList) {
        BinaryExpr* binExpr = dynamic_cast<BinaryExpr*> (expr);
        if (binExpr == NULL) 
            return NULL;

        Index* retIndex = NULL;

        /* rule #1: equal operation
             expr(var_lookup) = expr(var_known)
             expr(var_lookup) = expr(const)
         */
        if (binExpr->getOperator() == Expr::OP_EQ) {
            vector<string> varList1;
            vector<string> varList2;
            binExpr->getLeftExpr()->extractVariables(varList1);
            binExpr->getRightExpr()->extractVariables(varList2);

            if ((varList1.size() == 1) && 
                (varList1.front().compare(lookupWindowName) ==0) && 
                isSubset(varList2, queryWindowList) ) {
                // a hash index can be created
                // using left expr to evaluate index key in updating this index
                // using right expr to evaluate index key in doing index lookup
                retIndex = new HashIndex(binExpr->getLeftExpr(), binExpr->getRightExpr(), lookupWindowName, queryWindowList);
            }
            else if ((varList2.size() == 1) &&
                (varList2.front().compare(lookupWindowName) ==0) && 
                isSubset(varList1, queryWindowList) ) {
                // a hash index can be created
                // using right expr to evaluate index key in updating this index
                // using left expr to evaluate index key in doing index lookup
                retIndex = new HashIndex(binExpr->getRightExpr(), binExpr->getLeftExpr(), lookupWindowName, queryWindowList);
            }
            else
                retIndex = NULL;

            return retIndex;
        }  // end of rule #1
    

        /* rule #2: and operation
         a) pattern_1 :   expr1 and expr2
            check order:  expr1 -> expr2 

                  AND
                /     \
             expr1    expr2

         b) pattern_2 :   expr1 and expr2 and expr3  <==>  (expr1 and expr2) and expr3
            check order:  expr1 -> expr2 -> expr3

                  AND
                /     \
             AND       expr3
            /   \       
        expr1  expr2  

         c) pattern_3 :   expr1 and (expr2 and expr3)
            check order : expr1 -> expr2 -> expr3

                   AND
                /      \
             expr1    AND
                     /    \
                  expr2  expr3 

         d) pattern_4 :   (expr1 and expr2) and (expr3 and expr4)
            check order : expr1 -> expr2 -> expr3 -> expr4

                   AND
                /        \
             AND         AND
            /   \       /    \
        expr1  expr2  expr3  expr4

        ...

        */
        if (binExpr->getOperator() == Expr::OP_AND) {
            // left expression takes privilege than the right one
            retIndex = buildPlan(binExpr->getLeftExpr(), lookupWindowName, queryWindowList);
            if (retIndex)
                return retIndex;

            // analyze the right expression when left one cannot utilize index
            retIndex = buildPlan(binExpr->getRightExpr(), lookupWindowName, queryWindowList);
            return retIndex;
        } // end of rule #2


        /* rule #3.  or operation
         a) pattern_1   :   expr1 or expr2
            Union index :   expr1 , expr2

                  OR
                /     \
             expr1    expr2

         b) pattern_2   :   expr1 or expr2 or expr3
            Union index :   cannot be indexed

                  OR
                /     \
             OR       expr3
            /   \       
        expr1  expr2  

         c) pattern_3   :   expr1 and expr2 or expr3 <==> (expr1 and expr2) or expr3
            Union index :   expr1/expr2 , expr3

                  OR
                /     \
              AND       expr3
            /   \       
        expr1  expr2  

         d) pattern_4   :   (expr1 or expr2) and expr3 
            Union index :   expr1,expr2 
                or
            Hash index  :   expr3

                  AND
                /     \
              OR       expr3
            /   \       
        expr1  expr2  

         e) pattern_5 :   (expr1 and expr2) or (expr3 and expr4) 
            Union index :   expr1/expr2 , expr3/expr4

                   OR
                /       \
             AND         AND
            /   \       /    \
        expr1  expr2  expr3  expr4

         */
        if (binExpr->getOperator() == Expr::OP_OR) {
            // when both sides expr are relevent to this window in query, use union index
            Index* leftIndex = buildPlan(binExpr->getLeftExpr(), lookupWindowName, queryWindowList);
            Index* rightIndex = buildPlan(binExpr->getRightExpr(), lookupWindowName, queryWindowList);
            HashIndex* leftHashIndex = dynamic_cast<HashIndex*> (leftIndex);
            HashIndex* rightHashIndex = dynamic_cast<HashIndex*> (rightIndex);

            // index is created only when both sides can have/use index
            if (leftHashIndex && rightHashIndex) {
                retIndex = new UnionIndex(leftHashIndex, rightHashIndex, queryWindowList);                
            }
            else {
                retIndex = NULL;
                if (leftIndex) 
                    delete leftIndex;
                if (rightIndex)
                    delete rightIndex;
            }

            // when only one side expr is relevent to this window in query, use condition index
            if (! retIndex) {
                vector<string> winVarList;
                winVarList.push_back(lookupWindowName);

                vector<string> varList1;
                vector<string> varList2;
                binExpr->getLeftExpr()->extractVariables(varList1);
                binExpr->getRightExpr()->extractVariables(varList2);
                Index* tempIndex;
                if (! isSubset(winVarList, varList1)) {
                    // when left side expr is not relevent to this window in query, only use index built by right side expression
                    tempIndex =  buildPlan(binExpr->getRightExpr(), lookupWindowName, queryWindowList);
                    if (tempIndex) {
                        retIndex = new ConditionalIndex(tempIndex, binExpr->getLeftExpr(), queryWindowList);
                    }
                }
                else if (! isSubset(winVarList, varList2)) {
                    // when right side expr is not relevent to this window in query, only use index built by left side expression
                    tempIndex =  buildPlan(binExpr->getLeftExpr(), lookupWindowName, queryWindowList);
                    if (tempIndex) {
                        retIndex = new ConditionalIndex(tempIndex, binExpr->getRightExpr(), queryWindowList);
                    }
                }
                else {
                    retIndex = NULL;
                }                
            }

            return retIndex;
        }  // end OR operator analysis

        // no index for the rest cases
        return NULL;
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////               Operators implementation                                                                ////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Join operator
 */
class JoinOperator : public BaseOperator {
public:
    JoinOperator(vector<JoinDataSource*>* datasourceList, Expr* conditionExpr, Expr* resultExpr) :
         datasourceList_(datasourceList),
         conditionExpr_(conditionExpr),
         resultExpr_(resultExpr) {
         
         // join is a mutable operator
         isMutable_ = true; 
         // do index plan analysis
         buildJoinPlan();
    };

    virtual ~JoinOperator() {
        if (conditionExpr_)
            delete conditionExpr_;
        if (resultExpr_)
            delete resultExpr_;
        
        if (datasourceList_) {
            while ( ! datasourceList_->empty()) {
                delete(datasourceList_->back());
                datasourceList_->pop_back();
            }
        }
    }


    /**
     * add a datasource into the list, return its position (0 based)
     * when the datasource is already in the list,  directly returns its position without adding it again
     * when the datasource is NULL, return -1
     * 
     */
    int addDataSource(JoinDataSource* theDatasource) {
        if (theDatasource == NULL)
            return -1;

        // ensure the datasource is not in the list
        int i = 0;
        for (vector<JoinDataSource*>::const_iterator iter = datasourceList_->begin(); iter < datasourceList_->end(); iter++, i++) {
            if (theDatasource == *iter)
                return i;
        }

        datasourceList_->push_back(theDatasource);
        return ((int)datasourceList_->size() - 1);
    }

    /**
     * get a datasource via its id
     */
    JoinDataSource* getDataSource(int id) {
        if ((id > -1) && (id < datasourceList_->size()))
            return (*datasourceList_)[id];
        else
            return NULL;
    }

    int getDatasSourceNum() {
        if (datasourceList_)
            return (int) datasourceList_->size();
        else
            return 0;
    }

    // @ovverride
    virtual string& toString() {
        return strValue_ ;
    }

    // @override
    virtual BaseOperator* clone() {
        Expr* clonedConditionExpr = conditionExpr_->clone();
        Expr* clonedResultExpr = resultExpr_->clone();
        vector<JoinDataSource*>* clonedDatasouceList = new vector<JoinDataSource*>();
        for (vector<JoinDataSource*>::const_iterator iter = datasourceList_->begin(); iter < datasourceList_->end(); iter++) {
            JoinDataSource* theDatasource = *iter;
            clonedDatasouceList->push_back(theDatasource->clone());
        }
        JoinOperator* clonedOperator = new JoinOperator(clonedDatasouceList, clonedConditionExpr, clonedResultExpr);
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    // @override
    // join operator will only be invoked through JoinInputProxyOperator
    // this method won't be called
    virtual void execute(long64 seq, Event* pevent) {
    }

    /**
     * The operator execution body
     * it choose a sub join plan according to the active datasource
     * meanwhile, it also maitains the collection of the active datasource
     *
     * @param inputId  the active datasource id (0 based)
     * @param seq      the event sequence number
     * @param pevent   the event
     */
    void execute(int inputId, long64 seq, Event* pevent) {
        if (pevent->isClearEvent()) {
            // block clear event from propagating
            return;
        }
        else if (!(pevent->isDataEvent())) {
            // pass through other control events
            if (pNextOperator_) {
                 pNextOperator_->execute(seq, pevent);
            }
            return;
        }

        // update the active datasource
        SubJoinPlan& activeSubPlan = plan_[inputId];
        JoinDataSource* activeDataSource = getDataSource(activeSubPlan.activeDatasourceId_);
        activeDataSource->update(pevent);

        // init the variable binding list
        vector<VariableBinding> varList;
        VariableBinding theVar;
        int num = (int) activeSubPlan.lookupPlans_.size();
        theVar.name_ = activeDataSource->getDataSourceName();
        theVar.pvalue_ = pevent->getPayload();
        varList.push_back(theVar);
        for (int i=0; i<num; i++) {
            theVar.name_ = getDataSource(activeSubPlan.lookupPlans_[i].datasourceId_)->getDataSourceName();
            theVar.pvalue_ = &nullValue_;
            varList.push_back(theVar);
        }

        // when event comming from a stream, need to send clear event for downstream group operator
        if (activeDataSource->isStreamSource() && pNextOperator_) {
            tempClearEvent_.update(NULL, Event::TYPE_CLEAR, pevent->getTimestamp());
            pNextOperator_->execute(seq, &tempClearEvent_);
        }

        // recursively do lookup each datasource and produce result events, push to next operator
        long resultNum = 0;
        doLookup(seq, pevent, activeSubPlan, 0, varList, resultNum); 

        // when event comming from a stream,  need to send a flush event for downstream group operator
        if (activeDataSource->isStreamSource() && pNextOperator_) {
            tempFlushEvent_.update(NULL, Event::TYPE_FLUSH, pevent->getTimestamp());
            pNextOperator_->execute(seq, &tempFlushEvent_);
        }
    }
    

    string& explainPlan(string& planStr) {        
        int dsNum = getDatasSourceNum();        
        for (int activeDsId = 0; activeDsId < dsNum ; activeDsId++) {
            JoinDataSource* activeDs = getDataSource(activeDsId);
            SubJoinPlan activeSubPlan = plan_[activeDsId];
            planStr.append("(*)-> ").append(activeDs->getDataSourceName()).append("\n");
            
            size_t depth = 0;
            for (int i=0; i<activeSubPlan.lookupPlans_.size(); i++){
                depth++;
                JoinDataSource* passiveDs = getDataSource(activeSubPlan.lookupPlans_[i].datasourceId_);
                Index* theIndex = passiveDs->getIndex(activeSubPlan.lookupPlans_[i].indexId_);
                if (theIndex)
                    theIndex->explainPlan(planStr, depth);
                else {
                    if (passiveDs->isStreamSource()) 
                        planStr.append(depth * TAB_WIDTH, ' ').append("[").append(passiveDs->getDataSourceName()).append("] : ").append("Stream StopLoop").append("\n");
                    else
                        planStr.append(depth * TAB_WIDTH, ' ').append("[").append(passiveDs->getDataSourceName()).append("] : ").append("FullScan").append("\n");
                }
            }
        }

        return planStr;
    }
protected:
    void doLookup(long64 seq, Event* pevent, const SubJoinPlan& activeSubPlan, int lookupPlanId, vector<VariableBinding>& varList, long& resultNum) {
        if (lookupPlanId > activeSubPlan.lookupPlans_.size())
            return;
        else if (lookupPlanId == activeSubPlan.lookupPlans_.size()) {
            // all variable bindings have been setup, ready to evaluate result event
            doResult(seq, pevent, varList);
            resultNum++;
            return;
        }

        // get the lookup plan
        const IndexPlan& lookupPlan = activeSubPlan.lookupPlans_[lookupPlanId];
        JoinDataSource* theDataSource = getDataSource(lookupPlan.datasourceId_);
        EventBag* theBag = theDataSource->doLookup(lookupPlan.indexId_, varList);
        if (theBag && (theBag->size() > 0)) {
            // loop each event successfully joined and then recursively lookup the next datasource
            for (EventBag::const_iterator iter = theBag->begin(); iter != theBag->end(); iter++) {
                varList[lookupPlanId +1].pvalue_ = *iter;
                doLookup(seq, pevent, activeSubPlan, lookupPlanId + 1, varList, resultNum);
            }
        }
        if (theDataSource->isPreserveSource() && (resultNum == 0) ) {
            // outer join this source, recursively lookup the next datasource
            varList[lookupPlanId + 1].pvalue_ = &nullValue_;
            doLookup(seq, pevent, activeSubPlan, lookupPlanId + 1, varList, resultNum);
        }
        else {        
            // when code reaches here, this join wont produce any result
        }
    }

    void doResult(long64 seq, Event* pevent, const vector<VariableBinding>& varList) {
        // setup context
        context_.reset();
        context_.setCurrentEvent(pevent);
        for (vector<VariableBinding>::const_iterator iter = varList.begin(); iter < varList.end(); iter++) {
            context_.setVariable(iter->name_, iter->pvalue_);
        }

        // evaluate where condition for inner join
        // only outer join will have null value, for now only support "stream left outer join window"
        // so it's safe to skip the evaluation when any one of first 2 values is NULL
        if (!(varList[0].pvalue_->IsNull() || varList[1].pvalue_->IsNull())) {
            if (! conditionExpr_->eval(&context_).getValue().GetBool())
                return;
        }

        // evaluate result expression 
        // wrap its result into a Event object for downstream operators
        Value& v = resultExpr_->eval(&context_).getValue();
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

    void buildJoinPlan() {
        if (!plan_.empty())
            return;

        if (datasourceList_->size() < 2)
            return;
        
        int i = 0;
        IndexPlanBuilder builder;
        for (vector<JoinDataSource*>::const_iterator iter = datasourceList_->begin(); iter < datasourceList_->end(); iter++, i++) {
            JoinDataSource* activeDatasource = *iter;
            SubJoinPlan subPlan;
            subPlan.activeDatasourceId_ = i;

            // loop each inactive window, analyze lookup plan & build necessary index 
            for (int j=0; j<datasourceList_->size(); j++) {
                if (i == j)
                    continue;
                JoinDataSource* lookupDatasource = (*datasourceList_)[j];
                
                vector<string> queryWindowList;
                queryWindowList.push_back(activeDatasource->getDataSourceName());
                for (int k=0; k<j; k++) {                    
                    if (k!= i) {                                            
                        queryWindowList.push_back((*datasourceList_)[k]->getDataSourceName());
                    }
                }

                Index* theIndex = NULL;
                if (! lookupDatasource->isStreamSource()) 
                    theIndex = builder.buildPlan(conditionExpr_, lookupDatasource->getDataSourceName(), queryWindowList);            
                IndexPlan oneLookupPlan;
                oneLookupPlan.datasourceId_ = j;
                // indexId will be -1 when there is no viable index generated (i.e. NULL)
                oneLookupPlan.indexId_ = lookupDatasource->addIndex(theIndex);
                subPlan.lookupPlans_.push_back(oneLookupPlan);
            }
            
            plan_.push_back(subPlan);
        }
    }
private:
    string strValue_;                           // operator name

    Expr* conditionExpr_;                       // join condition
    Expr* resultExpr_;                          // join result
    vector<JoinDataSource*>* datasourceList_;   // datasource list
    JoinPlan plan_;                             // the nested loop and index usage plan

    BaseExecutionContext context_;              // expr evaluation context object
    IntermediateEvent resultEvent_;             // temp object used for sending OUT events to downstream
    Event tempFlushEvent_;                      // temp object used for sending FLUSH events to downstream
    Event tempClearEvent_;                      // temp object used for sending CLEAR events to downstream
    Value nullValue_;                           // used by outer join as a temporary null object
};


/**
 *  JoinInputProxy operator
 *  each join datasource correspond to one JoinInputProxy operator
 *  it's responsible for establishing the connectivity from ConnectPoint operator toward a Join operator
 *  i.e. multiple JoinInputProxy operators form a fan-in pipe onto the Join operator
 */
class JoinInputProxyOperator : public BaseOperator {
public:
    JoinInputProxyOperator(int id, const char* connectPointName) :
       strValue_("JoinInput"),
       connectPointName_(connectPointName),
       inputId_(id),
       joinOperator_(NULL) {
    }

    virtual ~JoinInputProxyOperator() {
    }

    const string& getConnectPointName() {
        return connectPointName_;
    }

    int getInputId() {
        return inputId_;
    }

    JoinOperator* getJoinOperator(){
        return joinOperator_ ;
    }

    void setJoinOperator(JoinOperator* theOp){
        joinOperator_ = theOp;
    }

    // @ovverride
    virtual string& toString() {
        return strValue_ ;
    }

    // @override
    virtual BaseOperator* clone() {
        BaseOperator* clonedOperator = new JoinInputProxyOperator(inputId_, connectPointName_.c_str());
        clonedOperator->cloneProtectedAttibutes(this);
        return clonedOperator;
    }

    // @ovverride
    virtual void execute(long64 seq, Event* pevent) {
        // pass to the join operator
        joinOperator_->execute(inputId_, seq, pevent);
    }

private:
    string strValue_;              // operator name

    JoinOperator* joinOperator_;   // the associated join operator
    string connectPointName_;      // the name of ConnectPoint wire to this input proxy
    int inputId_;                  // id of this input (0 based)
};



#endif