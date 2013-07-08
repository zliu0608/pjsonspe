#ifndef _AGG_FUNCS_H
#define _AGG_FUNCS_H

#include "builtinfuncs.h"
#include "groupcontext.h"



/**
 * Aggregate count function
 * agg_count()
 */
class AggCountFunction : public BuiltinFunctionExpr {
public:
    AggCountFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~AggCountFunction(){
    }

    // @override
    virtual Variant& eval(BaseExecutionContext* pCtx) {
        GroupContext* grpCtx = pCtx->getGroupContext();
        long n = grpCtx->getEventNumInGroup();
        ret_.getValue().SetInt64(n);
        return ret_;
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new AggCountFunction(clonedArgs);
    }

    // @override
    virtual bool hasState() {
        return true;
    }
};

/**
 * Aggregate sum function
 * agg_sum()
 */
class AggSumFunction : public BuiltinFunctionExpr {
public:
    /**
     * inner context class
     */
    class AggSumContext : public FunctionContext {
    public:
        Value sumValue_;

        AggSumContext() : sumValue_((int)0) {
        }
        
        virtual ~AggSumContext() {
        }
    };
public:
    AggSumFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~AggSumFunction(){
    }

    // @override
    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();

        // access to group context
        GroupContext* grpCtx = pCtx->getGroupContext();
        AggSumContext* funcCtx = reinterpret_cast<AggSumContext*> (grpCtx->getFunctionContext(getStateKey())) ;
        if (!funcCtx) {
            funcCtx = new AggSumContext();
            grpCtx->addFunctionContext(getStateKey(), funcCtx);
        }
        
        ret_.setValue(funcCtx->sumValue_);
        Variant argVar = Variant(arg);
        Event* pevent = pCtx->getCurrentEvent();
        if (pevent->getType() == Event::TYPE_PLUS) {
            ret_.add(ret_, argVar);
        }
        else if (pevent->getType() == Event::TYPE_MINUS) {
            ret_.sub(ret_, argVar);
        }        
        return ret_;
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new AggSumFunction(clonedArgs);
    }

    // @override
    virtual bool hasState() {
        return true;
    }
};


#endif