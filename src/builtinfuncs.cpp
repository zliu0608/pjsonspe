#include "builtinfuncs.h"
#include "aggfuncs.h"


BuiltinFunctionExpr* createBuiltinFunction(const char* funcName, vector<Expr*>* args) {
    if (0 == strcmp("abs", funcName)) {
        return new AbsFunction(args);
    }
    else if (0 == strcmp("round", funcName)) {
        return new RoundFunction(args);
    }
    else if (0 == strcmp("ceil", funcName)) {
        return new CeilFunction(args);
    }
    else if (0 == strcmp("floor", funcName)) {
        return new FloorFunction(args);
    }
    else if (0 == strcmp("number", funcName)) {
        return new ToNumberFunction(args);
    }
    else if (0 == strcmp("is_null", funcName)) {
        return new IsNullFunction(args);
    }    
    else if (0 == strcmp("bit_test", funcName)) {
        return new BitTestFunction(args);
    }   
    else if (0 == strcmp("max", funcName)) {
        return new MaxFunction(args);
    }  
    else if (0 == strcmp("min", funcName)) {
        return new MinFunction(args);
    }  
    else if (0 == strcmp("agg_count", funcName)) {
        return new AggCountFunction(args);
    }    
    else if (0 == strcmp("agg_sum", funcName)) {
        return new AggSumFunction(args);
    }    
    else {
        return NULL;
    }
}

