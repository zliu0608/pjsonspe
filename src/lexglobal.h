#ifndef LEX_GLOBAL_H_
#define LEX_GLOBAL_H_

#define TOKEN_MAX_LEN  255

#ifndef YYSTYPE
typedef union {
    long lval;       // int number value
    double fval;     // double/float number value
    char   sval[TOKEN_MAX_LEN+1];     // string value
    struct symtab *symp;
} yystype;

#define YYSTYPE yystype
#define YYSTYPE_IS_TRIVIAL 1

#endif


/*
// customize a flexer
// lex input should use prefix option
#ifndef __FLEX_LEXER_H
#define yyFlexLexer MyFlexLexer
#include "FlexLexer.h"
#undef yyFlexLexer
#endif

#include <iostream>

#define YY_DECL int MyLexer::yylex()

class MyLexer : public MyFlexLexer {
public: 
    MyLexer(std::istream* arg_yyin = 0,
        std::ostream* arg_yyout = 0) {
        MyFlexLexer(arg_yyin, arg_yyout);
    }

    virtual YY_DECL;
public:
    YYSTYPE yylval;
};
*/
#endif