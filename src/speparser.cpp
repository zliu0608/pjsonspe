/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 1 "speparser.y"

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include "tokendef.h"
#include "window.h"
#include "group.h"
#include "reduce.h"
#include "speparser.h"

using namespace std;

	void printCommands(vector<BaseOperator*>* pList) {
	    if (pList == NULL)
	       return;

        for (int i=0; i< pList->size(); i++) {
           cout <<"op[" << i << "] = " << (*pList)[i]->toString().c_str() << endl;
        }
	};
	
#line 32 "speparser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 76
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE Token
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  vector<Expr*>* yy29;
  vector<GroupExpr*>* yy32;
  GroupExpr* yy37;
  PathExpr* yy41;
  vector<string>* yy51;
  string* yy52;
  RecordMemberExpr* yy65;
  RecordConstructExpr* yy76;
  vector<BaseOperator*> * yy77;
  BaseOperator* yy94;
  vector<RecordMemberExpr*>* yy100;
  Expr* yy122;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL ParseContext* pParseContext;
#define ParseARG_PDECL ,ParseContext* pParseContext
#define ParseARG_FETCH ParseContext* pParseContext = yypParser->pParseContext
#define ParseARG_STORE yypParser->pParseContext = pParseContext
#define YYNSTATE 156
#define YYNRULE 81
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
#define YY_ACTTAB_COUNT (672)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    23,   22,   21,   29,   28,   27,   26,   25,   24,   35,
 /*    10 */    33,   32,   31,   30,   23,   22,   21,   29,   28,   27,
 /*    20 */    26,   25,   24,   35,   33,   32,   31,   30,   23,   22,
 /*    30 */    21,   29,   28,   27,   26,   25,   24,   35,   33,   32,
 /*    40 */    31,   30,  155,   16,  147,  120,   22,   21,   29,   28,
 /*    50 */    27,   26,   25,   24,   35,   33,   32,   31,   30,  119,
 /*    60 */   146,   87,  144,   23,   22,   21,   29,   28,   27,   26,
 /*    70 */    25,   24,   35,   33,   32,   31,   30,   43,   39,   20,
 /*    80 */   113,   94,   17,   38,  143,   96,   81,  109,   19,   34,
 /*    90 */   130,  141,  100,  103,  153,  152,  151,  150,  149,  148,
 /*   100 */   142,  140,  139,  138,  137,   37,   32,   31,   30,    8,
 /*   110 */   128,  117,  101,   10,   18,  125,   20,  105,   94,   48,
 /*   120 */   145,  143,   45,    7,  116,  127,   34,   40,  141,  114,
 /*   130 */   132,   35,   33,   32,   31,   30,  118,  142,  140,  139,
 /*   140 */   138,  137,   37,  112,  110,   20,    8,   94,  117,   39,
 /*   150 */   143,   43,   44,   17,   91,   34,   96,  141,   46,  111,
 /*   160 */    90,  129,   19,  108,   89,  124,  142,  140,  139,  138,
 /*   170 */   137,   37,   39,   76,  117,    8,   17,  117,   95,   96,
 /*   180 */    29,   28,   27,   26,   25,   24,   35,   33,   32,   31,
 /*   190 */    30,  100,  103,  153,  152,  151,  150,  149,  148,  156,
 /*   200 */    88,   86,  103,  153,  152,  151,  150,  149,  148,  238,
 /*   210 */     1,   82,   14,    3,   96,    4,   71,   46,   99,   93,
 /*   220 */    98,   36,   15,   92,  122,    6,   13,   44,  107,   75,
 /*   230 */     9,  106,   73,   83,  103,  153,  152,  151,  150,  149,
 /*   240 */   148,   85,  103,  153,  152,  151,  150,  149,  148,  117,
 /*   250 */    84,  102,   99,  115,   98,   36,   15,   92,   12,    2,
 /*   260 */    18,   11,    5,   75,  123,    9,   73,  104,   74,   48,
 /*   270 */   145,  126,   45,    7,  116,  239,  121,   78,  121,   79,
 /*   280 */   132,  104,   72,   48,  145,   96,   45,    7,  116,   42,
 /*   290 */   239,   71,  239,   99,  132,   98,   36,   15,   92,  239,
 /*   300 */   239,  239,  239,  239,   75,  239,  239,   73,  154,  153,
 /*   310 */   152,  151,  150,  149,  148,   99,  239,   98,   36,   15,
 /*   320 */    92,  239,  239,  239,  239,  239,   75,  239,  239,   73,
 /*   330 */    56,  145,   80,   45,    7,  116,  239,  239,  239,  239,
 /*   340 */   239,  132,   56,  145,   77,   45,    7,  116,  239,  239,
 /*   350 */   239,  239,   93,  132,   59,  145,  239,   45,    7,  116,
 /*   360 */    44,  239,  239,  239,  239,  132,  239,  239,   69,  145,
 /*   370 */   239,   45,    7,  116,  239,  239,  239,  131,  239,  132,
 /*   380 */    58,   97,  117,   41,    7,  116,  239,  239,  239,  239,
 /*   390 */   239,  132,   70,  145,  239,   45,    7,  116,  239,  239,
 /*   400 */   239,  239,  239,  132,  136,  145,  239,   45,    7,  116,
 /*   410 */   239,  239,  239,  239,  239,  132,  135,  145,  239,   45,
 /*   420 */     7,  116,  239,  239,  239,  239,  239,  132,  239,  134,
 /*   430 */   145,  239,   45,    7,  116,  239,  239,  239,  239,  239,
 /*   440 */   132,   68,  145,  239,   45,    7,  116,  239,  239,  239,
 /*   450 */   239,  239,  132,   67,  145,  239,   45,    7,  116,  239,
 /*   460 */   239,  239,  239,  239,  132,   66,  145,  239,   45,    7,
 /*   470 */   116,  239,  239,  239,  239,  239,  132,   65,  145,  239,
 /*   480 */    45,    7,  116,  239,  239,  239,  239,  239,  132,  239,
 /*   490 */    64,  145,  239,   45,    7,  116,  239,  239,  239,  239,
 /*   500 */   239,  132,   63,  145,  239,   45,    7,  116,  239,  239,
 /*   510 */   239,  239,  239,  132,   60,  145,  239,   45,    7,  116,
 /*   520 */   239,  239,  239,  239,  239,  132,   62,  145,  239,   45,
 /*   530 */     7,  116,  239,  239,  239,  239,  239,  132,   61,  145,
 /*   540 */   239,   45,    7,  116,  239,  239,  239,  239,  239,  132,
 /*   550 */   239,  133,  145,  239,   45,    7,  116,  239,  239,  239,
 /*   560 */   239,  239,  132,   57,  145,  239,   45,    7,  116,  239,
 /*   570 */   239,  239,  239,  239,  132,   55,  145,  239,   45,    7,
 /*   580 */   116,  239,  239,  239,  239,  239,  132,   47,  145,  239,
 /*   590 */    45,    7,  116,  239,  239,  239,  239,  239,  132,   49,
 /*   600 */   145,  239,   45,    7,  116,  239,  239,  239,  239,  239,
 /*   610 */   132,  239,   54,  145,  239,   45,    7,  116,  239,  239,
 /*   620 */   239,  239,  239,  132,   53,  145,  239,   45,    7,  116,
 /*   630 */   239,  239,  239,  239,  239,  132,   52,  145,  239,   45,
 /*   640 */     7,  116,  239,  239,  239,  239,  239,  132,   51,  145,
 /*   650 */   239,   45,    7,  116,  239,  239,  239,  239,  239,  132,
 /*   660 */    50,  145,  239,   45,    7,  116,  239,  239,  239,  239,
 /*   670 */   239,  132,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,    1,    2,    3,    4,    5,    6,
 /*    20 */     7,    8,    9,   10,   11,   12,   13,   14,    1,    2,
 /*    30 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    40 */    13,   14,   16,   44,   20,   46,    2,    3,    4,    5,
 /*    50 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   46,
 /*    60 */    20,   34,   28,    1,    2,    3,    4,    5,    6,    7,
 /*    70 */     8,    9,   10,   11,   12,   13,   14,   63,   41,   15,
 /*    80 */    43,   17,   45,   35,   20,   48,   72,   73,   74,   25,
 /*    90 */    42,   27,   50,   51,   52,   53,   54,   55,   56,   57,
 /*   100 */    36,   37,   38,   39,   40,   41,   12,   13,   14,   45,
 /*   110 */    46,   47,   70,   18,   35,   17,   15,   58,   17,   60,
 /*   120 */    61,   20,   63,   64,   65,   46,   25,   11,   27,   28,
 /*   130 */    71,   10,   11,   12,   13,   14,   25,   36,   37,   38,
 /*   140 */    39,   40,   41,   44,   17,   15,   45,   17,   47,   41,
 /*   150 */    20,   63,   25,   45,   25,   25,   48,   27,   35,   28,
 /*   160 */    26,   73,   74,   28,   29,   42,   36,   37,   38,   39,
 /*   170 */    40,   41,   41,   27,   47,   45,   45,   47,   12,   48,
 /*   180 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   190 */    14,   50,   51,   52,   53,   54,   55,   56,   57,    0,
 /*   200 */    27,   50,   51,   52,   53,   54,   55,   56,   57,   68,
 /*   210 */    69,   70,   31,   32,   48,    8,   17,   35,   19,   17,
 /*   220 */    21,   22,   23,   24,   42,   18,   31,   25,   28,   30,
 /*   230 */    35,   17,   33,   50,   51,   52,   53,   54,   55,   56,
 /*   240 */    57,   50,   51,   52,   53,   54,   55,   56,   57,   47,
 /*   250 */    17,   16,   19,   28,   21,   22,   23,   24,   31,   32,
 /*   260 */    35,   31,   18,   30,   66,   35,   33,   58,   59,   60,
 /*   270 */    61,   66,   63,   64,   65,   75,   66,   67,   66,   67,
 /*   280 */    71,   58,   59,   60,   61,   48,   63,   64,   65,   63,
 /*   290 */    75,   17,   75,   19,   71,   21,   22,   23,   24,   75,
 /*   300 */    75,   75,   75,   75,   30,   75,   75,   33,   51,   52,
 /*   310 */    53,   54,   55,   56,   57,   19,   75,   21,   22,   23,
 /*   320 */    24,   75,   75,   75,   75,   75,   30,   75,   75,   33,
 /*   330 */    60,   61,   62,   63,   64,   65,   75,   75,   75,   75,
 /*   340 */    75,   71,   60,   61,   62,   63,   64,   65,   75,   75,
 /*   350 */    75,   75,   17,   71,   60,   61,   75,   63,   64,   65,
 /*   360 */    25,   75,   75,   75,   75,   71,   75,   75,   60,   61,
 /*   370 */    75,   63,   64,   65,   75,   75,   75,   42,   75,   71,
 /*   380 */    60,   61,   47,   63,   64,   65,   75,   75,   75,   75,
 /*   390 */    75,   71,   60,   61,   75,   63,   64,   65,   75,   75,
 /*   400 */    75,   75,   75,   71,   60,   61,   75,   63,   64,   65,
 /*   410 */    75,   75,   75,   75,   75,   71,   60,   61,   75,   63,
 /*   420 */    64,   65,   75,   75,   75,   75,   75,   71,   75,   60,
 /*   430 */    61,   75,   63,   64,   65,   75,   75,   75,   75,   75,
 /*   440 */    71,   60,   61,   75,   63,   64,   65,   75,   75,   75,
 /*   450 */    75,   75,   71,   60,   61,   75,   63,   64,   65,   75,
 /*   460 */    75,   75,   75,   75,   71,   60,   61,   75,   63,   64,
 /*   470 */    65,   75,   75,   75,   75,   75,   71,   60,   61,   75,
 /*   480 */    63,   64,   65,   75,   75,   75,   75,   75,   71,   75,
 /*   490 */    60,   61,   75,   63,   64,   65,   75,   75,   75,   75,
 /*   500 */    75,   71,   60,   61,   75,   63,   64,   65,   75,   75,
 /*   510 */    75,   75,   75,   71,   60,   61,   75,   63,   64,   65,
 /*   520 */    75,   75,   75,   75,   75,   71,   60,   61,   75,   63,
 /*   530 */    64,   65,   75,   75,   75,   75,   75,   71,   60,   61,
 /*   540 */    75,   63,   64,   65,   75,   75,   75,   75,   75,   71,
 /*   550 */    75,   60,   61,   75,   63,   64,   65,   75,   75,   75,
 /*   560 */    75,   75,   71,   60,   61,   75,   63,   64,   65,   75,
 /*   570 */    75,   75,   75,   75,   71,   60,   61,   75,   63,   64,
 /*   580 */    65,   75,   75,   75,   75,   75,   71,   60,   61,   75,
 /*   590 */    63,   64,   65,   75,   75,   75,   75,   75,   71,   60,
 /*   600 */    61,   75,   63,   64,   65,   75,   75,   75,   75,   75,
 /*   610 */    71,   75,   60,   61,   75,   63,   64,   65,   75,   75,
 /*   620 */    75,   75,   75,   71,   60,   61,   75,   63,   64,   65,
 /*   630 */    75,   75,   75,   75,   75,   71,   60,   61,   75,   63,
 /*   640 */    64,   65,   75,   75,   75,   75,   75,   71,   60,   61,
 /*   650 */    75,   63,   64,   65,   75,   75,   75,   75,   75,   71,
 /*   660 */    60,   61,   75,   63,   64,   65,   75,   75,   75,   75,
 /*   670 */    75,   71,
};
#define YY_SHIFT_USE_DFLT (-2)
#define YY_SHIFT_COUNT (101)
#define YY_SHIFT_MIN   (-1)
#define YY_SHIFT_MAX   (335)
static const short yy_shift_ofst[] = {
 /*     0 */   274,  199,  130,  130,  233,  296,  296,  101,   64,  130,
 /*    10 */   296,  130,  130,  130,  130,  130,  130,  130,  130,  130,
 /*    20 */   130,  130,  130,  130,  130,  130,  130,  130,  130,  130,
 /*    30 */   130,  130,  130,  130,  130,  130,  130,  335,  202,  166,
 /*    40 */   237,  131,  131,   37,  127,  108,  237,   -1,   27,   13,
 /*    50 */    62,   62,   62,   62,   62,   62,   62,   62,   62,   62,
 /*    60 */    44,  176,  176,  121,  121,  121,  121,  121,  121,   94,
 /*    70 */    94,  207,  230,  227,  195,  181,  135,  225,  182,  123,
 /*    80 */    79,   48,  235,   95,  244,   95,   95,  214,  200,  173,
 /*    90 */   146,  134,  129,   99,  111,  116,   98,   34,   40,   24,
 /*   100 */    95,   26,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (46)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (600)
static const short yy_reduce_ofst[] = {
 /*     0 */   141,   42,  223,  209,  191,  183,  151,  282,  270,   59,
 /*    10 */   257,  600,  588,  576,  564,  552,  539,  527,  515,  503,
 /*    20 */   491,  478,  466,  454,  442,  430,  417,  405,  393,  381,
 /*    30 */   369,  356,  344,  332,  320,  308,  294,   14,   88,  212,
 /*    40 */   210,  205,  205,  205,  226,  205,  198,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   237,  237,  237,  237,  237,  237,  237,  237,  237,  237,
 /*    10 */   237,  237,  237,  237,  237,  237,  237,  237,  237,  237,
 /*    20 */   237,  237,  237,  237,  237,  237,  237,  237,  237,  237,
 /*    30 */   237,  237,  237,  237,  237,  237,  237,  237,  237,  237,
 /*    40 */   237,  221,  237,  237,  237,  221,  237,  237,  181,  237,
 /*    50 */   180,  179,  178,  177,  174,  220,  219,  214,  237,  173,
 /*    60 */   205,  208,  206,  204,  203,  202,  201,  200,  199,  194,
 /*    70 */   195,  237,  237,  237,  237,  237,  237,  237,  237,  237,
 /*    80 */   237,  237,  237,  162,  237,  161,  160,  237,  237,  237,
 /*    90 */   237,  237,  237,  223,  223,  237,  237,  185,  237,  237,
 /*   100 */   159,  237,  157,  163,  183,  184,  182,  176,  175,  212,
 /*   110 */   223,  222,  216,  215,  235,  236,  233,  224,  234,  232,
 /*   120 */   231,  229,  228,  230,  227,  226,  225,  218,  217,  213,
 /*   130 */   211,  210,  209,  207,  198,  197,  196,  193,  192,  191,
 /*   140 */   190,  189,  188,  187,  186,  185,  172,  171,  170,  169,
 /*   150 */   168,  167,  166,  165,  164,  158,
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "OR",            "AND",           "IN",          
  "LT",            "GT",            "LE",            "GE",          
  "EQ",            "NE",            "ADD",           "SUB",         
  "MUL",           "DIV",           "MOD",           "NOT",         
  "SEMI",          "ID",            "PIPE",          "SSTREAM",     
  "DQ_STRING",     "OSTREAM",       "FILTER",        "TRANSFORM",   
  "WINDOW_KW",     "LP",            "ROWS",          "INT",         
  "RP",            "SLIDE",         "GROUP",         "INTO",        
  "BY",            "REDUCE",        "AS",            "COMMA",       
  "SQ_STRING",     "DOUBLE",        "TRUE",          "FALSE",       
  "NUL",           "LCB",           "RCB",           "DOT_STAR",    
  "COLON",         "LSB",           "RSB",           "DOLLAR",      
  "DOT",           "error",         "commands",      "command",     
  "stream_cmd",    "filter_cmd",    "transform_cmd",  "sliding_count_window_cmd",
  "group_cmd",     "reduce_cmd",    "group_expr",    "group_exprs", 
  "expr",          "pri_expr",      "exprs",         "path_expr",   
  "func_name",     "func_expr",     "project_name",  "project_names",
  "main",          "epn",           "stmt",          "record",      
  "fields",        "field",         "field_name",  
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "main ::= epn",
 /*   1 */ "epn ::= stmt SEMI",
 /*   2 */ "epn ::= epn stmt SEMI",
 /*   3 */ "stmt ::= commands",
 /*   4 */ "stmt ::= ID PIPE commands",
 /*   5 */ "stmt ::= ID EQ commands",
 /*   6 */ "stmt ::= ID EQ ID PIPE commands",
 /*   7 */ "commands ::= command",
 /*   8 */ "commands ::= commands PIPE command",
 /*   9 */ "command ::= stream_cmd",
 /*  10 */ "command ::= filter_cmd",
 /*  11 */ "command ::= transform_cmd",
 /*  12 */ "command ::= sliding_count_window_cmd",
 /*  13 */ "command ::= group_cmd",
 /*  14 */ "command ::= reduce_cmd",
 /*  15 */ "stream_cmd ::= SSTREAM DQ_STRING",
 /*  16 */ "stream_cmd ::= OSTREAM DQ_STRING",
 /*  17 */ "filter_cmd ::= FILTER expr",
 /*  18 */ "transform_cmd ::= TRANSFORM expr",
 /*  19 */ "sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT RP",
 /*  20 */ "sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT SLIDE INT RP",
 /*  21 */ "group_cmd ::= GROUP INTO expr",
 /*  22 */ "group_cmd ::= GROUP BY group_exprs INTO expr",
 /*  23 */ "reduce_cmd ::= REDUCE INTO expr",
 /*  24 */ "reduce_cmd ::= REDUCE BY group_exprs INTO expr",
 /*  25 */ "group_expr ::= expr",
 /*  26 */ "group_expr ::= expr AS ID",
 /*  27 */ "group_exprs ::= group_expr",
 /*  28 */ "group_exprs ::= group_exprs COMMA group_expr",
 /*  29 */ "expr ::= pri_expr",
 /*  30 */ "pri_expr ::= LP pri_expr RP",
 /*  31 */ "pri_expr ::= DQ_STRING",
 /*  32 */ "pri_expr ::= SQ_STRING",
 /*  33 */ "pri_expr ::= INT",
 /*  34 */ "pri_expr ::= DOUBLE",
 /*  35 */ "pri_expr ::= TRUE",
 /*  36 */ "pri_expr ::= FALSE",
 /*  37 */ "pri_expr ::= NUL",
 /*  38 */ "pri_expr ::= expr ADD expr",
 /*  39 */ "pri_expr ::= expr SUB expr",
 /*  40 */ "pri_expr ::= expr MUL expr",
 /*  41 */ "pri_expr ::= expr DIV expr",
 /*  42 */ "pri_expr ::= expr MOD expr",
 /*  43 */ "pri_expr ::= expr LT expr",
 /*  44 */ "pri_expr ::= expr GT expr",
 /*  45 */ "pri_expr ::= expr LE expr",
 /*  46 */ "pri_expr ::= expr GE expr",
 /*  47 */ "pri_expr ::= expr EQ expr",
 /*  48 */ "pri_expr ::= expr NE expr",
 /*  49 */ "pri_expr ::= expr OR expr",
 /*  50 */ "pri_expr ::= expr AND expr",
 /*  51 */ "pri_expr ::= NOT expr",
 /*  52 */ "pri_expr ::= expr IN expr",
 /*  53 */ "expr ::= record",
 /*  54 */ "record ::= LCB RCB",
 /*  55 */ "record ::= LCB fields RCB",
 /*  56 */ "fields ::= field",
 /*  57 */ "fields ::= fields COMMA field",
 /*  58 */ "field ::= field_name expr",
 /*  59 */ "field ::= path_expr DOT_STAR",
 /*  60 */ "field_name ::= ID COLON",
 /*  61 */ "expr ::= LSB RSB",
 /*  62 */ "expr ::= LSB exprs RSB",
 /*  63 */ "exprs ::= expr",
 /*  64 */ "exprs ::= exprs COMMA expr",
 /*  65 */ "expr ::= path_expr",
 /*  66 */ "path_expr ::= LP path_expr RP",
 /*  67 */ "path_expr ::= ID",
 /*  68 */ "path_expr ::= DOLLAR",
 /*  69 */ "path_expr ::= path_expr project_name",
 /*  70 */ "project_name ::= DOT ID",
 /*  71 */ "path_expr ::= path_expr LCB project_names RCB",
 /*  72 */ "path_expr ::= path_expr LCB MUL SUB project_names RCB",
 /*  73 */ "project_names ::= project_name",
 /*  74 */ "project_names ::= project_names COMMA project_name",
 /*  75 */ "path_expr ::= path_expr LSB expr RSB",
 /*  76 */ "path_expr ::= path_expr LSB expr COLON expr RSB",
 /*  77 */ "expr ::= func_expr",
 /*  78 */ "func_name ::= ID LP",
 /*  79 */ "func_expr ::= func_name RP",
 /*  80 */ "func_expr ::= func_name exprs RP",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_COUNT
   || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 68, 1 },
  { 69, 2 },
  { 69, 3 },
  { 70, 1 },
  { 70, 3 },
  { 70, 3 },
  { 70, 5 },
  { 50, 1 },
  { 50, 3 },
  { 51, 1 },
  { 51, 1 },
  { 51, 1 },
  { 51, 1 },
  { 51, 1 },
  { 51, 1 },
  { 52, 2 },
  { 52, 2 },
  { 53, 2 },
  { 54, 2 },
  { 55, 5 },
  { 55, 7 },
  { 56, 3 },
  { 56, 5 },
  { 57, 3 },
  { 57, 5 },
  { 58, 1 },
  { 58, 3 },
  { 59, 1 },
  { 59, 3 },
  { 60, 1 },
  { 61, 3 },
  { 61, 1 },
  { 61, 1 },
  { 61, 1 },
  { 61, 1 },
  { 61, 1 },
  { 61, 1 },
  { 61, 1 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 3 },
  { 61, 2 },
  { 61, 3 },
  { 60, 1 },
  { 71, 2 },
  { 71, 3 },
  { 72, 1 },
  { 72, 3 },
  { 73, 2 },
  { 73, 2 },
  { 74, 2 },
  { 60, 2 },
  { 60, 3 },
  { 62, 1 },
  { 62, 3 },
  { 60, 1 },
  { 63, 3 },
  { 63, 1 },
  { 63, 1 },
  { 63, 2 },
  { 66, 2 },
  { 63, 4 },
  { 63, 6 },
  { 67, 1 },
  { 67, 3 },
  { 63, 4 },
  { 63, 6 },
  { 60, 1 },
  { 64, 2 },
  { 65, 2 },
  { 65, 3 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 4: /* stmt ::= ID PIPE commands */
#line 67 "speparser.y"
{
      std::cout << "sth like 'c1->operator' is resolved." << std::endl;
      printCommands(yymsp[0].minor.yy77);  
      // g_pCommands = yymsp[0].minor.yy77;    

	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy77
	  ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == bPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  bPoint->addTargetPipeline(yymsp[0].minor.yy77);      
   }
#line 1027 "speparser.c"
        break;
      case 5: /* stmt ::= ID EQ commands */
#line 81 "speparser.y"
{
	  std::cout << "sth like 'c1 = operator->...' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy77); 
	  // g_pCommands = yymsp[0].minor.yy77;  

	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-2].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' already exists." << std::endl;		 
	  }	  
   }
#line 1042 "speparser.c"
        break;
      case 6: /* stmt ::= ID EQ ID PIPE commands */
#line 93 "speparser.y"
{
	  std::cout << "sth like 'c2 = c1->operator' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy77);
	  // g_pCommands = yymsp[0].minor.yy77;  
	  
	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-4].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-4].minor.yy0.z << "' already exists." << std::endl;		 
	  }
	  
	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy77
	  ConnectPointOperator* cPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == cPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  cPoint->addTargetPipeline(yymsp[0].minor.yy77);
   }
#line 1065 "speparser.c"
        break;
      case 7: /* commands ::= command */
#line 113 "speparser.y"
{
      yygotominor.yy77 = new vector<BaseOperator*>();
      yygotominor.yy77->push_back(yymsp[0].minor.yy94);
      
      // set current pipeline, add it into all pipelines
      pParseContext->curPipeline = yygotominor.yy77;
      pParseContext->allPipelines.push_back(yygotominor.yy77);
}
#line 1077 "speparser.c"
        break;
      case 8: /* commands ::= commands PIPE command */
#line 122 "speparser.y"
{
      yygotominor.yy77 = yymsp[-2].minor.yy77;
      yygotominor.yy77->push_back(yymsp[0].minor.yy94);
}
#line 1085 "speparser.c"
        break;
      case 9: /* command ::= stream_cmd */
      case 10: /* command ::= filter_cmd */ yytestcase(yyruleno==10);
      case 11: /* command ::= transform_cmd */ yytestcase(yyruleno==11);
      case 12: /* command ::= sliding_count_window_cmd */ yytestcase(yyruleno==12);
      case 13: /* command ::= group_cmd */ yytestcase(yyruleno==13);
      case 14: /* command ::= reduce_cmd */ yytestcase(yyruleno==14);
#line 127 "speparser.y"
{
      yygotominor.yy94 = yymsp[0].minor.yy94;
}
#line 1097 "speparser.c"
        break;
      case 15: /* stream_cmd ::= SSTREAM DQ_STRING */
#line 149 "speparser.y"
{
      yygotominor.yy94 = new StreamOperator(yymsp[0].minor.yy0.z, 0);
}
#line 1104 "speparser.c"
        break;
      case 16: /* stream_cmd ::= OSTREAM DQ_STRING */
#line 153 "speparser.y"
{
      yygotominor.yy94 = new StreamOperator(yymsp[0].minor.yy0.z, 1);
}
#line 1111 "speparser.c"
        break;
      case 17: /* filter_cmd ::= FILTER expr */
#line 157 "speparser.y"
{
      yygotominor.yy94 = new FilterOperator(yymsp[0].minor.yy122);
}
#line 1118 "speparser.c"
        break;
      case 18: /* transform_cmd ::= TRANSFORM expr */
#line 161 "speparser.y"
{
	  yygotominor.yy94 = new TransformOperator(yymsp[0].minor.yy122);
}
#line 1125 "speparser.c"
        break;
      case 19: /* sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT RP */
#line 165 "speparser.y"
{
	yygotominor.yy94 = new SlidingCountWindowOperator(atoi(yymsp[-1].minor.yy0.z), 1);
}
#line 1132 "speparser.c"
        break;
      case 20: /* sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT SLIDE INT RP */
#line 169 "speparser.y"
{
	yygotominor.yy94 = new SlidingCountWindowOperator(atoi(yymsp[-3].minor.yy0.z), atoi(yymsp[-1].minor.yy0.z));
}
#line 1139 "speparser.c"
        break;
      case 21: /* group_cmd ::= GROUP INTO expr */
#line 173 "speparser.y"
{
    yygotominor.yy94 = new GroupOperator(NULL, yymsp[0].minor.yy122);
}
#line 1146 "speparser.c"
        break;
      case 22: /* group_cmd ::= GROUP BY group_exprs INTO expr */
#line 177 "speparser.y"
{
    yygotominor.yy94 = new GroupOperator(yymsp[-2].minor.yy32, yymsp[0].minor.yy122);
}
#line 1153 "speparser.c"
        break;
      case 23: /* reduce_cmd ::= REDUCE INTO expr */
#line 181 "speparser.y"
{
    yygotominor.yy94 = new ReduceOperator(NULL, yymsp[0].minor.yy122);
}
#line 1160 "speparser.c"
        break;
      case 24: /* reduce_cmd ::= REDUCE BY group_exprs INTO expr */
#line 185 "speparser.y"
{
    yygotominor.yy94 = new ReduceOperator(yymsp[-2].minor.yy32, yymsp[0].minor.yy122);
}
#line 1167 "speparser.c"
        break;
      case 25: /* group_expr ::= expr */
#line 189 "speparser.y"
{
    yygotominor.yy37 = new GroupExpr(yymsp[0].minor.yy122, "");
}
#line 1174 "speparser.c"
        break;
      case 26: /* group_expr ::= expr AS ID */
#line 193 "speparser.y"
{
    yygotominor.yy37 = new GroupExpr(yymsp[-2].minor.yy122, yymsp[0].minor.yy0.z);
}
#line 1181 "speparser.c"
        break;
      case 27: /* group_exprs ::= group_expr */
#line 197 "speparser.y"
{
    yygotominor.yy32 = new vector<GroupExpr*>();
    yygotominor.yy32->push_back(yymsp[0].minor.yy37);
}
#line 1189 "speparser.c"
        break;
      case 28: /* group_exprs ::= group_exprs COMMA group_expr */
#line 202 "speparser.y"
{
    yymsp[-2].minor.yy32->push_back(yymsp[0].minor.yy37);
    yygotominor.yy32 = yymsp[-2].minor.yy32;
}
#line 1197 "speparser.c"
        break;
      case 29: /* expr ::= pri_expr */
      case 77: /* expr ::= func_expr */ yytestcase(yyruleno==77);
#line 207 "speparser.y"
{
	yygotominor.yy122 = yymsp[0].minor.yy122;
}
#line 1205 "speparser.c"
        break;
      case 30: /* pri_expr ::= LP pri_expr RP */
#line 211 "speparser.y"
{
      yygotominor.yy122 = yymsp[-1].minor.yy122;
}
#line 1212 "speparser.c"
        break;
      case 31: /* pri_expr ::= DQ_STRING */
      case 32: /* pri_expr ::= SQ_STRING */ yytestcase(yyruleno==32);
#line 215 "speparser.y"
{
      // strip off the quote char
      std::string s(yymsp[0].minor.yy0.z+1, strlen(yymsp[0].minor.yy0.z)-2);
      yygotominor.yy122 = new StringValue(s.c_str());
}
#line 1222 "speparser.c"
        break;
      case 33: /* pri_expr ::= INT */
#line 227 "speparser.y"
{
      yygotominor.yy122 = new IntValue(atoi(yymsp[0].minor.yy0.z));
}
#line 1229 "speparser.c"
        break;
      case 34: /* pri_expr ::= DOUBLE */
#line 231 "speparser.y"
{
      yygotominor.yy122 = new DoubleValue(atof(yymsp[0].minor.yy0.z));
}
#line 1236 "speparser.c"
        break;
      case 35: /* pri_expr ::= TRUE */
#line 235 "speparser.y"
{
      yygotominor.yy122 = new BoolValue(true);
}
#line 1243 "speparser.c"
        break;
      case 36: /* pri_expr ::= FALSE */
#line 239 "speparser.y"
{
      yygotominor.yy122 = new BoolValue(false);
}
#line 1250 "speparser.c"
        break;
      case 37: /* pri_expr ::= NUL */
#line 242 "speparser.y"
{
      yygotominor.yy122 = new NullValue();
}
#line 1257 "speparser.c"
        break;
      case 38: /* pri_expr ::= expr ADD expr */
#line 246 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_ADD, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1264 "speparser.c"
        break;
      case 39: /* pri_expr ::= expr SUB expr */
#line 250 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_SUB, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1271 "speparser.c"
        break;
      case 40: /* pri_expr ::= expr MUL expr */
#line 254 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_MUL, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1278 "speparser.c"
        break;
      case 41: /* pri_expr ::= expr DIV expr */
#line 257 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_DIV, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1285 "speparser.c"
        break;
      case 42: /* pri_expr ::= expr MOD expr */
#line 261 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_MOD, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1292 "speparser.c"
        break;
      case 43: /* pri_expr ::= expr LT expr */
#line 265 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_LT, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1299 "speparser.c"
        break;
      case 44: /* pri_expr ::= expr GT expr */
#line 269 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_GT, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1306 "speparser.c"
        break;
      case 45: /* pri_expr ::= expr LE expr */
#line 273 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_LE, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1313 "speparser.c"
        break;
      case 46: /* pri_expr ::= expr GE expr */
#line 277 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_GE, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1320 "speparser.c"
        break;
      case 47: /* pri_expr ::= expr EQ expr */
#line 281 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_EQ, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1327 "speparser.c"
        break;
      case 48: /* pri_expr ::= expr NE expr */
#line 285 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_NE, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1334 "speparser.c"
        break;
      case 49: /* pri_expr ::= expr OR expr */
#line 289 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_OR, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1341 "speparser.c"
        break;
      case 50: /* pri_expr ::= expr AND expr */
#line 293 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_AND, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1348 "speparser.c"
        break;
      case 51: /* pri_expr ::= NOT expr */
#line 297 "speparser.y"
{
      yygotominor.yy122 = new UnaryExpr(Expr::OP_NOT, yymsp[0].minor.yy122);
}
#line 1355 "speparser.c"
        break;
      case 52: /* pri_expr ::= expr IN expr */
#line 301 "speparser.y"
{
      yygotominor.yy122 = new BinaryExpr(Expr::OP_IN, yymsp[-2].minor.yy122, yymsp[0].minor.yy122);
}
#line 1362 "speparser.c"
        break;
      case 53: /* expr ::= record */
#line 310 "speparser.y"
{
	yygotominor.yy122 = yymsp[0].minor.yy76;
}
#line 1369 "speparser.c"
        break;
      case 54: /* record ::= LCB RCB */
#line 314 "speparser.y"
{
	yygotominor.yy76 = new RecordConstructExpr( new vector<RecordMemberExpr*>());
}
#line 1376 "speparser.c"
        break;
      case 55: /* record ::= LCB fields RCB */
#line 318 "speparser.y"
{
	yygotominor.yy76 = new RecordConstructExpr(yymsp[-1].minor.yy100);
}
#line 1383 "speparser.c"
        break;
      case 56: /* fields ::= field */
#line 322 "speparser.y"
{
	yygotominor.yy100 = new vector<RecordMemberExpr*>();
	yygotominor.yy100->push_back(yymsp[0].minor.yy65);
}
#line 1391 "speparser.c"
        break;
      case 57: /* fields ::= fields COMMA field */
#line 327 "speparser.y"
{
	yymsp[-2].minor.yy100->push_back(yymsp[0].minor.yy65);
	yygotominor.yy100 = yymsp[-2].minor.yy100;
}
#line 1399 "speparser.c"
        break;
      case 58: /* field ::= field_name expr */
#line 332 "speparser.y"
{
	yygotominor.yy65 = new RecordMemberExpr(yymsp[-1].minor.yy52->c_str(), yymsp[0].minor.yy122);
}
#line 1406 "speparser.c"
        break;
      case 59: /* field ::= path_expr DOT_STAR */
#line 336 "speparser.y"
{
	yygotominor.yy65 = new RecordMemberExpr("*", yymsp[-1].minor.yy41);
}
#line 1413 "speparser.c"
        break;
      case 60: /* field_name ::= ID COLON */
#line 340 "speparser.y"
{
	yygotominor.yy52 = new string(yymsp[-1].minor.yy0.z);
}
#line 1420 "speparser.c"
        break;
      case 61: /* expr ::= LSB RSB */
#line 344 "speparser.y"
{
    yygotominor.yy122 = new ArrayConstructExpr();
}
#line 1427 "speparser.c"
        break;
      case 62: /* expr ::= LSB exprs RSB */
#line 348 "speparser.y"
{
	yygotominor.yy122 = new ArrayConstructExpr(yymsp[-1].minor.yy29);
}
#line 1434 "speparser.c"
        break;
      case 63: /* exprs ::= expr */
#line 352 "speparser.y"
{
	yygotominor.yy29 = new vector<Expr*>();
	yygotominor.yy29->push_back(yymsp[0].minor.yy122);
}
#line 1442 "speparser.c"
        break;
      case 64: /* exprs ::= exprs COMMA expr */
#line 357 "speparser.y"
{
	yygotominor.yy29 = yymsp[-2].minor.yy29;
	yygotominor.yy29->push_back(yymsp[0].minor.yy122);
}
#line 1450 "speparser.c"
        break;
      case 65: /* expr ::= path_expr */
#line 362 "speparser.y"
{
      yygotominor.yy122 = yymsp[0].minor.yy41;
}
#line 1457 "speparser.c"
        break;
      case 66: /* path_expr ::= LP path_expr RP */
#line 366 "speparser.y"
{
	  yygotominor.yy41 = yymsp[-1].minor.yy41;
}
#line 1464 "speparser.c"
        break;
      case 67: /* path_expr ::= ID */
#line 370 "speparser.y"
{
      yygotominor.yy41 = new VarRefExpr(yymsp[0].minor.yy0.z);
}
#line 1471 "speparser.c"
        break;
      case 68: /* path_expr ::= DOLLAR */
#line 373 "speparser.y"
{
      yygotominor.yy41 = new VarRefExpr("$");
}
#line 1478 "speparser.c"
        break;
      case 69: /* path_expr ::= path_expr project_name */
#line 377 "speparser.y"
{
      yymsp[-1].minor.yy41->addChildPathExpr(new StepExpr(yymsp[0].minor.yy52->c_str()) );
      yygotominor.yy41 = yymsp[-1].minor.yy41;
      delete yymsp[0].minor.yy52;
}
#line 1487 "speparser.c"
        break;
      case 70: /* project_name ::= DOT ID */
#line 383 "speparser.y"
{
      yygotominor.yy52 = new string(yymsp[0].minor.yy0.z);
}
#line 1494 "speparser.c"
        break;
      case 71: /* path_expr ::= path_expr LCB project_names RCB */
#line 387 "speparser.y"
{
	  yymsp[-3].minor.yy41->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy51, false));
	  yygotominor.yy41 = yymsp[-3].minor.yy41;
}
#line 1502 "speparser.c"
        break;
      case 72: /* path_expr ::= path_expr LCB MUL SUB project_names RCB */
#line 392 "speparser.y"
{
	  yymsp[-5].minor.yy41->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy51, true));
	  yygotominor.yy41 = yymsp[-5].minor.yy41;
}
#line 1510 "speparser.c"
        break;
      case 73: /* project_names ::= project_name */
#line 397 "speparser.y"
{
      yygotominor.yy51 = new vector<string>();
      yygotominor.yy51->push_back(string(yymsp[0].minor.yy52->c_str()));
      delete yymsp[0].minor.yy52; 
}
#line 1519 "speparser.c"
        break;
      case 74: /* project_names ::= project_names COMMA project_name */
#line 402 "speparser.y"
{
      yymsp[-2].minor.yy51->push_back(string(yymsp[0].minor.yy52->c_str()));
      yygotominor.yy51 = yymsp[-2].minor.yy51;
      delete yymsp[0].minor.yy52;
}
#line 1528 "speparser.c"
        break;
      case 75: /* path_expr ::= path_expr LSB expr RSB */
#line 408 "speparser.y"
{
	yymsp[-3].minor.yy41->addChildPathExpr(new ArrayElementsExpr(yymsp[-1].minor.yy122,NULL));
	yygotominor.yy41 = yymsp[-3].minor.yy41;	  
}
#line 1536 "speparser.c"
        break;
      case 76: /* path_expr ::= path_expr LSB expr COLON expr RSB */
#line 413 "speparser.y"
{  
	yymsp[-5].minor.yy41->addChildPathExpr(new ArrayElementsExpr(yymsp[-3].minor.yy122,yymsp[-1].minor.yy122));
	yygotominor.yy41 = yymsp[-5].minor.yy41;
}
#line 1544 "speparser.c"
        break;
      case 78: /* func_name ::= ID LP */
#line 423 "speparser.y"
{
	yygotominor.yy52 = new string(yymsp[-1].minor.yy0.z);
	printf("recongnize a function %s.\n ", yygotominor.yy52->c_str());
}
#line 1552 "speparser.c"
        break;
      case 79: /* func_expr ::= func_name RP */
#line 428 "speparser.y"
{
    vector<Expr*>* args = new vector<Expr*>();
	yygotominor.yy122 = createBuiltinFunction(yymsp[-1].minor.yy52->c_str(), args);
	if (!yygotominor.yy122) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-1].minor.yy52->c_str());
	}
	delete yymsp[-1].minor.yy52;
}
#line 1565 "speparser.c"
        break;
      case 80: /* func_expr ::= func_name exprs RP */
#line 438 "speparser.y"
{
	yygotominor.yy122 = createBuiltinFunction(yymsp[-2].minor.yy52->c_str(), yymsp[-1].minor.yy29);
	if (!yygotominor.yy122) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-2].minor.yy52->c_str());
	}
	delete yymsp[-2].minor.yy52;
}
#line 1577 "speparser.c"
        break;
      default:
      /* (0) main ::= epn */ yytestcase(yyruleno==0);
      /* (1) epn ::= stmt SEMI */ yytestcase(yyruleno==1);
      /* (2) epn ::= epn stmt SEMI */ yytestcase(yyruleno==2);
      /* (3) stmt ::= commands */ yytestcase(yyruleno==3);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 57 "speparser.y"

  /* The tokenizer always gives us a token */
  std::cout << "near at the Token (line #" << TOKEN.line << ", column #" << TOKEN.column << "), Syntax error!" << std::endl;
  pParseContext->errorCode = -1;
#line 1648 "speparser.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
