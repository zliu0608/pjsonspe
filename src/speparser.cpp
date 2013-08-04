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
#define YYNOCODE 78
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE Token
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  RecordMemberExpr* yy1;
  vector<Expr*>* yy5;
  vector<GroupExpr*>* yy20;
  PathExpr* yy55;
  vector<string>* yy59;
  string* yy82;
  RecordConstructExpr* yy104;
  vector<BaseOperator*> * yy115;
  vector<RecordMemberExpr*>* yy124;
  Expr* yy132;
  GroupExpr* yy133;
  BaseOperator* yy144;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL ParseContext* pParseContext;
#define ParseARG_PDECL ,ParseContext* pParseContext
#define ParseARG_FETCH ParseContext* pParseContext = yypParser->pParseContext
#define ParseARG_STORE yypParser->pParseContext = pParseContext
#define YYNSTATE 160
#define YYNRULE 84
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
#define YY_ACTTAB_COUNT (678)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    25,   24,   23,   31,   30,   29,   28,   27,   26,   37,
 /*    10 */    35,   34,   33,   32,   25,   24,   23,   31,   30,   29,
 /*    20 */    28,   27,   26,   37,   35,   34,   33,   32,   25,   24,
 /*    30 */    23,   31,   30,   29,   28,   27,   26,   37,   35,   34,
 /*    40 */    33,   32,  159,   39,   22,   18,   97,  124,   10,  147,
 /*    50 */   148,  151,  150,   34,   33,   32,   47,   36,   40,  145,
 /*    60 */   117,  123,   19,  114,   89,   99,  134,  146,  144,  143,
 /*    70 */   142,  141,   38,  101,   45,   11,    8,  129,  121,  128,
 /*    80 */    25,   24,   23,   31,   30,   29,   28,   27,   26,   37,
 /*    90 */    35,   34,   33,   32,   96,  121,   22,  108,   97,   49,
 /*   100 */   149,  147,   46,    7,  120,   45,   96,   41,   20,   36,
 /*   110 */   136,  145,   37,   35,   34,   33,   32,   45,  119,  146,
 /*   120 */   144,  143,  142,  141,   38,   20,  121,   40,    8,  132,
 /*   130 */   121,   19,  116,  135,   99,   22,    9,   97,  121,  122,
 /*   140 */   147,  158,  157,  156,  155,  154,  153,  152,   36,   14,
 /*   150 */   145,  118,  131,  111,   91,   44,  112,   92,  146,  144,
 /*   160 */   143,  142,  141,   38,   83,  113,   21,    8,   93,  121,
 /*   170 */    24,   23,   31,   30,   29,   28,   27,   26,   37,   35,
 /*   180 */    34,   33,   32,   47,    9,   22,   78,   97,   15,    3,
 /*   190 */   147,   58,  149,   82,   46,    7,  120,   12,   36,   90,
 /*   200 */   145,  109,  136,  110,   44,    5,  126,   99,  146,  144,
 /*   210 */   143,  142,  141,   38,  133,   21,  105,    8,  127,  121,
 /*   220 */    13,    2,   31,   30,   29,   28,   27,   26,   37,   35,
 /*   230 */    34,   33,   32,  103,  106,  157,  156,  155,  154,  153,
 /*   240 */   152,  160,  130,  103,  106,  157,  156,  155,  154,  153,
 /*   250 */   152,  245,    1,   84,    4,   43,  125,   80,   73,  246,
 /*   260 */   102,  125,   81,  104,    6,   95,   17,   16,   94,   98,
 /*   270 */   246,  246,  246,  246,   77,  246,  246,   75,   88,  106,
 /*   280 */   157,  156,  155,  154,  153,  152,   85,  106,  157,  156,
 /*   290 */   155,  154,  153,  152,   87,  106,  157,  156,  155,  154,
 /*   300 */   153,  152,   86,  246,  102,  246,  246,   99,  246,   95,
 /*   310 */    17,   16,   94,  246,  246,  246,  246,  246,   77,  246,
 /*   320 */   246,   75,  246,  246,  107,   76,   49,  149,  246,   46,
 /*   330 */     7,  120,  246,  246,  115,  246,  246,  136,  107,   74,
 /*   340 */    49,  149,  246,   46,    7,  120,   40,   73,  246,  102,
 /*   350 */    19,  136,  246,   99,   95,   17,   16,   94,  246,  246,
 /*   360 */   246,  246,  246,   77,  102,  246,   75,  246,  246,   95,
 /*   370 */    17,   16,   94,  246,  246,  246,  246,  246,   77,  246,
 /*   380 */   246,   75,  246,  246,   58,  149,   79,   46,    7,  120,
 /*   390 */   246,  246,  246,   71,  149,  136,   46,    7,  120,  246,
 /*   400 */   246,  246,   60,  100,  136,   42,    7,  120,  246,  246,
 /*   410 */   246,  246,  246,  136,   72,  149,  246,   46,    7,  120,
 /*   420 */   246,  246,  246,  246,  246,  136,  140,  149,  246,   46,
 /*   430 */     7,  120,  246,  246,  246,  139,  149,  136,   46,    7,
 /*   440 */   120,  246,  246,  246,  138,  149,  136,   46,    7,  120,
 /*   450 */   246,  246,  246,  246,  246,  136,   70,  149,  246,   46,
 /*   460 */     7,  120,  246,  246,  246,   69,  149,  136,   46,    7,
 /*   470 */   120,  246,  246,  246,  246,  246,  136,   68,  149,  246,
 /*   480 */    46,    7,  120,  246,  246,  246,  246,  246,  136,   67,
 /*   490 */   149,  246,   46,    7,  120,  246,  246,  246,   66,  149,
 /*   500 */   136,   46,    7,  120,  246,  246,  246,   65,  149,  136,
 /*   510 */    46,    7,  120,  246,  246,  246,  246,  246,  136,   62,
 /*   520 */   149,  246,   46,    7,  120,  246,  246,  246,   64,  149,
 /*   530 */   136,   46,    7,  120,  246,  246,  246,  246,  246,  136,
 /*   540 */    63,  149,  246,   46,    7,  120,  246,  246,  246,  246,
 /*   550 */   246,  136,  137,  149,  246,   46,    7,  120,  246,  246,
 /*   560 */   246,   59,  149,  136,   46,    7,  120,  246,  246,  246,
 /*   570 */    57,  149,  136,   46,    7,  120,  246,  246,  246,  246,
 /*   580 */   246,  136,   48,  149,  246,   46,    7,  120,  246,  246,
 /*   590 */   246,   50,  149,  136,   46,    7,  120,  246,  246,  246,
 /*   600 */   246,  246,  136,   56,  149,  246,   46,    7,  120,  246,
 /*   610 */   246,  246,  246,  246,  136,   55,  149,  246,   46,    7,
 /*   620 */   120,  246,  246,  246,   54,  149,  136,   46,    7,  120,
 /*   630 */   246,  246,  246,   53,  149,  136,   46,    7,  120,  246,
 /*   640 */   246,  246,  246,  246,  136,   52,  149,  246,   46,    7,
 /*   650 */   120,  246,  246,  246,   51,  149,  136,   46,    7,  120,
 /*   660 */   246,  246,  246,  246,  246,  136,   61,  149,  246,   46,
 /*   670 */     7,  120,  246,  246,  246,  246,  246,  136,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,    1,    2,    3,    4,    5,    6,
 /*    20 */     7,    8,    9,   10,   11,   12,   13,   14,    1,    2,
 /*    30 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    40 */    13,   14,   16,   21,   15,   46,   17,   48,   18,   20,
 /*    50 */    31,   22,   23,   12,   13,   14,   21,   28,   43,   30,
 /*    60 */    45,   48,   47,   17,   37,   50,   44,   38,   39,   40,
 /*    70 */    41,   42,   43,   20,   28,   21,   47,   17,   49,   44,
 /*    80 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    90 */    11,   12,   13,   14,   17,   49,   15,   60,   17,   62,
 /*   100 */    63,   20,   65,   66,   67,   28,   17,   11,   21,   28,
 /*   110 */    73,   30,   10,   11,   12,   13,   14,   28,   31,   38,
 /*   120 */    39,   40,   41,   42,   43,   21,   49,   43,   47,   48,
 /*   130 */    49,   47,   46,   44,   50,   15,   21,   17,   49,   28,
 /*   140 */    20,   53,   54,   55,   56,   57,   58,   59,   28,   34,
 /*   150 */    30,   31,   48,   31,   32,   65,   20,   29,   38,   39,
 /*   160 */    40,   41,   42,   43,   74,   75,   76,   47,   28,   49,
 /*   170 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   180 */    12,   13,   14,   21,   21,   15,   30,   17,   34,   35,
 /*   190 */    20,   62,   63,   64,   65,   66,   67,   34,   28,   30,
 /*   200 */    30,   17,   73,   31,   65,   18,   44,   50,   38,   39,
 /*   210 */    40,   41,   42,   43,   75,   76,   16,   47,   68,   49,
 /*   220 */    34,   35,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   230 */    12,   13,   14,   52,   53,   54,   55,   56,   57,   58,
 /*   240 */    59,    0,   68,   52,   53,   54,   55,   56,   57,   58,
 /*   250 */    59,   70,   71,   72,    8,   65,   68,   69,   17,   77,
 /*   260 */    19,   68,   69,   72,   18,   24,   25,   26,   27,   12,
 /*   270 */    77,   77,   77,   77,   33,   77,   77,   36,   52,   53,
 /*   280 */    54,   55,   56,   57,   58,   59,   52,   53,   54,   55,
 /*   290 */    56,   57,   58,   59,   52,   53,   54,   55,   56,   57,
 /*   300 */    58,   59,   17,   77,   19,   77,   77,   50,   77,   24,
 /*   310 */    25,   26,   27,   77,   77,   77,   77,   77,   33,   77,
 /*   320 */    77,   36,   77,   77,   60,   61,   62,   63,   77,   65,
 /*   330 */    66,   67,   77,   77,   31,   77,   77,   73,   60,   61,
 /*   340 */    62,   63,   77,   65,   66,   67,   43,   17,   77,   19,
 /*   350 */    47,   73,   77,   50,   24,   25,   26,   27,   77,   77,
 /*   360 */    77,   77,   77,   33,   19,   77,   36,   77,   77,   24,
 /*   370 */    25,   26,   27,   77,   77,   77,   77,   77,   33,   77,
 /*   380 */    77,   36,   77,   77,   62,   63,   64,   65,   66,   67,
 /*   390 */    77,   77,   77,   62,   63,   73,   65,   66,   67,   77,
 /*   400 */    77,   77,   62,   63,   73,   65,   66,   67,   77,   77,
 /*   410 */    77,   77,   77,   73,   62,   63,   77,   65,   66,   67,
 /*   420 */    77,   77,   77,   77,   77,   73,   62,   63,   77,   65,
 /*   430 */    66,   67,   77,   77,   77,   62,   63,   73,   65,   66,
 /*   440 */    67,   77,   77,   77,   62,   63,   73,   65,   66,   67,
 /*   450 */    77,   77,   77,   77,   77,   73,   62,   63,   77,   65,
 /*   460 */    66,   67,   77,   77,   77,   62,   63,   73,   65,   66,
 /*   470 */    67,   77,   77,   77,   77,   77,   73,   62,   63,   77,
 /*   480 */    65,   66,   67,   77,   77,   77,   77,   77,   73,   62,
 /*   490 */    63,   77,   65,   66,   67,   77,   77,   77,   62,   63,
 /*   500 */    73,   65,   66,   67,   77,   77,   77,   62,   63,   73,
 /*   510 */    65,   66,   67,   77,   77,   77,   77,   77,   73,   62,
 /*   520 */    63,   77,   65,   66,   67,   77,   77,   77,   62,   63,
 /*   530 */    73,   65,   66,   67,   77,   77,   77,   77,   77,   73,
 /*   540 */    62,   63,   77,   65,   66,   67,   77,   77,   77,   77,
 /*   550 */    77,   73,   62,   63,   77,   65,   66,   67,   77,   77,
 /*   560 */    77,   62,   63,   73,   65,   66,   67,   77,   77,   77,
 /*   570 */    62,   63,   73,   65,   66,   67,   77,   77,   77,   77,
 /*   580 */    77,   73,   62,   63,   77,   65,   66,   67,   77,   77,
 /*   590 */    77,   62,   63,   73,   65,   66,   67,   77,   77,   77,
 /*   600 */    77,   77,   73,   62,   63,   77,   65,   66,   67,   77,
 /*   610 */    77,   77,   77,   77,   73,   62,   63,   77,   65,   66,
 /*   620 */    67,   77,   77,   77,   62,   63,   73,   65,   66,   67,
 /*   630 */    77,   77,   77,   62,   63,   73,   65,   66,   67,   77,
 /*   640 */    77,   77,   77,   77,   73,   62,   63,   77,   65,   66,
 /*   650 */    67,   77,   77,   77,   62,   63,   73,   65,   66,   67,
 /*   660 */    77,   77,   77,   77,   77,   73,   62,   63,   77,   65,
 /*   670 */    66,   67,   77,   77,   77,   77,   77,   73,
};
#define YY_SHIFT_USE_DFLT (-2)
#define YY_SHIFT_COUNT (104)
#define YY_SHIFT_MIN   (-1)
#define YY_SHIFT_MAX   (345)
static const short yy_shift_ofst[] = {
 /*     0 */   330,  241,  170,  170,  285,  345,  345,  120,   81,  170,
 /*    10 */   345,   29,  170,  170,  170,  170,  170,  170,  170,  170,
 /*    20 */   170,  170,  170,  170,  170,  170,  170,  170,  170,  170,
 /*    30 */   170,  170,  170,  170,  170,  170,  170,  170,   89,   77,
 /*    40 */   257,  157,  303,  303,   15,   46,   84,  157,   -1,   27,
 /*    50 */    13,   79,   79,   79,   79,   79,   79,   79,   79,   79,
 /*    60 */    79,   79,  168,  218,  218,  102,  102,  102,  102,  102,
 /*    70 */   102,   41,   41,  246,  163,  186,  115,  154,  122,   87,
 /*    80 */   162,   35,  104,   22,  200,   30,  187,   30,   30,  184,
 /*    90 */   172,  169,  156,  128,  140,  136,   86,  111,   96,   60,
 /*   100 */    19,   54,   53,   30,   26,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (47)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (604)
static const short yy_reduce_ofst[] = {
 /*     0 */   181,  191,  278,  264,  242,  234,  226,  322,  129,   37,
 /*    10 */    88,  604,  592,  583,  571,  562,  553,  541,  529,  520,
 /*    20 */   508,  499,  490,  478,  466,  457,  445,  436,  427,  415,
 /*    30 */   403,  394,  382,  373,  364,  352,  340,  331,   90,  139,
 /*    40 */   193,  188,  174,  174,  174,  190,  174,  150,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   244,  244,  244,  244,  244,  244,  244,  244,  244,  244,
 /*    10 */   244,  244,  244,  244,  244,  244,  244,  244,  244,  244,
 /*    20 */   244,  244,  244,  244,  244,  244,  244,  244,  244,  244,
 /*    30 */   244,  244,  244,  244,  244,  244,  244,  244,  244,  244,
 /*    40 */   244,  244,  228,  244,  244,  244,  228,  244,  244,  188,
 /*    50 */   244,  187,  186,  185,  184,  181,  180,  227,  226,  221,
 /*    60 */   244,  178,  212,  215,  213,  211,  210,  209,  208,  207,
 /*    70 */   206,  201,  202,  244,  244,  244,  244,  244,  244,  244,
 /*    80 */   244,  244,  244,  244,  244,  166,  244,  165,  164,  244,
 /*    90 */   244,  244,  244,  244,  244,  244,  230,  230,  244,  244,
 /*   100 */   192,  175,  244,  163,  244,  161,  167,  190,  191,  189,
 /*   110 */   183,  182,  179,  219,  230,  229,  223,  222,  242,  243,
 /*   120 */   240,  231,  241,  239,  238,  236,  235,  237,  234,  233,
 /*   130 */   232,  225,  224,  220,  218,  217,  216,  214,  205,  204,
 /*   140 */   203,  200,  199,  198,  197,  196,  195,  194,  193,  192,
 /*   150 */   177,  176,  174,  173,  172,  171,  170,  169,  168,  162,
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
  "DQ_STRING",     "COMMA",         "SHUFFLE",       "ALL",         
  "OSTREAM",       "FILTER",        "TRANSFORM",     "WINDOW_KW",   
  "LP",            "ROWS",          "INT",           "RP",          
  "SLIDE",         "GROUP",         "INTO",          "BY",          
  "REDUCE",        "AS",            "SQ_STRING",     "DOUBLE",      
  "TRUE",          "FALSE",         "NUL",           "LCB",         
  "RCB",           "DOT_STAR",      "COLON",         "LSB",         
  "RSB",           "DOLLAR",        "DOT",           "error",       
  "commands",      "command",       "stream_cmd",    "filter_cmd",  
  "transform_cmd",  "sliding_count_window_cmd",  "group_cmd",     "reduce_cmd",  
  "group_expr",    "group_exprs",   "expr",          "pri_expr",    
  "exprs",         "path_expr",     "func_name",     "func_expr",   
  "project_name",  "project_names",  "main",          "epn",         
  "stmt",          "record",        "fields",        "field",       
  "field_name",  
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
 /*  16 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA SHUFFLE",
 /*  17 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA ALL",
 /*  18 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA expr",
 /*  19 */ "stream_cmd ::= OSTREAM DQ_STRING",
 /*  20 */ "filter_cmd ::= FILTER expr",
 /*  21 */ "transform_cmd ::= TRANSFORM expr",
 /*  22 */ "sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT RP",
 /*  23 */ "sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT SLIDE INT RP",
 /*  24 */ "group_cmd ::= GROUP INTO expr",
 /*  25 */ "group_cmd ::= GROUP BY group_exprs INTO expr",
 /*  26 */ "reduce_cmd ::= REDUCE INTO expr",
 /*  27 */ "reduce_cmd ::= REDUCE BY group_exprs INTO expr",
 /*  28 */ "group_expr ::= expr",
 /*  29 */ "group_expr ::= expr AS ID",
 /*  30 */ "group_exprs ::= group_expr",
 /*  31 */ "group_exprs ::= group_exprs COMMA group_expr",
 /*  32 */ "expr ::= pri_expr",
 /*  33 */ "pri_expr ::= LP pri_expr RP",
 /*  34 */ "pri_expr ::= DQ_STRING",
 /*  35 */ "pri_expr ::= SQ_STRING",
 /*  36 */ "pri_expr ::= INT",
 /*  37 */ "pri_expr ::= DOUBLE",
 /*  38 */ "pri_expr ::= TRUE",
 /*  39 */ "pri_expr ::= FALSE",
 /*  40 */ "pri_expr ::= NUL",
 /*  41 */ "pri_expr ::= expr ADD expr",
 /*  42 */ "pri_expr ::= expr SUB expr",
 /*  43 */ "pri_expr ::= expr MUL expr",
 /*  44 */ "pri_expr ::= expr DIV expr",
 /*  45 */ "pri_expr ::= expr MOD expr",
 /*  46 */ "pri_expr ::= expr LT expr",
 /*  47 */ "pri_expr ::= expr GT expr",
 /*  48 */ "pri_expr ::= expr LE expr",
 /*  49 */ "pri_expr ::= expr GE expr",
 /*  50 */ "pri_expr ::= expr EQ expr",
 /*  51 */ "pri_expr ::= expr NE expr",
 /*  52 */ "pri_expr ::= expr OR expr",
 /*  53 */ "pri_expr ::= expr AND expr",
 /*  54 */ "pri_expr ::= NOT expr",
 /*  55 */ "pri_expr ::= expr IN expr",
 /*  56 */ "expr ::= record",
 /*  57 */ "record ::= LCB RCB",
 /*  58 */ "record ::= LCB fields RCB",
 /*  59 */ "fields ::= field",
 /*  60 */ "fields ::= fields COMMA field",
 /*  61 */ "field ::= field_name expr",
 /*  62 */ "field ::= path_expr DOT_STAR",
 /*  63 */ "field_name ::= ID COLON",
 /*  64 */ "expr ::= LSB RSB",
 /*  65 */ "expr ::= LSB exprs RSB",
 /*  66 */ "exprs ::= expr",
 /*  67 */ "exprs ::= exprs COMMA expr",
 /*  68 */ "expr ::= path_expr",
 /*  69 */ "path_expr ::= LP path_expr RP",
 /*  70 */ "path_expr ::= ID",
 /*  71 */ "path_expr ::= DOLLAR",
 /*  72 */ "path_expr ::= path_expr project_name",
 /*  73 */ "project_name ::= DOT ID",
 /*  74 */ "path_expr ::= path_expr LCB project_names RCB",
 /*  75 */ "path_expr ::= path_expr LCB MUL SUB project_names RCB",
 /*  76 */ "project_names ::= project_name",
 /*  77 */ "project_names ::= project_names COMMA project_name",
 /*  78 */ "path_expr ::= path_expr LSB expr RSB",
 /*  79 */ "path_expr ::= path_expr LSB expr COLON expr RSB",
 /*  80 */ "expr ::= func_expr",
 /*  81 */ "func_name ::= ID LP",
 /*  82 */ "func_expr ::= func_name RP",
 /*  83 */ "func_expr ::= func_name exprs RP",
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
  { 70, 1 },
  { 71, 2 },
  { 71, 3 },
  { 72, 1 },
  { 72, 3 },
  { 72, 3 },
  { 72, 5 },
  { 52, 1 },
  { 52, 3 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 54, 2 },
  { 54, 4 },
  { 54, 4 },
  { 54, 4 },
  { 54, 2 },
  { 55, 2 },
  { 56, 2 },
  { 57, 5 },
  { 57, 7 },
  { 58, 3 },
  { 58, 5 },
  { 59, 3 },
  { 59, 5 },
  { 60, 1 },
  { 60, 3 },
  { 61, 1 },
  { 61, 3 },
  { 62, 1 },
  { 63, 3 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 1 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 63, 2 },
  { 63, 3 },
  { 62, 1 },
  { 73, 2 },
  { 73, 3 },
  { 74, 1 },
  { 74, 3 },
  { 75, 2 },
  { 75, 2 },
  { 76, 2 },
  { 62, 2 },
  { 62, 3 },
  { 64, 1 },
  { 64, 3 },
  { 62, 1 },
  { 65, 3 },
  { 65, 1 },
  { 65, 1 },
  { 65, 2 },
  { 68, 2 },
  { 65, 4 },
  { 65, 6 },
  { 69, 1 },
  { 69, 3 },
  { 65, 4 },
  { 65, 6 },
  { 62, 1 },
  { 66, 2 },
  { 67, 2 },
  { 67, 3 },
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
      printCommands(yymsp[0].minor.yy115);  
      // g_pCommands = yymsp[0].minor.yy115;    

	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy115
	  ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == bPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  bPoint->addTargetPipeline(yymsp[0].minor.yy115);      
   }
#line 1034 "speparser.c"
        break;
      case 5: /* stmt ::= ID EQ commands */
#line 81 "speparser.y"
{
	  std::cout << "sth like 'c1 = operator->...' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy115); 
	  // g_pCommands = yymsp[0].minor.yy115;  

	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-2].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' already exists." << std::endl;		 
	  }	  
   }
#line 1049 "speparser.c"
        break;
      case 6: /* stmt ::= ID EQ ID PIPE commands */
#line 93 "speparser.y"
{
	  std::cout << "sth like 'c2 = c1->operator' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy115);
	  // g_pCommands = yymsp[0].minor.yy115;  
	  
	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-4].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-4].minor.yy0.z << "' already exists." << std::endl;		 
	  }
	  
	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy115
	  ConnectPointOperator* cPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == cPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  cPoint->addTargetPipeline(yymsp[0].minor.yy115);
   }
#line 1072 "speparser.c"
        break;
      case 7: /* commands ::= command */
#line 113 "speparser.y"
{
      yygotominor.yy115 = new vector<BaseOperator*>();
      yygotominor.yy115->push_back(yymsp[0].minor.yy144);
      
      // set current pipeline, add it into all pipelines
      pParseContext->curPipeline = yygotominor.yy115;
      pParseContext->allPipelines.push_back(yygotominor.yy115);
}
#line 1084 "speparser.c"
        break;
      case 8: /* commands ::= commands PIPE command */
#line 122 "speparser.y"
{
      yygotominor.yy115 = yymsp[-2].minor.yy115;
      yygotominor.yy115->push_back(yymsp[0].minor.yy144);
}
#line 1092 "speparser.c"
        break;
      case 9: /* command ::= stream_cmd */
      case 10: /* command ::= filter_cmd */ yytestcase(yyruleno==10);
      case 11: /* command ::= transform_cmd */ yytestcase(yyruleno==11);
      case 12: /* command ::= sliding_count_window_cmd */ yytestcase(yyruleno==12);
      case 13: /* command ::= group_cmd */ yytestcase(yyruleno==13);
      case 14: /* command ::= reduce_cmd */ yytestcase(yyruleno==14);
#line 127 "speparser.y"
{
      yygotominor.yy144 = yymsp[0].minor.yy144;
}
#line 1104 "speparser.c"
        break;
      case 15: /* stream_cmd ::= SSTREAM DQ_STRING */
#line 149 "speparser.y"
{
      yygotominor.yy144 = new StreamOperator(yymsp[0].minor.yy0.z, 0);
}
#line 1111 "speparser.c"
        break;
      case 16: /* stream_cmd ::= SSTREAM DQ_STRING COMMA SHUFFLE */
#line 153 "speparser.y"
{
      yygotominor.yy144 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionRoundrobin());
}
#line 1118 "speparser.c"
        break;
      case 17: /* stream_cmd ::= SSTREAM DQ_STRING COMMA ALL */
#line 157 "speparser.y"
{
      yygotominor.yy144 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionAll());
}
#line 1125 "speparser.c"
        break;
      case 18: /* stream_cmd ::= SSTREAM DQ_STRING COMMA expr */
#line 161 "speparser.y"
{
      yygotominor.yy144 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionPerValue(yymsp[0].minor.yy132));
}
#line 1132 "speparser.c"
        break;
      case 19: /* stream_cmd ::= OSTREAM DQ_STRING */
#line 165 "speparser.y"
{
      yygotominor.yy144 = new StreamOperator(yymsp[0].minor.yy0.z, 1);
}
#line 1139 "speparser.c"
        break;
      case 20: /* filter_cmd ::= FILTER expr */
#line 169 "speparser.y"
{
      yygotominor.yy144 = new FilterOperator(yymsp[0].minor.yy132);
}
#line 1146 "speparser.c"
        break;
      case 21: /* transform_cmd ::= TRANSFORM expr */
#line 173 "speparser.y"
{
	  yygotominor.yy144 = new TransformOperator(yymsp[0].minor.yy132);
}
#line 1153 "speparser.c"
        break;
      case 22: /* sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT RP */
#line 177 "speparser.y"
{
	yygotominor.yy144 = new SlidingCountWindowOperator(atoi(yymsp[-1].minor.yy0.z), 1);
}
#line 1160 "speparser.c"
        break;
      case 23: /* sliding_count_window_cmd ::= WINDOW_KW LP ROWS INT SLIDE INT RP */
#line 181 "speparser.y"
{
	yygotominor.yy144 = new SlidingCountWindowOperator(atoi(yymsp[-3].minor.yy0.z), atoi(yymsp[-1].minor.yy0.z));
}
#line 1167 "speparser.c"
        break;
      case 24: /* group_cmd ::= GROUP INTO expr */
#line 185 "speparser.y"
{
    yygotominor.yy144 = new GroupOperator(NULL, yymsp[0].minor.yy132);
}
#line 1174 "speparser.c"
        break;
      case 25: /* group_cmd ::= GROUP BY group_exprs INTO expr */
#line 189 "speparser.y"
{
    yygotominor.yy144 = new GroupOperator(yymsp[-2].minor.yy20, yymsp[0].minor.yy132);
}
#line 1181 "speparser.c"
        break;
      case 26: /* reduce_cmd ::= REDUCE INTO expr */
#line 193 "speparser.y"
{
    yygotominor.yy144 = new ReduceOperator(NULL, yymsp[0].minor.yy132);
}
#line 1188 "speparser.c"
        break;
      case 27: /* reduce_cmd ::= REDUCE BY group_exprs INTO expr */
#line 197 "speparser.y"
{
    yygotominor.yy144 = new ReduceOperator(yymsp[-2].minor.yy20, yymsp[0].minor.yy132);
}
#line 1195 "speparser.c"
        break;
      case 28: /* group_expr ::= expr */
#line 201 "speparser.y"
{
    yygotominor.yy133 = new GroupExpr(yymsp[0].minor.yy132, "");
}
#line 1202 "speparser.c"
        break;
      case 29: /* group_expr ::= expr AS ID */
#line 205 "speparser.y"
{
    yygotominor.yy133 = new GroupExpr(yymsp[-2].minor.yy132, yymsp[0].minor.yy0.z);
}
#line 1209 "speparser.c"
        break;
      case 30: /* group_exprs ::= group_expr */
#line 209 "speparser.y"
{
    yygotominor.yy20 = new vector<GroupExpr*>();
    yygotominor.yy20->push_back(yymsp[0].minor.yy133);
}
#line 1217 "speparser.c"
        break;
      case 31: /* group_exprs ::= group_exprs COMMA group_expr */
#line 214 "speparser.y"
{
    yymsp[-2].minor.yy20->push_back(yymsp[0].minor.yy133);
    yygotominor.yy20 = yymsp[-2].minor.yy20;
}
#line 1225 "speparser.c"
        break;
      case 32: /* expr ::= pri_expr */
      case 80: /* expr ::= func_expr */ yytestcase(yyruleno==80);
#line 219 "speparser.y"
{
	yygotominor.yy132 = yymsp[0].minor.yy132;
}
#line 1233 "speparser.c"
        break;
      case 33: /* pri_expr ::= LP pri_expr RP */
#line 223 "speparser.y"
{
      yygotominor.yy132 = yymsp[-1].minor.yy132;
}
#line 1240 "speparser.c"
        break;
      case 34: /* pri_expr ::= DQ_STRING */
      case 35: /* pri_expr ::= SQ_STRING */ yytestcase(yyruleno==35);
#line 227 "speparser.y"
{
      // strip off the quote char
      std::string s(yymsp[0].minor.yy0.z+1, strlen(yymsp[0].minor.yy0.z)-2);
      yygotominor.yy132 = new StringValue(s.c_str());
}
#line 1250 "speparser.c"
        break;
      case 36: /* pri_expr ::= INT */
#line 239 "speparser.y"
{
      yygotominor.yy132 = new IntValue(atoi(yymsp[0].minor.yy0.z));
}
#line 1257 "speparser.c"
        break;
      case 37: /* pri_expr ::= DOUBLE */
#line 243 "speparser.y"
{
      yygotominor.yy132 = new DoubleValue(atof(yymsp[0].minor.yy0.z));
}
#line 1264 "speparser.c"
        break;
      case 38: /* pri_expr ::= TRUE */
#line 247 "speparser.y"
{
      yygotominor.yy132 = new BoolValue(true);
}
#line 1271 "speparser.c"
        break;
      case 39: /* pri_expr ::= FALSE */
#line 251 "speparser.y"
{
      yygotominor.yy132 = new BoolValue(false);
}
#line 1278 "speparser.c"
        break;
      case 40: /* pri_expr ::= NUL */
#line 254 "speparser.y"
{
      yygotominor.yy132 = new NullValue();
}
#line 1285 "speparser.c"
        break;
      case 41: /* pri_expr ::= expr ADD expr */
#line 258 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_ADD, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1292 "speparser.c"
        break;
      case 42: /* pri_expr ::= expr SUB expr */
#line 262 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_SUB, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1299 "speparser.c"
        break;
      case 43: /* pri_expr ::= expr MUL expr */
#line 266 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_MUL, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1306 "speparser.c"
        break;
      case 44: /* pri_expr ::= expr DIV expr */
#line 269 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_DIV, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1313 "speparser.c"
        break;
      case 45: /* pri_expr ::= expr MOD expr */
#line 273 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_MOD, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1320 "speparser.c"
        break;
      case 46: /* pri_expr ::= expr LT expr */
#line 277 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_LT, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1327 "speparser.c"
        break;
      case 47: /* pri_expr ::= expr GT expr */
#line 281 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_GT, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1334 "speparser.c"
        break;
      case 48: /* pri_expr ::= expr LE expr */
#line 285 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_LE, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1341 "speparser.c"
        break;
      case 49: /* pri_expr ::= expr GE expr */
#line 289 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_GE, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1348 "speparser.c"
        break;
      case 50: /* pri_expr ::= expr EQ expr */
#line 293 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_EQ, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1355 "speparser.c"
        break;
      case 51: /* pri_expr ::= expr NE expr */
#line 297 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_NE, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1362 "speparser.c"
        break;
      case 52: /* pri_expr ::= expr OR expr */
#line 301 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_OR, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1369 "speparser.c"
        break;
      case 53: /* pri_expr ::= expr AND expr */
#line 305 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_AND, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1376 "speparser.c"
        break;
      case 54: /* pri_expr ::= NOT expr */
#line 309 "speparser.y"
{
      yygotominor.yy132 = new UnaryExpr(Expr::OP_NOT, yymsp[0].minor.yy132);
}
#line 1383 "speparser.c"
        break;
      case 55: /* pri_expr ::= expr IN expr */
#line 313 "speparser.y"
{
      yygotominor.yy132 = new BinaryExpr(Expr::OP_IN, yymsp[-2].minor.yy132, yymsp[0].minor.yy132);
}
#line 1390 "speparser.c"
        break;
      case 56: /* expr ::= record */
#line 322 "speparser.y"
{
	yygotominor.yy132 = yymsp[0].minor.yy104;
}
#line 1397 "speparser.c"
        break;
      case 57: /* record ::= LCB RCB */
#line 326 "speparser.y"
{
	yygotominor.yy104 = new RecordConstructExpr( new vector<RecordMemberExpr*>());
}
#line 1404 "speparser.c"
        break;
      case 58: /* record ::= LCB fields RCB */
#line 330 "speparser.y"
{
	yygotominor.yy104 = new RecordConstructExpr(yymsp[-1].minor.yy124);
}
#line 1411 "speparser.c"
        break;
      case 59: /* fields ::= field */
#line 334 "speparser.y"
{
	yygotominor.yy124 = new vector<RecordMemberExpr*>();
	yygotominor.yy124->push_back(yymsp[0].minor.yy1);
}
#line 1419 "speparser.c"
        break;
      case 60: /* fields ::= fields COMMA field */
#line 339 "speparser.y"
{
	yymsp[-2].minor.yy124->push_back(yymsp[0].minor.yy1);
	yygotominor.yy124 = yymsp[-2].minor.yy124;
}
#line 1427 "speparser.c"
        break;
      case 61: /* field ::= field_name expr */
#line 344 "speparser.y"
{
	yygotominor.yy1 = new RecordMemberExpr(yymsp[-1].minor.yy82->c_str(), yymsp[0].minor.yy132);
}
#line 1434 "speparser.c"
        break;
      case 62: /* field ::= path_expr DOT_STAR */
#line 348 "speparser.y"
{
	yygotominor.yy1 = new RecordMemberExpr("*", yymsp[-1].minor.yy55);
}
#line 1441 "speparser.c"
        break;
      case 63: /* field_name ::= ID COLON */
#line 352 "speparser.y"
{
	yygotominor.yy82 = new string(yymsp[-1].minor.yy0.z);
}
#line 1448 "speparser.c"
        break;
      case 64: /* expr ::= LSB RSB */
#line 356 "speparser.y"
{
    yygotominor.yy132 = new ArrayConstructExpr();
}
#line 1455 "speparser.c"
        break;
      case 65: /* expr ::= LSB exprs RSB */
#line 360 "speparser.y"
{
	yygotominor.yy132 = new ArrayConstructExpr(yymsp[-1].minor.yy5);
}
#line 1462 "speparser.c"
        break;
      case 66: /* exprs ::= expr */
#line 364 "speparser.y"
{
	yygotominor.yy5 = new vector<Expr*>();
	yygotominor.yy5->push_back(yymsp[0].minor.yy132);
}
#line 1470 "speparser.c"
        break;
      case 67: /* exprs ::= exprs COMMA expr */
#line 369 "speparser.y"
{
	yygotominor.yy5 = yymsp[-2].minor.yy5;
	yygotominor.yy5->push_back(yymsp[0].minor.yy132);
}
#line 1478 "speparser.c"
        break;
      case 68: /* expr ::= path_expr */
#line 374 "speparser.y"
{
      yygotominor.yy132 = yymsp[0].minor.yy55;
}
#line 1485 "speparser.c"
        break;
      case 69: /* path_expr ::= LP path_expr RP */
#line 378 "speparser.y"
{
	  yygotominor.yy55 = yymsp[-1].minor.yy55;
}
#line 1492 "speparser.c"
        break;
      case 70: /* path_expr ::= ID */
#line 382 "speparser.y"
{
      yygotominor.yy55 = new VarRefExpr(yymsp[0].minor.yy0.z);
}
#line 1499 "speparser.c"
        break;
      case 71: /* path_expr ::= DOLLAR */
#line 385 "speparser.y"
{
      yygotominor.yy55 = new VarRefExpr("$");
}
#line 1506 "speparser.c"
        break;
      case 72: /* path_expr ::= path_expr project_name */
#line 389 "speparser.y"
{
      yymsp[-1].minor.yy55->addChildPathExpr(new StepExpr(yymsp[0].minor.yy82->c_str()) );
      yygotominor.yy55 = yymsp[-1].minor.yy55;
      delete yymsp[0].minor.yy82;
}
#line 1515 "speparser.c"
        break;
      case 73: /* project_name ::= DOT ID */
#line 395 "speparser.y"
{
      yygotominor.yy82 = new string(yymsp[0].minor.yy0.z);
}
#line 1522 "speparser.c"
        break;
      case 74: /* path_expr ::= path_expr LCB project_names RCB */
#line 399 "speparser.y"
{
	  yymsp[-3].minor.yy55->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy59, false));
	  yygotominor.yy55 = yymsp[-3].minor.yy55;
}
#line 1530 "speparser.c"
        break;
      case 75: /* path_expr ::= path_expr LCB MUL SUB project_names RCB */
#line 404 "speparser.y"
{
	  yymsp[-5].minor.yy55->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy59, true));
	  yygotominor.yy55 = yymsp[-5].minor.yy55;
}
#line 1538 "speparser.c"
        break;
      case 76: /* project_names ::= project_name */
#line 409 "speparser.y"
{
      yygotominor.yy59 = new vector<string>();
      yygotominor.yy59->push_back(string(yymsp[0].minor.yy82->c_str()));
      delete yymsp[0].minor.yy82; 
}
#line 1547 "speparser.c"
        break;
      case 77: /* project_names ::= project_names COMMA project_name */
#line 414 "speparser.y"
{
      yymsp[-2].minor.yy59->push_back(string(yymsp[0].minor.yy82->c_str()));
      yygotominor.yy59 = yymsp[-2].minor.yy59;
      delete yymsp[0].minor.yy82;
}
#line 1556 "speparser.c"
        break;
      case 78: /* path_expr ::= path_expr LSB expr RSB */
#line 420 "speparser.y"
{
	yymsp[-3].minor.yy55->addChildPathExpr(new ArrayElementsExpr(yymsp[-1].minor.yy132,NULL));
	yygotominor.yy55 = yymsp[-3].minor.yy55;	  
}
#line 1564 "speparser.c"
        break;
      case 79: /* path_expr ::= path_expr LSB expr COLON expr RSB */
#line 425 "speparser.y"
{  
	yymsp[-5].minor.yy55->addChildPathExpr(new ArrayElementsExpr(yymsp[-3].minor.yy132,yymsp[-1].minor.yy132));
	yygotominor.yy55 = yymsp[-5].minor.yy55;
}
#line 1572 "speparser.c"
        break;
      case 81: /* func_name ::= ID LP */
#line 435 "speparser.y"
{
	yygotominor.yy82 = new string(yymsp[-1].minor.yy0.z);
	printf("recongnize a function %s.\n ", yygotominor.yy82->c_str());
}
#line 1580 "speparser.c"
        break;
      case 82: /* func_expr ::= func_name RP */
#line 440 "speparser.y"
{
    vector<Expr*>* args = new vector<Expr*>();
	yygotominor.yy132 = createBuiltinFunction(yymsp[-1].minor.yy82->c_str(), args);
	if (!yygotominor.yy132) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-1].minor.yy82->c_str());
	}
	delete yymsp[-1].minor.yy82;
}
#line 1593 "speparser.c"
        break;
      case 83: /* func_expr ::= func_name exprs RP */
#line 450 "speparser.y"
{
	yygotominor.yy132 = createBuiltinFunction(yymsp[-2].minor.yy82->c_str(), yymsp[-1].minor.yy5);
	if (!yygotominor.yy132) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-2].minor.yy82->c_str());
	}
	delete yymsp[-2].minor.yy82;
}
#line 1605 "speparser.c"
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
#line 1676 "speparser.c"
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
