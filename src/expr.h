#ifndef EXPR_H
#define EXPR_H

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output

#include <limits>
#include <string>
#include <vector>
#include <string.h>
#include <assert.h>

#include "event.h"
#include "groupcontext.h"

using namespace rapidjson;
using namespace std;

#define E_OK                 0
#define E_INVALID_OPERAND    1    // operand type invalid


class Variant {
private:
    Value value_;
    Value* pv_;    // pointer of the value
    string s_;
public:
    Variant() : value_(), pv_(&value_) { // init value_ as a NULL value
    };

    Variant(bool b) : value_(b), pv_(&value_) {
    };

    Variant(int i) : value_(i), pv_(&value_) {
    };

    Variant(double d) : value_(d), pv_(&value_) {
    };

    // doesn't make a copy of the string
    Variant (const char* s) : value_(s), pv_(&value_) {
    };

    // initialize it as a reference to a value object
    Variant(Value& v) : value_(), pv_(NULL) {
        setValue(v);
    };

    virtual ~Variant() {
    }

    // access member of an object value
    Value& operator[](const char* name) {
        return (*pv_)[name];
    }

    // access element of an array object
    Value& operator[](int index) {
        return (*pv_)[index];
    }

    Value& getValue() {
        return *pv_;
    }

    void setValue(Value& v) {
        pv_ = &v;
    }

    bool getBool() {
        return pv_->GetBool();
    }

    // add operation
    int add(Variant& ret, Variant& other) {
        // number + number
        if (pv_->IsNumber() && other.pv_->IsNumber()) {
            if (pv_->IsInt() && other.pv_->IsInt()) {
                // int + int = int
                int v1 = pv_->GetInt();
                int v2 = other.pv_->GetInt();
                int v3 = v1+v2;
                ret.pv_->SetInt(v3);
            }
            else {
                // double + int = double
                // int + double = double
                double d1 = pv_->GetDouble();
                double d2 = other.pv_->GetDouble();
                double d3 = d1 + d2;
                ret.pv_->SetDouble(d3);
            }
            return E_OK ;
        }

        // string + string
        // note, the returned string is indeed held by s_
        if (pv_->IsString() && other.pv_->IsString()) {
            s_ = pv_->GetString();
            s_ += other.pv_->GetString();
            ret.pv_->SetString(s_.c_str());
            return E_OK;
        }

        return E_INVALID_OPERAND;
    }

    // sub operation
    int sub(Variant& ret, Variant& other) {
        // number - number
        if (pv_->IsNumber() && other.pv_->IsNumber()) {
            if (pv_->IsInt() && other.pv_->IsInt()) {
                // int - int = int
                int v1 = pv_->GetInt();
                int v2 = other.pv_->GetInt();
                int v3 = v1 - v2;
                ret.pv_->SetInt(v3);
            }
            else {
                // double - int = double
                // int - double = double
                double d1 = pv_->GetDouble();
                double d2 = other.pv_->GetDouble();
                double d3 = d1 - d2;
                ret.pv_->SetDouble(d3);
            }
            return E_OK ;
        }
        return E_INVALID_OPERAND;
    }

    int mul(Variant& ret, Variant& other) {
        if (pv_->IsNumber() && other.pv_->IsNumber()) {
            if (pv_->IsInt() && other.pv_->IsInt()) {
                // int * int = int
                int v1 = pv_->GetInt();
                int v2 = other.pv_->GetInt();
                int v3 = v1 * v2;
                ret.pv_->SetInt(v3);
            }
            else {
                // double * int = double
                // int * double = double
                double d1 = pv_->GetDouble();
                double d2 = other.pv_->GetDouble();
                double d3 = d1 * d2;
                ret.pv_->SetDouble(d3);
            }
            return E_OK ;
        }
        return E_INVALID_OPERAND;
    }

    int div(Variant& ret, Variant& other) {
        if (pv_->IsNumber() && other.pv_->IsNumber()) {
            // int/int = double
            // double / int = double
            // int / double = double
            double d1 = pv_->GetDouble();
            double d2 = other.pv_->GetDouble();
            double d3 = d1 / d2;
            ret.pv_->SetDouble(d3);            
            return E_OK ;
        }
        return E_INVALID_OPERAND;
    }


    int mod(Variant& ret, Variant& other) {
        if (pv_->IsInt() && other.pv_->IsInt()) {
            // int % int = %
            int v1 = pv_->GetInt();
            int v2 = other.pv_->GetInt();
            int v3 = v1 % v2;
            ret.pv_->SetInt(v3);            
            return E_OK ;
        }
        return E_INVALID_OPERAND;
    }

    int lt(Variant& ret, Variant& other) {
        bool b;

        if (pv_->IsInt() && other.pv_->IsInt()) {            
           // int < int
           b = pv_->GetInt() < other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsDouble() && other.pv_->IsDouble()) {            
           // double < double
           b = pv_->GetDouble() < other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsInt() && other.pv_->IsDouble()) {            
           // int < double
           b = pv_->GetInt() < other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        if (pv_->IsDouble() && other.pv_->IsInt()) {            
           // double < int
           b = pv_->GetDouble() < other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsString() && other.pv_->IsString()) {            
           // string < string
           b = strcmp(pv_->GetString() , other.pv_->GetString()) < 0;
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        return E_INVALID_OPERAND;        
    }

    int le(Variant& ret, Variant& other) {
        bool b;

        if (pv_->IsInt() && other.pv_->IsInt()) {            
           // int <= int
           b = pv_->GetInt() <= other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsDouble() && other.pv_->IsDouble()) {            
           // double <= double
           b = pv_->GetDouble() <= other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsInt() && other.pv_->IsDouble()) {            
           // int <= double
           b = pv_->GetInt() <= other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        if (pv_->IsDouble() && other.pv_->IsInt()) {            
           // double <= int
           b = pv_->GetDouble() <= other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsString() && other.pv_->IsString()) {            
           // string <= string
           b = strcmp(pv_->GetString() , other.pv_->GetString()) <= 0;
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        return E_INVALID_OPERAND;        
    }

    int gt(Variant& ret, Variant& other) {
        bool b;

        if (pv_->IsInt() && other.pv_->IsInt()) {            
           // int > int
           b = pv_->GetInt() > other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsDouble() && other.pv_->IsDouble()) {            
           // double > double
           b = pv_->GetDouble() > other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsInt() && other.pv_->IsDouble()) {            
           // int > double
           b = pv_->GetInt() > other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        if (pv_->IsDouble() && other.pv_->IsInt()) {            
           // double > int
           b = pv_->GetDouble() > other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsString() && other.pv_->IsString()) {            
           // string > string
           b = strcmp(pv_->GetString() , other.pv_->GetString()) > 0;
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        return E_INVALID_OPERAND;        
    }

    int ge(Variant& ret, Variant& other) {
        bool b;

        if (pv_->IsInt() && other.pv_->IsInt()) {            
           // int >= int
           b = pv_->GetInt() >= other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsDouble() && other.pv_->IsDouble()) {            
           // double >= double
           b = pv_->GetDouble() >= other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsInt() && other.pv_->IsDouble()) {            
           // int >= double
           b = pv_->GetInt() >= other.pv_->GetDouble();
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        if (pv_->IsDouble() && other.pv_->IsInt()) {            
           // double >= int
           b = pv_->GetDouble() >= other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsString() && other.pv_->IsString()) {            
           // string >= string
           b = strcmp(pv_->GetString() , other.pv_->GetString()) >= 0;
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        return E_INVALID_OPERAND;        
    }

    int eq(Variant& ret, Variant& other) {
        bool b;

        if (pv_->IsInt() && other.pv_->IsInt()) {            
           // int == int
           b = pv_->GetInt() == other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsDouble() && other.pv_->IsDouble()) {            
           // double == double
           b = pv_->GetDouble() - other.pv_->GetDouble() < numeric_limits<double>::epsilon() ;
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsInt() && other.pv_->IsDouble()) {            
           // int == double
           b = pv_->GetInt() - other.pv_->GetDouble() < numeric_limits<double>::epsilon();
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        if (pv_->IsDouble() && other.pv_->IsInt()) {            
           // double == int
           b = pv_->GetDouble() - other.pv_->GetInt() < numeric_limits<double>::epsilon();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsString() && other.pv_->IsString()) {            
           // string == string
           b = strcmp(pv_->GetString() , other.pv_->GetString()) == 0;
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        return E_INVALID_OPERAND;        
    }

    int ne(Variant& ret, Variant& other) {
        bool b;

        if (pv_->IsInt() && other.pv_->IsInt()) {            
           // int != int
           b = pv_->GetInt() != other.pv_->GetInt();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsDouble() && other.pv_->IsDouble()) {            
           // double != double
           b = pv_->GetDouble() - other.pv_->GetDouble() > numeric_limits<double>::epsilon() ;
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsInt() && other.pv_->IsDouble()) {            
           // int != double
           b = pv_->GetInt() - other.pv_->GetDouble() > numeric_limits<double>::epsilon();
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        if (pv_->IsDouble() && other.pv_->IsInt()) {            
           // double != int
           b = pv_->GetDouble() - other.pv_->GetInt() > numeric_limits<double>::epsilon();
           ret.pv_->SetBool(b);
           return E_OK ;
        }
        if (pv_->IsString() && other.pv_->IsString()) {            
           // string != string
           b = strcmp(pv_->GetString() , other.pv_->GetString()) != 0;
           ret.pv_->SetBool(b);
           return E_OK ;
        }        
        return E_INVALID_OPERAND;        
    }

    int and(Variant& ret, Variant& other) {
        if (pv_->IsBool() && other.pv_->IsBool()) {
            ret.pv_->SetBool(pv_->GetBool() && other.pv_->GetBool());
            return E_OK;
        }
        return E_INVALID_OPERAND; 
    }

    int or(Variant& ret, Variant& other) {
        if (pv_->IsBool() && other.pv_->IsBool()) {
            ret.pv_->SetBool(pv_->GetBool() || other.pv_->GetBool());
            return E_OK;
        }
        return E_INVALID_OPERAND; 
    }

    int in(Variant& ret, Variant& other) {
        // other must be an array
        if (!other.pv_->IsArray()) {
            return E_INVALID_OPERAND; 
        }

        bool b =  false;
        // test if a int exists in a array
        if (pv_->IsInt()) {
            int v = pv_->GetInt();
            for (unsigned long l=0; l<other.pv_->Size(); l++) {
                if ((*other.pv_)[l].IsInt() && v == (*other.pv_)[l].GetInt()) {
                    b = true;
                    break;
                }
            }
            ret.pv_->SetBool(b);
            return E_OK;
        }

        // test if a double exists in a array
        if (pv_->IsDouble()) {
            double v = pv_->GetDouble();
            for (unsigned long l=0; l<other.pv_->Size(); l++) {
                if ((*other.pv_)[l].IsDouble() && 
                    (v - (*other.pv_)[l].GetDouble() < numeric_limits<double>::epsilon())
                    ) {
                    b = true;
                    break;
                }
            }
            ret.pv_->SetBool(b);
            return E_OK;
        }

        // test if a bool exists in a array
        if (pv_->IsBool()) {
            bool v = pv_->GetBool();
            for (unsigned long l=0; l<other.pv_->Size(); l++) {
                if ((*other.pv_)[l].IsBool() && 
                    v == (*other.pv_)[l].GetBool()
                    ) {
                    b = true;
                    break;
                }
            }
            ret.pv_->SetBool(b);
            return E_OK;
        }

        // test if a string exists in a array
        if (pv_->IsString()) {
            const char* v = pv_->GetString();
            for (unsigned long l=0; l<other.pv_->Size(); l++) {
                if ((*other.pv_)[l].IsString() && 
                    0 == strcmp(v, (*other.pv_)[l].GetString())
                    ) {
                    b = true;
                    break;
                }
            }
            ret.pv_->SetBool(b);
            return E_OK;
        }

        // test if NULL exists in a array
        if (pv_->IsNull()) {
            for (unsigned long l=0; l<other.pv_->Size(); l++) {
                if ((*other.pv_)[l].IsNull()) {
                    b = true;
                    break;
                }
            }
            ret.pv_->SetBool(b);
            return E_OK;
        }

        return E_INVALID_OPERAND; 
    }

    int not(Variant& ret) {
        if (! pv_->IsBool()) {
            return E_INVALID_OPERAND;
        }

        ret.pv_->SetBool(! pv_->GetBool());
        return E_OK;
    }
};


class BaseExecutionContext {
private:
    Event* pCurrentEvent_;
    GroupContext* pGroupContext_;
public:
    BaseExecutionContext() : pCurrentEvent_ (NULL) {
    };

    void reset() {
        pCurrentEvent_ = NULL;
        pGroupContext_ = NULL;
    }

    Event* getCurrentEvent() {
        return pCurrentEvent_;
    }

    void setCurrentEvent(Event* newEvent) {
        pCurrentEvent_ = newEvent;
    }

    GroupContext* getGroupContext() {
        return pGroupContext_;
    }

    void setGroupContext(GroupContext* grpContext) {
        pGroupContext_ = grpContext;
    }
};


class Expr {
public:
    enum OpKind {
        // binary operators
        OP_ADD, 
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_MOD,
        OP_LT,
        OP_GT,
        OP_LE,
        OP_GE,
        OP_EQ,
        OP_NE,
        OP_OR,
        OP_AND,
        OP_IN,
        //unary operators
        OP_NOT
    };

    virtual Variant& eval(BaseExecutionContext* pCtx) = 0;

    virtual bool isConstant() {
        return false;    
    };

    virtual Expr* clone() = 0;
};


#define BINARY_OP(OP) do { \
  Variant& v1 = left_->eval(pCtx); \
  Variant& v2 = right_->eval(pCtx); \
  v1.OP(ret_, v2); \
} while (0)

#define UNARY_OP(OP) do { \
  Variant& v1 = operand_->eval(pCtx); \
  v1.OP(ret_); \
} while (0)

class BinaryExpr : public Expr {
protected:
    Expr* left_;
    Expr* right_;
    Expr::OpKind op_;
    Variant ret_;
public:
    BinaryExpr (Expr::OpKind op, Expr* left, Expr* right) :
        op_(op), left_(left), right_(right) {
    }

    virtual ~BinaryExpr() {
        if (left_ != NULL) 
            delete left_;
        if (right_ != NULL)
            delete right_;
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        switch (op_) {
            case Expr::OP_ADD : 
                BINARY_OP(add);
                break;
            case Expr::OP_SUB :
                BINARY_OP(sub);
                break;
            case Expr::OP_MUL :
                BINARY_OP(mul);
                break;
            case Expr::OP_DIV :
                BINARY_OP(div);
                break;
            case Expr::OP_MOD :
                BINARY_OP(mod);
                break;
            case Expr::OP_LT :
                BINARY_OP(lt);
                break;
            case Expr::OP_GT :
                BINARY_OP(gt);
                break;
            case Expr::OP_LE :
                BINARY_OP(le);
                break;
            case Expr::OP_GE :
                BINARY_OP(ge);
                break;
            case Expr::OP_EQ :
                BINARY_OP(eq);
                break;
            case Expr::OP_NE :
                BINARY_OP(ne);
                break;
            case Expr::OP_AND :
                BINARY_OP(and);
                break;
            case Expr::OP_OR :
                BINARY_OP(or);
                break;
            case Expr::OP_IN :
                BINARY_OP(in);
                break;
            default:
                break;
        }
        return ret_;        
    }

    // @override
    virtual Expr* clone() {
        Expr* clonedLeft = NULL;
        Expr* clonedRight = NULL;

        if (left_) 
            clonedLeft = left_->clone();
        if (right_)
            clonedRight = right_->clone();
        return new BinaryExpr(op_, clonedLeft, clonedRight);
    }

    // @override
    virtual bool isConstant() {
        return left_->isConstant() && right_->isConstant();
    };
};


class UnaryExpr : public Expr {
protected:
    Expr* operand_;
    Expr::OpKind op_;
    Variant ret_;
public:
    UnaryExpr (Expr::OpKind op, Expr* operand): 
        op_(op), operand_(operand) {
    };

    virtual ~UnaryExpr() {
        if (operand_ != NULL) 
            delete operand_;
    };    

   virtual Variant& eval(BaseExecutionContext* pCtx) {
        switch (op_) {
            case Expr::OP_NOT :
                UNARY_OP(not);
                break;
            default:
                break;
        }
        return ret_;
   };

    // @override
    virtual Expr* clone() {
        Expr* clonedOperand = NULL;        

        if (operand_) 
            clonedOperand = operand_->clone();
        return new UnaryExpr(op_, clonedOperand);
    }

    // @override
    virtual bool isConstant() {
        return operand_->isConstant();
    };
};

class StringValue : public Expr {
private:
    string s_;   //  to hold the memory for string content
    Variant ret_;
public:
   StringValue(const char* s) : s_(s), ret_(s_.c_str()) {
   }

   virtual Variant& eval(BaseExecutionContext* pCtx) {
       return ret_;
   }

   //@override
   virtual bool isConstant() {
        return true; 
   }

    // @override
    virtual Expr* clone() {
        return new StringValue(s_.c_str());
    }
};

class IntValue : public Expr {
private:
    Variant ret_;
public:
   IntValue(int i) :  ret_(i) {
   }

   virtual Variant& eval(BaseExecutionContext* pCtx) {
       return ret_;
   }

   //@override
   virtual bool isConstant() {
        return true; 
   }

    // @override
   virtual Expr* clone() {
        return new IntValue(ret_.getValue().GetInt());
    }
};

class DoubleValue : public Expr {
private:
    Variant ret_;
public:
   DoubleValue(double d) :  ret_(d) {
   }

   virtual Variant& eval(BaseExecutionContext* pCtx) {
       return ret_;
   }

   //@override
   virtual bool isConstant() {
        return true; 
   }

    // @override
   virtual Expr* clone() {
        return new DoubleValue(ret_.getValue().GetDouble());
    }
};

class BoolValue : public Expr {
private:
    Variant ret_;
public:
   BoolValue(bool b) :  ret_(b) {
   }

   virtual Variant& eval(BaseExecutionContext* pCtx) {
       return ret_;
   }

   //@override
   virtual bool isConstant() {
        return true; 
   }

    // @override
   virtual Expr* clone() {
        return new BoolValue(ret_.getValue().GetBool());
    }
};

class NullValue : public Expr {
private:
    Variant ret_;
public:
   NullValue()  {
   }

   virtual Variant& eval(BaseExecutionContext* pCtx) {
       return ret_;
   }

   //@override
   virtual bool isConstant() {
        return true; 
   }

    // @override
   virtual Expr* clone() {
        return new NullValue();
    }
};

/**
 * Expression for constructing an array. 
 * it can be a constant array,  or an array of variables
 */
class ArrayConstructExpr : public Expr {
private:
    Variant ret_;
    bool isConstant_;
    Value arrayValue_;
    vector<Expr*> * elementList_;

    // this document object provides allocator for evlating the array expr
    // it should not be destructed until end of life of the expr.
    Document* pDoc_;        

public:
    ArrayConstructExpr() : isConstant_(true), elementList_(NULL) , pDoc_(new Document()){
        arrayValue_.SetArray();
        ret_.setValue(arrayValue_);
    }

    ArrayConstructExpr(vector<Expr*> * elementList) : isConstant_(true), elementList_(elementList), arrayValue_(kArrayType), pDoc_(new Document()) {
        // arrayValue_.SetArray();
        ret_.setValue(arrayValue_);

        if ((NULL == elementList) || elementList->empty() ) {
            isConstant_ = true;
            return;
        }
        for (vector<Expr*>::const_iterator iter = elementList->begin(); iter < elementList->end(); iter++) {
            isConstant_ &= (*iter)->isConstant();
        }
                
        if (isConstant_) {
            constructArrayFromElementExprs(NULL);            
        }
    }

    virtual ~ArrayConstructExpr() {
        if (elementList_) {
            for (vector<Expr*>::const_iterator iter = elementList_->begin(); iter< elementList_->end(); iter++) {
                delete (*iter);
            }
            elementList_->clear();
        }
        delete elementList_;

        if (pDoc_)
            delete pDoc_;
    }

   //@override
   virtual Variant& eval(BaseExecutionContext* pCtx) {
       if (isConstant_)
            return ret_;
       else {
           constructArrayFromElementExprs(pCtx);
           return ret_;
       }
   }

   //@override
   virtual bool isConstant() {
        return isConstant_; 
   }

    // @override
   virtual Expr* clone() {
        if (NULL == elementList_) {
            return new ArrayConstructExpr();
        }

#ifdef _DEBUG
        printf("about to clone array, original value is :  \n");
        printContent();
#endif

        vector<Expr*>* clonedList = new vector<Expr*>();
        for (vector<Expr*>::const_iterator iter = elementList_->begin(); iter< elementList_->end(); iter++) {
            clonedList->push_back((*iter)->clone());
        }
        ArrayConstructExpr* pNewArray = new ArrayConstructExpr(clonedList);
#ifdef _DEBUG
        printf("cloned value is :  \n");
        pNewArray->printContent();
#endif
        return pNewArray;
    }

private:
    /**
     * evaluate each element expr, copy its resulting value into the array object
     *
     * @param pCtx evaluation context. 
     *   A constant array is evaluated at operator compilation time, NULL can be used for pCtx,
     *   for array variable, this parameter should use the corresponding event evalaution context
     */
    void constructArrayFromElementExprs(BaseExecutionContext* pCtx) {
        Document* tempDoc = pDoc_;
        for (vector<Expr*>::const_iterator iter = elementList_->begin(); iter < elementList_->end(); iter++) {
            //tempDoc->~Document();
            //new(tempDoc)Document();

            Value& v = (*iter)->eval(pCtx).getValue();    // get its constant value and then copy into the array object
            Value tempValue;
            switch(v.GetType()) {
            case kNullType: 
                arrayValue_.PushBack(tempValue, tempDoc->GetAllocator());  
                break;
            case kFalseType:
            case kTrueType:
                // tempValue.SetBool(v.GetBool());
                arrayValue_.PushBack(v.GetBool(), tempDoc->GetAllocator()); 
                break;
            case kObjectType:
            case kArrayType:
                arrayValue_.PushBack(v.CopyTo(*tempDoc), tempDoc->GetAllocator()); 
                break;
            case kStringType:
                // tempValue.SetString(v.GetString());
                arrayValue_.PushBack(v.GetString(), tempDoc->GetAllocator()); 
                break;
            case kNumberType:
                if (v.IsInt()) {
                    //tempValue.SetInt(v.GetInt());
                    arrayValue_.PushBack(v.GetInt(), tempDoc->GetAllocator()); 
                }
                else if (v.IsUint()) {
                    // tempValue.SetUint(v.GetUint());
                    arrayValue_.PushBack(v.GetUint(), tempDoc->GetAllocator()); 
                }
                else if (v.IsInt64())	{
                    // tempValue.SetInt64(v.GetInt64());
                    arrayValue_.PushBack(v.GetInt64(), tempDoc->GetAllocator()); 
                }
                else if (v.IsUint64()){
                    // tempValue.SetUint64(v.GetUint64());
                    arrayValue_.PushBack(v.GetUint64(), tempDoc->GetAllocator()); 
                }
                else {
                    // tempValue.SetDouble(v.GetDouble());
                    arrayValue_.PushBack(v.GetDouble(), tempDoc->GetAllocator()); 
                }
                break;
            } // end switch
#ifdef _DEBUG
        // printf("constructing value:  \n");
        // printContent();
#endif
        }  // end for 
    }

    void printContent() {
        if (!isConstant_) {
            return;
        }
        
        int num = (int) arrayValue_.Size();
        int ivv;
        for (int i=0; i<num; i++) {
            Value& v = arrayValue_[SizeType(i)];
            if (v.IsInt()) {
                ivv = v.GetInt();
                printf("** v[%d] = %d \n", i, ivv);
            }
            else if (v.IsUint()) {
                ivv = v.GetUint();
                printf("** v[%d] = %d \n", i, ivv);
            }
        }
    }
};


// path expressions
class PathExpr : public Expr {
private:
    Variant nullRet_;
public:
    virtual void addChildPathExpr(PathExpr* p) {
        assert(false);
    }

    virtual int getChildExprCount() {
        assert(false);
        return 0;
    }

    virtual PathExpr* childExprAt(int index) {
        assert(false);
        return NULL;
    }

    virtual Value& evalPath(BaseExecutionContext* pCtx, Value& v) {
        assert(false);
        return nullRet_.getValue();
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        assert(false);
        return nullRet_;
    }
};

class VarRefExpr : public PathExpr {
private:
    string varName_;   // $ is a special name
    vector<PathExpr*>  children_;
    Variant ret_;
    Variant var_;
public:
    VarRefExpr(const char* pszVarName) : varName_(pszVarName) {       
    }

    virtual ~VarRefExpr() {
        // delete all child exprs
        size_t n = children_.size();
        for (size_t i=0; i<n; i++) {
            delete (children_[i]);
        }
    }

    virtual void addChildPathExpr(PathExpr* p) {
        children_.push_back(p);
    }

    virtual int getChildExprCount() {
        return (int) children_.size();
    }

    virtual PathExpr* childExprAt(int index) {
        return children_[index];
    }

    virtual Value& evalPath(BaseExecutionContext* pCtx, Value& v) {
        Value* pv = &v;
        size_t n = children_.size();
        for (size_t i=0; i<n; i++) {
            pv = &(children_[i]->evalPath(pCtx, *pv));
        }
        return *pv;
    }

    virtual Variant& eval(BaseExecutionContext* pCtx) {
        //evaluate the current variable
        if (varName_.compare("$") == 0) {
            Value& v = *pCtx->getCurrentEvent()->getPayload();
            var_.setValue(v);
        }
        else {
            // TODO if reference to other names
            printf("varName_=%s\n", varName_.c_str());
        }
        
        // evaluate path down to leaf
        Value& v = evalPath(pCtx, var_.getValue());
        ret_.setValue(v);
        return ret_;
    }

   // @override
   virtual Expr* clone() {
        VarRefExpr* clonedExpr = new VarRefExpr(varName_.c_str());
        // clone the children 
        for (vector<PathExpr*>::const_iterator iter = children_.begin(); iter< children_.end(); iter++) {
            PathExpr* clonedChildPathExpr = dynamic_cast<PathExpr*>((*iter)->clone());
            clonedExpr->addChildPathExpr(clonedChildPathExpr);
        }
        return clonedExpr;
   }
};


class StepExpr : public PathExpr {
private:    
    string projectName_;
    Value arrayValue_;   // for returning a array projection result
    Value nullValue_;    // for returning a null value
    Document* pDoc_;
public:
    StepExpr(const char* pszName) : projectName_(pszName), pDoc_(new Document()) {
    }

    virtual ~StepExpr(){
        if (pDoc_)
            delete pDoc_;
    }

    virtual Value& evalPath(BaseExecutionContext* pCtx, Value& v) {
        if (v.IsObject()) {
            return v[projectName_.c_str()];
        }
        if (v.IsArray()) {
            Value& vv = *pDoc_;
            arrayValue_.SetArray();
            SizeType num = v.Size();
            for (SizeType i=0; i<num; i++){
                if (!v[i].IsNull() && v[i].IsObject()){
                    Value& tempResult = (v[i])[projectName_.c_str()];
                    tempResult.CopyTo(*pDoc_);
                }
                else {
                    nullValue_.CopyTo(*pDoc_);
                }
                arrayValue_.PushBack(vv, pDoc_->GetAllocator()); 
            }
            return arrayValue_;
        }
        // other cases return null
        return nullValue_;
    }

   // @override
   virtual Expr* clone() {
        return new StepExpr(projectName_.c_str());
   }
};

class ProjectExpr : public PathExpr {
private:
    vector<string>* projectNames_;
    bool exclusive_;
    Document doc_;      
    Value arrayValue_;  // for returning a array projection result
    Value nullValue_;   // for returning a null value
    Document tempDoc_;
public:
    ProjectExpr(vector<string>* prjList, bool exclusive) : projectNames_(prjList), exclusive_(exclusive) {
    }

    virtual ~ProjectExpr(){
        if(projectNames_) 
            delete projectNames_;
    }

    
    virtual Value& evalPath(BaseExecutionContext* pCtx, Value& v) {
        if (v.IsObject()) {
            return evalPathAgainstRecord(v);
        }
        if (v.IsArray()) {
            return evalPathAgainstArray(v);
        }

        // other cases return null
        return nullValue_;
    }

   // @override
   virtual Expr* clone() {
        if (NULL == projectNames_)
            return new ProjectExpr(NULL, exclusive_);

        vector<string>* clonedList = new vector<string>();
        for (vector<string>::const_iterator iter = projectNames_->begin(); iter< projectNames_->end(); iter++) {
            clonedList->push_back(*iter);
        }
        return new ProjectExpr(clonedList, exclusive_);
   }

private:
    Value& evalPathAgainstRecord(Value& v) {
        if (exclusive_) {
            v.CopyTo(doc_);
            // remove the unwanted members
            int n = (int) projectNames_->size();
            for (int i=0; i<n; i++) {
                doc_.RemoveMember((*projectNames_)[i].c_str());
            }
            return doc_;
        }
        else {            
            doc_.SetObject();
            // add wanted members
            int n = (int) projectNames_->size();            
            for (int i=0; i<n; i++) {
                Value& vv = v[(*projectNames_)[i].c_str()];
                vv.CopyTo(tempDoc_);
                
                // to avoid call implicit conversion which would result in compilation error, alternative way is to use static_cast
                Value& copiedValue = tempDoc_;  
                doc_.AddMember((*projectNames_)[i].c_str(), copiedValue, doc_.GetAllocator());
            }            
            return doc_;
        }
    }

    Value& evalPathAgainstArray(Value& v) {
        arrayValue_.SetArray(); 
        SizeType num = v.Size();
        int n = (int) projectNames_->size();

        if (exclusive_) {            
            // loop each element of v, exclude those unwanted members
            for (SizeType idx=0; idx<num; idx++){
                v[idx].CopyTo(doc_);
                for (int i=0; i<n; i++) {
                    doc_.RemoveMember((*projectNames_)[i].c_str());
                }
                // to avoid call implicit conversion which would result in compilation error, alternative way is to use static_cast
                Value& copiedValue = doc_;
                arrayValue_.PushBack(copiedValue, doc_.GetAllocator());
            }
            return arrayValue_;
        }
        else {
            // loop each element of v, add those wanted members
            for (SizeType idx=0; idx<num; idx++){
                doc_.SetObject();
                Value& theElement = v[idx]; 
                for (int i=0; i<n; i++) {
                    Value& vv = theElement[(*projectNames_)[i].c_str()];
                    vv.CopyTo(tempDoc_);
                    
                    // to avoid call implicit conversion which would result in compilation error, alternative way is to use static_cast
                    Value& copiedValue = tempDoc_;  
                    doc_.AddMember((*projectNames_)[i].c_str(), copiedValue, doc_.GetAllocator());
                }
                Value& copiedValue2 = doc_;
                arrayValue_.PushBack(copiedValue2, doc_.GetAllocator());
            }
            return arrayValue_;
        }
    }
};

/**
 * Expression to access one or more elements of an array
 * if only beginIndex is provided (endIndex expr=null), then only access to one element
 * otherwise, access subarray from beginIndex all the way to endIndex
 * note: endIndex must >= beginIndex
 *
 */
class ArrayElementsExpr : public PathExpr {
private:
    Expr* beginIndex_;
    Expr* endIndex_;
    bool returnArray_;
    Value arrayValue_;   // for returning an array
    Document tempDoc_;
public:
    ArrayElementsExpr(Expr* beginIndex, Expr* endIndex) : 
       beginIndex_(beginIndex), 
       endIndex_(endIndex), 
       returnArray_( NULL != endIndex ) {
    }

    virtual ~ArrayElementsExpr(){
        if (beginIndex_)
            delete beginIndex_;
        if (endIndex_) 
            delete endIndex_;
    }

    virtual Value& evalPath(BaseExecutionContext* pCtx, Value& v) {
        int idx1 = -1;
        int idx2 = -1;
        Value& v1 = beginIndex_->eval(pCtx).getValue();
        if (v1.IsInt()) {
            idx1 = v1.GetInt();
        }
        else if (v1.IsUint()) {
            idx1 = v1.GetUint();
        }
        else if (v1.IsInt64()) {
            idx1 = (int) v1.GetInt64();
        }
        else if (v1.IsUint64()) {
            idx1 = (int) v1.GetUint64();
        }        
        if (!returnArray_) {
            // only return one element
            return v[idx1];
        }

        // otherwise to construct an array
        arrayValue_.SetArray();
        Value& v2 = endIndex_->eval(pCtx).getValue();
        if (v2.IsInt()) {
            idx2 = v2.GetInt();
        }
        else if (v2.IsUint()) {
            idx2 = v2.GetUint();
        }
        else if (v2.IsInt64()) {
            idx2 = (int) v2.GetInt64();
        }
        else if (v2.IsUint64()) {
            idx2 = (int) v2.GetUint64();
        }

        Value& clonedValue = tempDoc_;
        for (int i=idx1; i<=idx2; i++) {
            Value& vElement = v[i];
            switch(vElement.GetType()) {
            case kNullType:
            case kObjectType:
            case kArrayType:
                vElement.CopyTo(tempDoc_);                
                arrayValue_.PushBack(clonedValue, tempDoc_.GetAllocator()); 
                break;
            case kFalseType:
            case kTrueType:
                arrayValue_.PushBack(vElement.GetBool(), tempDoc_.GetAllocator());
                break;
            case kStringType:                
                arrayValue_.PushBack(vElement.GetString(), tempDoc_.GetAllocator());
                break;
            case kNumberType:
                if (vElement.IsInt()) {
                    arrayValue_.PushBack(vElement.GetInt(), tempDoc_.GetAllocator()); 
                }
                else if (vElement.IsUint()) {
                    arrayValue_.PushBack(vElement.GetUint(), tempDoc_.GetAllocator()); 
                }
                else if (vElement.IsInt64()) {
                    arrayValue_.PushBack(vElement.GetInt64(), tempDoc_.GetAllocator()); 
                }
                else if (vElement.IsUint64()) {
                    arrayValue_.PushBack(vElement.GetUint64(), tempDoc_.GetAllocator()); 
                }
                else {
                    arrayValue_.PushBack(vElement.GetDouble(), tempDoc_.GetAllocator()); 
                }
                break;
            }// end switch
        } // end loop
        return arrayValue_;
    }

   // @override
   virtual Expr* clone() {
       Expr* clonedBegin = beginIndex_->clone();
       Expr* clonedEnd = (endIndex_== NULL) ? NULL : endIndex_->clone();
       return new ArrayElementsExpr(clonedBegin, clonedEnd);
   }
};

class RecordConstructExpr;

class RecordMemberExpr : public Expr {
    friend class RecordConstructExpr;
private:
    string mName_;         // member name
    Expr* mValue_;         // member value expr
public:
    RecordMemberExpr(const char* pszName, Expr* valueExpr) : mName_(pszName), mValue_(valueExpr) {
    }

    virtual ~RecordMemberExpr() {
        if (mValue_) 
            delete mValue_;
    }

    //@override
    virtual Variant& eval(BaseExecutionContext* pCtx) {
        return mValue_->eval(pCtx);
    }

    //@override
    virtual bool isConstant() {
        return mValue_->isConstant(); 
    }
    
    //@override
    virtual Expr* clone() {
        Expr* clonedValueExpr = mValue_->clone();
        return new RecordMemberExpr(mName_.c_str(), clonedValueExpr);
    }
};

/**
 * Expression for constructing a record
 * it can be a constant record or a record with variable expressions
 */
class RecordConstructExpr : public Expr {
private:
    vector<RecordMemberExpr*>* memberList_;
    Variant ret_;
    bool isConstant_;
    Value recordValue_;

    // this document object provides allocator for evlating the array expr
    // it should not be destructed until end of life of the expr.
    Document* pDoc_;        
public:
    RecordConstructExpr(vector<RecordMemberExpr*>* memberList) : memberList_(memberList), isConstant_(true), recordValue_(kObjectType), pDoc_(new Document()){
        ret_.setValue(recordValue_);

        if ((NULL == memberList) || memberList->empty()){
            isConstant_ = true;
            return;
        }

        for (vector<RecordMemberExpr*>::const_iterator iter = memberList->begin(); iter < memberList->end(); iter++) {
            isConstant_ &= (*iter)->isConstant();
        }
        if (isConstant_) {
            constructRecordFromMemberExprs(NULL);            
        }
    }

    virtual ~RecordConstructExpr() {
        if (memberList_) {
            for (vector<RecordMemberExpr*>::const_iterator iter = memberList_->begin(); iter< memberList_->end(); iter++) {
                delete (*iter);
            }
            memberList_->clear();
        }
        delete memberList_;

        if (pDoc_)
            delete pDoc_;        
    }

   //@override
   virtual Variant& eval(BaseExecutionContext* pCtx) {
       if (isConstant_)
            return ret_;
       else {
           constructRecordFromMemberExprs(pCtx);
           return ret_;
       }
   }

   //@override
   virtual bool isConstant() {
        return isConstant_; 
   }

    // @override
   virtual Expr* clone() {
        if (!memberList_)
            return NULL;

        vector<RecordMemberExpr*>* clonedList = new vector<RecordMemberExpr*>();
        for (vector<RecordMemberExpr*>::const_iterator iter = memberList_->begin(); iter< memberList_->end(); iter++) {
            RecordMemberExpr* pMemberExpr = reinterpret_cast<RecordMemberExpr*>((*iter)->clone());
            clonedList->push_back(pMemberExpr);
        }
        RecordConstructExpr* clonedRecordExpr = new RecordConstructExpr(clonedList);
        return clonedRecordExpr;
   }

private:
    void constructRecordFromMemberExprs(BaseExecutionContext* pCtx) {
        recordValue_.SetObject();                       // set the result to an empty object
        Document* tempDoc = pDoc_;
        Value& vv = *tempDoc;        
        // call destructor to free its allocactor
        tempDoc->~Document();
        new (tempDoc) Document();

        for (vector<RecordMemberExpr*>::const_iterator iter = memberList_->begin(); iter < memberList_->end(); iter++) {
            Value& v = (*iter)->eval(pCtx).getValue();  // get its constant value and then copy into the record object
            Value tempValue;

            switch(v.GetType()) {
            case kNullType:
                recordValue_.AddMember((*iter)->mName_.c_str(), tempValue, tempDoc->GetAllocator());
                break;
            case kFalseType:
            case kTrueType:
                recordValue_.AddMember((*iter)->mName_.c_str(), v.GetBool(), tempDoc->GetAllocator());
                break;
            case kObjectType:
                if ((*iter)->mName_.compare("*") != 0) {
                    v.CopyTo(*tempDoc);
                    // Value& vv = *tempDoc;
                    recordValue_.AddMember((*iter)->mName_.c_str(), vv, tempDoc->GetAllocator());
                }
                else {
                    // for the case "path_expr DOT_STAR", to extract all members from path_expr, put them in this resulting value
                    Value::MemberIterator mIter = v.MemberBegin();
                    Value::MemberIterator endIter = v.MemberEnd();
                    while (mIter != endIter) {
                        mIter->value.CopyTo(*tempDoc);
                        const char* pszName = mIter->name.GetString();
                        // Value& vv = *tempDoc;
                        recordValue_.AddMember(pszName, vv, tempDoc->GetAllocator());
                        mIter++;
                    }
                }
                break;
            case kArrayType:
                v.CopyTo(*tempDoc);
                // Value& vv = *tempDoc;
                recordValue_.AddMember((*iter)->mName_.c_str(), vv, tempDoc->GetAllocator());
                break;
            case kStringType:
                recordValue_.AddMember((*iter)->mName_.c_str(), v.GetString(), tempDoc->GetAllocator());
                break;
            case kNumberType:
                if (v.IsInt()) {
                    recordValue_.AddMember((*iter)->mName_.c_str(), v.GetInt(), tempDoc->GetAllocator());
                }
                else if (v.IsUint()) {
                    recordValue_.AddMember((*iter)->mName_.c_str(), v.GetUint(), tempDoc->GetAllocator());
                }
                else if (v.IsInt64()) {
                    recordValue_.AddMember((*iter)->mName_.c_str(), v.GetInt64(), tempDoc->GetAllocator());
                }
                else if (v.IsUint64()){
                    recordValue_.AddMember((*iter)->mName_.c_str(), v.GetUint64(), tempDoc->GetAllocator());
                }
                else {
                    recordValue_.AddMember((*iter)->mName_.c_str(), v.GetDouble(), tempDoc->GetAllocator());
                }
                break;
            } // end switch
        } // end for loop
    }
};
#endif