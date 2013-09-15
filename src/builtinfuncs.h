#ifndef _BUILTIN_FUNCS_H
#define _BUILTIN_FUNCS_H

#include "expr.h"
#include <assert.h>
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace std;

/**
 * Builtin function base class
 * 
 * To support parallel execution,  each builtin function subclass must override the clone() method. <p>
 * a utility method cloneArgList() can be used to clone function arguments 
 */
class BuiltinFunctionExpr : public Expr {
protected:
    Variant ret_;
    vector<Expr*>* argumentList_;
    int stateContextKey_;             // the key to access state context of the function
public:
    BuiltinFunctionExpr(vector<Expr*>* args) : argumentList_(args), stateContextKey_(-1) {
    }

    virtual ~BuiltinFunctionExpr() {
        if (argumentList_) {
            for (vector<Expr*>::const_iterator iter = argumentList_->begin(); iter< argumentList_->end(); iter++) {
                delete (*iter);
            }
            argumentList_->clear();
        }
        delete argumentList_;
    }

    // @override
    // should never be called
    virtual Variant& eval(BaseExecutionContext* pCtx) = 0;
    
    // @override
    virtual bool isConstant() {
        return false;
    }

    // override
    // should never be called,  instead call its subclass
    virtual Expr* clone() = 0;

    // need to override if a function require state
    // therefore a state access key will be set for the function
    virtual bool hasState() {
        return false;
    }

    virtual void setStateKey(int key) {
        stateContextKey_ = key;
    }

    virtual int getStateKey() {
        return stateContextKey_;
    }

    // @override
    virtual void extractVariables(vector<string>& varList) {
        for (vector<Expr*>::const_iterator iter = argumentList_->begin(); iter< argumentList_->end(); iter++) {
            (*iter)->extractVariables(varList);
        }
    }
protected:

    /**
     * make a deep copy of the argument expr list
     *
     * @return the cloned argument expr list
     */
    vector<Expr*>* cloneArgList() {
        vector<Expr*>* clonedList = new vector<Expr*>();
        for (vector<Expr*>::const_iterator iter = argumentList_->begin(); iter< argumentList_->end(); iter++) {
            clonedList->push_back((*iter)->clone());
        }
        return clonedList;
    }
};


/**
 * abs function, returns absolute value of a number argument
 * the argument should be a scalar number
 */
class AbsFunction : public BuiltinFunctionExpr {
public:
    AbsFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~AbsFunction() {
    }

    // @override
    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();
        if (arg.IsInt())  {
            ret_.getValue().SetInt(std::abs(arg.GetInt()));
        }
        else if (arg.IsUint()) {
            long l = (long)arg.GetUint();
            ret_.getValue().SetInt64(std::abs(l));
        }
        else if (arg.IsInt64()){
            long l = (long) arg.GetInt64();
            ret_.getValue().SetInt64(std::abs(l));
        }
        else if ( arg.IsUint64()) {
            long l = (long) arg.GetUint64();
            ret_.getValue().SetInt64(std::abs(l));
        }
        else if (arg.IsDouble()) {
            ret_.getValue().SetDouble(std::abs(arg.GetDouble()));
        }
        return ret_;
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new AbsFunction(clonedArgs);
    }
};


/**
 * max function, returns a bigger value of the 2 arguments
 * the argument should be a scalar number
 */
class MaxFunction : public BuiltinFunctionExpr {
public:
    MaxFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~MaxFunction() {
    }

    // @override
    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Variant& v0 = (*argumentList_)[0]->eval(pCtx);
        Variant& v1 = (*argumentList_)[1]->eval(pCtx);
        Value& arg0 = v0.getValue();
        Value& arg1 = v1.getValue();

        v0.gt(vFlag_, v1);
        if (vFlag_.getBool()) {
            if (arg0.IsInt())  {
                ret_.getValue().SetInt(arg0.GetInt());
            }
            else if (arg0.IsUint()) {
                ret_.getValue().SetUint(arg0.GetUint());
            }
            else if (arg0.IsInt64()){
                ret_.getValue().SetInt64(arg0.GetInt64());
            }
            else if ( arg0.IsUint64()) {
                ret_.getValue().SetUint64(arg0.GetUint64());
            }
            else if (arg0.IsDouble()) {
                ret_.getValue().SetDouble(arg0.GetDouble());
            }            
        }
        else {
            if (arg1.IsInt())  {
                ret_.getValue().SetInt(arg1.GetInt());
            }
            else if (arg1.IsUint()) {
                ret_.getValue().SetUint(arg1.GetUint());
            }
            else if (arg1.IsInt64()){
                ret_.getValue().SetInt64(arg1.GetInt64());
            }
            else if ( arg1.IsUint64()) {
                ret_.getValue().SetUint64(arg1.GetUint64());
            }
            else if (arg1.IsDouble()) {
                ret_.getValue().SetDouble(arg1.GetDouble());
            }     
        }
        return ret_;
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new MaxFunction(clonedArgs);
    }
private:
    Variant vFlag_;
};


/**
 * min function, returns a smaller value of the 2 arguments
 * the argument should be a scalar number
 */
class MinFunction : public BuiltinFunctionExpr {
public:
    MinFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~MinFunction() {
    }

    // @override
    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Variant& v0 = (*argumentList_)[0]->eval(pCtx);
        Variant& v1 = (*argumentList_)[1]->eval(pCtx);
        Value& arg0 = v0.getValue();
        Value& arg1 = v1.getValue();

        v0.lt(vFlag_, v1);
        if (vFlag_.getBool()) {
            if (arg0.IsInt())  {
                ret_.getValue().SetInt(arg0.GetInt());
            }
            else if (arg0.IsUint()) {
                ret_.getValue().SetUint(arg0.GetUint());
            }
            else if (arg0.IsInt64()){
                ret_.getValue().SetInt64(arg0.GetInt64());
            }
            else if ( arg0.IsUint64()) {
                ret_.getValue().SetUint64(arg0.GetUint64());
            }
            else if (arg0.IsDouble()) {
                ret_.getValue().SetDouble(arg0.GetDouble());
            }            
        }
        else {
            if (arg1.IsInt())  {
                ret_.getValue().SetInt(arg1.GetInt());
            }
            else if (arg1.IsUint()) {
                ret_.getValue().SetUint(arg1.GetUint());
            }
            else if (arg1.IsInt64()){
                ret_.getValue().SetInt64(arg1.GetInt64());
            }
            else if ( arg1.IsUint64()) {
                ret_.getValue().SetUint64(arg1.GetUint64());
            }
            else if (arg1.IsDouble()) {
                ret_.getValue().SetDouble(arg1.GetDouble());
            }     
        }
        return ret_;
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new MinFunction(clonedArgs);
    }
private:
    Variant vFlag_;
};

/**
 * round function, round a number to the closest integer
 * the argument should be a scalar number
 */
class RoundFunction : public BuiltinFunctionExpr {
public:
    RoundFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~RoundFunction() {
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new RoundFunction(clonedArgs);
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();     
        if (arg.IsDouble()) {
            double d = arg.GetDouble();
            long l = (long) (d < 0.0 ? ceil(d - 0.5) : floor(d + 0.5));
            ret_.getValue().SetInt64(l);
        }
        else if (arg.IsInt())  {
            ret_.getValue().SetInt(arg.GetInt());
        }
        else if (arg.IsUint()) {
            long l = (long)arg.GetUint();
            ret_.getValue().SetInt64(l);
        }
        else if (arg.IsInt64()){
            long l = (long) arg.GetInt64();
            ret_.getValue().SetInt64(l);
        }
        else if ( arg.IsUint64()) {
            long l = (long) arg.GetUint64();
            ret_.getValue().SetInt64(l);
        }
        return ret_;
    }
};


/**
 * ceil function, returns the smallest integral value that is not less than x
 * the argument should be a scalar number
 */
class CeilFunction : public  BuiltinFunctionExpr {
public:
    CeilFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~CeilFunction() {
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new CeilFunction(clonedArgs);
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();     
        if (arg.IsDouble()) {
            long l = (long) ceil(arg.GetDouble());
            ret_.getValue().SetInt64(l);
        }
        else if (arg.IsInt())  {
            ret_.getValue().SetInt(arg.GetInt());
        }
        else if (arg.IsUint()) {
            long l = (long)arg.GetUint();
            ret_.getValue().SetInt64(l);
        }
        else if (arg.IsInt64()){
            long l = (long) arg.GetInt64();
            ret_.getValue().SetInt64(l);
        }
        else if ( arg.IsUint64()) {
            long l = (long) arg.GetUint64();
            ret_.getValue().SetInt64(l);
        }
        return ret_;
    }
};


/**
 * floor function, returns the largest integral value that is not greater than x
 * the argument should be a scalar number
 */
class FloorFunction : public BuiltinFunctionExpr {
public:
    FloorFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~FloorFunction() {
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new FloorFunction(clonedArgs);
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();     
        if (arg.IsDouble()) {
            long l = (long) floor(arg.GetDouble());
            ret_.getValue().SetInt64(l);
        }
        else if (arg.IsInt())  {
            ret_.getValue().SetInt(arg.GetInt());
        }
        else if (arg.IsUint()) {
            long l = (long)arg.GetUint();
            ret_.getValue().SetInt64(l);
        }
        else if (arg.IsInt64()){
            long l = (long) arg.GetInt64();
            ret_.getValue().SetInt64(l);
        }
        else if ( arg.IsUint64()) {
            long l = (long) arg.GetUint64();
            ret_.getValue().SetInt64(l);
        }
        return ret_;
    }
};

/**
 * number function, convert other types to a number
 * here are the conversion rules: 
 * (1) boolean to a number
 *     false -> int 0
 *     true  -> int 1
 * (2) string to a number
 *     str ->  long integer (if it can be converted to a integer)
 *     str ->  double  (if it can be converted to a double)
 *     leading and trailing spaces are allowed.
 * (3) number to number
 *     no conversion at all
 * (4) on conversion failure, return a Null value
 */
class ToNumberFunction : public BuiltinFunctionExpr {
public:
    ToNumberFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~ToNumberFunction() {
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new ToNumberFunction(clonedArgs);
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();     
        if (arg.IsBool()) {            
            ret_.getValue().SetInt(arg.GetBool() ? 1 : 0);
        }
        else if (arg.IsString()) {
            const char* str = arg.GetString();
            char* pstop;
            long l = strtol(str, &pstop, 10);
            // trim rest white space chars
            while (*pstop == ' ') {
                    pstop++;
            }
            if (*pstop == 0) {
                ret_.getValue().SetInt64(l);
                return ret_;
            }
            
            // otherwise try to to convert to a double
            double d = strtod(str, &pstop);
            // trim rest white space chars
            while (*pstop == ' ') {
                    pstop++;
            }
            if (*pstop == 0) {
                ret_.getValue().SetDouble(d);
                return ret_;
            }
            // TODO to add error handling code on conversion failure
            // return NULL for now
            ret_.getValue().SetNull();
            return ret_;
        }
        else if (arg.IsDouble()) {
            ret_.getValue().SetDouble(arg.GetDouble());
        }
        else if (arg.IsInt())  {
            ret_.getValue().SetInt(arg.GetInt());
        }
        else if (arg.IsUint()) {
            ret_.getValue().SetUint(arg.GetUint());
        }
        else if (arg.IsInt64()){
            ret_.getValue().SetInt64(arg.GetInt64());
        }
        else if ( arg.IsUint64()) {
            ret_.getValue().SetUint64(arg.GetUint64());
        }
        else {
            // TODO to add error handling code on conversion failure
            // return NULL for now
            ret_.getValue().SetNull();
            return ret_;
        }
        return ret_;
    }
};

/**
 * is_null function, returns true if argument is null, otherwise false
 */
class IsNullFunction : public BuiltinFunctionExpr {
public:
    IsNullFunction(vector<Expr*>* args) : BuiltinFunctionExpr(args) {
    }

    virtual ~IsNullFunction() {
    }

    // @override
    virtual Expr* clone() {
        vector<Expr*>* clonedArgs = cloneArgList();
        return new IsNullFunction(clonedArgs);
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        Value& arg = (*argumentList_)[0]->eval(pCtx).getValue();
        ret_.getValue().SetBool(arg.IsNull());
        return ret_;
    }
};

/**
 * factory pattern: to create a builtin function expr from the name
 */
BuiltinFunctionExpr* createBuiltinFunction(const char* funcName, vector<Expr*>* args);

#endif