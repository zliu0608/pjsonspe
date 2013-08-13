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
#define YYNOCODE 84
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE Token
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  GroupExpr* yy3;
  Expr* yy10;
  RecordConstructExpr* yy30;
  string* yy36;
  long64 yy43;
  vector<RecordMemberExpr*>* yy78;
  BaseOperator* yy80;
  vector<Expr*>* yy101;
  vector<GroupExpr*>* yy104;
  vector<string>* yy115;
  RecordMemberExpr* yy129;
  vector<BaseOperator*> * yy137;
  PathExpr* yy149;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL ParseContext* pParseContext;
#define ParseARG_PDECL ,ParseContext* pParseContext
#define ParseARG_FETCH ParseContext* pParseContext = yypParser->pParseContext
#define ParseARG_STORE yypParser->pParseContext = pParseContext
#define YYNSTATE 175
#define YYNRULE 94
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
#define YY_ACTTAB_COUNT (666)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    25,   24,   23,   31,   30,   29,   28,   27,   26,   37,
 /*    10 */    35,   34,   33,   32,   25,   24,   23,   31,   30,   29,
 /*    20 */    28,   27,   26,   37,   35,   34,   33,   32,   25,   24,
 /*    30 */    23,   31,   30,   29,   28,   27,   26,   37,   35,   34,
 /*    40 */    33,   32,  115,  116,  117,   22,   75,  102,  118,   18,
 /*    50 */   161,  138,  165,  164,  123,  122,  121,  120,   36,  159,
 /*    60 */    37,   35,   34,   33,   32,  137,  125,   98,   96,   44,
 /*    70 */    15,    6,  160,  158,  157,  156,  155,   38,   90,  127,
 /*    80 */    21,    9,   10,  135,  174,   25,   24,   23,   31,   30,
 /*    90 */    29,   28,   27,   26,   37,   35,   34,   33,   32,   14,
 /*   100 */    39,   22,  106,  102,   85,    7,  161,  123,  122,  121,
 /*   110 */   120,   34,   33,   32,   36,  159,   95,  111,  172,  171,
 /*   120 */   170,  169,  168,   20,  167,  166,  175,  148,  160,  158,
 /*   130 */   157,  156,  155,   38,   11,   13,    5,    9,  146,  135,
 /*   140 */   162,   49,   49,   79,   22,  107,  102,  139,   87,  161,
 /*   150 */   100,   17,   16,   99,  145,   20,  143,   36,  159,   75,
 /*   160 */   132,  119,   47,   83,  136,  133,   81,   44,  142,  140,
 /*   170 */   128,  160,  158,  157,  156,  155,   38,  147,   21,   41,
 /*   180 */     9,   45,  135,   24,   23,   31,   30,   29,   28,   27,
 /*   190 */    26,   37,   35,   34,   33,   32,  101,   40,   22,  131,
 /*   200 */   102,   19,  101,  161,  104,  130,  135,   45,  126,   97,
 /*   210 */    48,   36,  159,   45,   10,  108,  111,  172,  171,  170,
 /*   220 */   169,  168,  124,  167,  166,  160,  158,  157,  156,  155,
 /*   230 */    38,   12,  135,  149,    9,  114,  135,  109,  135,    3,
 /*   240 */   108,  111,  172,  171,  170,  169,  168,    2,  167,  166,
 /*   250 */    92,  111,  172,  171,  170,  169,  168,    4,  167,  166,
 /*   260 */   270,    1,   91,  139,   88,   31,   30,   29,   28,   27,
 /*   270 */    26,   37,   35,   34,   33,   32,   94,  111,  172,  171,
 /*   280 */   170,  169,  168,  103,  167,  166,  173,  172,  171,  170,
 /*   290 */   169,  168,  110,  167,  166,  104,  112,   82,   51,  163,
 /*   300 */    40,   46,    8,  134,   19,  141,   76,  104,   67,  150,
 /*   310 */   112,   80,   51,  163,  129,   46,    8,  134,   84,   93,
 /*   320 */    74,  107,  144,  150,  271,  104,  100,   17,   16,   99,
 /*   330 */    40,   43,  271,  271,   19,  271,  271,  104,  271,   83,
 /*   340 */   271,   79,   81,  107,  271,  271,  271,  271,  100,   17,
 /*   350 */    16,   99,  113,  271,   51,  163,  271,   46,    8,  134,
 /*   360 */   271,   83,  271,  271,   81,  150,   60,  163,   89,   46,
 /*   370 */     8,  134,  271,  271,  271,  271,  271,  150,   60,  163,
 /*   380 */    86,   46,    8,  134,  271,  107,  271,  271,  271,  150,
 /*   390 */   100,   17,   16,   99,  271,  271,   77,  163,  271,   46,
 /*   400 */     8,  134,  271,   83,  271,  271,   81,  150,   62,  105,
 /*   410 */   271,   42,    8,  134,  271,  271,  271,   78,  163,  150,
 /*   420 */    46,    8,  134,  271,  271,  271,  154,  163,  150,   46,
 /*   430 */     8,  134,  271,  271,  271,  153,  163,  150,   46,    8,
 /*   440 */   134,  271,  271,  271,  271,  271,  150,  152,  163,  271,
 /*   450 */    46,    8,  134,  271,  271,  271,  271,  271,  150,   73,
 /*   460 */   163,  271,   46,    8,  134,  271,  271,  271,   72,  163,
 /*   470 */   150,   46,    8,  134,  271,  271,  271,   71,  163,  150,
 /*   480 */    46,    8,  134,  271,  271,  271,   70,  163,  150,   46,
 /*   490 */     8,  134,  271,  271,  271,   69,  163,  150,   46,    8,
 /*   500 */   134,  271,  271,  271,   68,  163,  150,   46,    8,  134,
 /*   510 */   271,  271,  271,  271,  271,  150,   64,  163,  271,   46,
 /*   520 */     8,  134,  271,  271,  271,  271,  271,  150,   66,  163,
 /*   530 */   271,   46,    8,  134,  271,  271,  271,   65,  163,  150,
 /*   540 */    46,    8,  134,  271,  271,  271,  151,  163,  150,   46,
 /*   550 */     8,  134,  271,  271,  271,   61,  163,  150,   46,    8,
 /*   560 */   134,  271,  271,  271,   59,  163,  150,   46,    8,  134,
 /*   570 */   271,  271,  271,   50,  163,  150,   46,    8,  134,  271,
 /*   580 */   271,  271,  271,  271,  150,   52,  163,  271,   46,    8,
 /*   590 */   134,  271,  271,  271,  271,  271,  150,   58,  163,  271,
 /*   600 */    46,    8,  134,  271,  271,  271,   57,  163,  150,   46,
 /*   610 */     8,  134,  271,  271,  271,   56,  163,  150,   46,    8,
 /*   620 */   134,  271,  271,  271,   55,  163,  150,   46,    8,  134,
 /*   630 */   271,  271,  271,   54,  163,  150,   46,    8,  134,  271,
 /*   640 */   271,  271,   53,  163,  150,   46,    8,  134,  271,  271,
 /*   650 */   271,  271,  271,  150,   63,  163,  271,   46,    8,  134,
 /*   660 */   271,  271,  271,  271,  271,  150,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,    1,    2,    3,    4,    5,    6,
 /*    20 */     7,    8,    9,   10,   11,   12,   13,   14,    1,    2,
 /*    30 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    40 */    13,   14,   33,   34,   35,   15,   29,   17,   31,   50,
 /*    50 */    20,   52,   22,   23,   33,   34,   35,   36,   28,   29,
 /*    60 */    10,   11,   12,   13,   14,   52,   31,   32,   41,   71,
 /*    70 */    38,   39,   42,   43,   44,   45,   46,   47,   80,   81,
 /*    80 */    82,   51,   21,   53,   16,    1,    2,    3,    4,    5,
 /*    90 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   38,
 /*   100 */    21,   15,   20,   17,   30,   18,   20,   33,   34,   35,
 /*   110 */    36,   12,   13,   14,   28,   29,   56,   57,   58,   59,
 /*   120 */    60,   61,   62,   21,   64,   65,    0,   48,   42,   43,
 /*   130 */    44,   45,   46,   47,   21,   38,   39,   51,   52,   53,
 /*   140 */    31,   21,   21,   17,   15,   19,   17,   74,   75,   20,
 /*   150 */    24,   25,   26,   27,   52,   21,   17,   28,   29,   29,
 /*   160 */    31,   31,   32,   37,   28,   31,   40,   71,   48,   48,
 /*   170 */    17,   42,   43,   44,   45,   46,   47,   81,   82,   11,
 /*   180 */    51,   28,   53,    2,    3,    4,    5,    6,    7,    8,
 /*   190 */     9,   10,   11,   12,   13,   14,   17,   47,   15,   49,
 /*   200 */    17,   51,   17,   20,   54,   50,   53,   28,   20,   29,
 /*   210 */    28,   28,   29,   28,   21,   56,   57,   58,   59,   60,
 /*   220 */    61,   62,   31,   64,   65,   42,   43,   44,   45,   46,
 /*   230 */    47,   38,   53,   48,   51,   17,   53,   78,   53,   18,
 /*   240 */    56,   57,   58,   59,   60,   61,   62,    8,   64,   65,
 /*   250 */    56,   57,   58,   59,   60,   61,   62,   18,   64,   65,
 /*   260 */    76,   77,   78,   74,   75,    4,    5,    6,    7,    8,
 /*   270 */     9,   10,   11,   12,   13,   14,   56,   57,   58,   59,
 /*   280 */    60,   61,   62,   12,   64,   65,   57,   58,   59,   60,
 /*   290 */    61,   62,   16,   64,   65,   54,   66,   67,   68,   69,
 /*   300 */    47,   71,   72,   73,   51,   74,   63,   54,   29,   79,
 /*   310 */    66,   67,   68,   69,   31,   71,   72,   73,   63,   17,
 /*   320 */    29,   19,   74,   79,   83,   54,   24,   25,   26,   27,
 /*   330 */    47,   71,   83,   83,   51,   83,   83,   54,   83,   37,
 /*   340 */    83,   17,   40,   19,   83,   83,   83,   83,   24,   25,
 /*   350 */    26,   27,   66,   83,   68,   69,   83,   71,   72,   73,
 /*   360 */    83,   37,   83,   83,   40,   79,   68,   69,   70,   71,
 /*   370 */    72,   73,   83,   83,   83,   83,   83,   79,   68,   69,
 /*   380 */    70,   71,   72,   73,   83,   19,   83,   83,   83,   79,
 /*   390 */    24,   25,   26,   27,   83,   83,   68,   69,   83,   71,
 /*   400 */    72,   73,   83,   37,   83,   83,   40,   79,   68,   69,
 /*   410 */    83,   71,   72,   73,   83,   83,   83,   68,   69,   79,
 /*   420 */    71,   72,   73,   83,   83,   83,   68,   69,   79,   71,
 /*   430 */    72,   73,   83,   83,   83,   68,   69,   79,   71,   72,
 /*   440 */    73,   83,   83,   83,   83,   83,   79,   68,   69,   83,
 /*   450 */    71,   72,   73,   83,   83,   83,   83,   83,   79,   68,
 /*   460 */    69,   83,   71,   72,   73,   83,   83,   83,   68,   69,
 /*   470 */    79,   71,   72,   73,   83,   83,   83,   68,   69,   79,
 /*   480 */    71,   72,   73,   83,   83,   83,   68,   69,   79,   71,
 /*   490 */    72,   73,   83,   83,   83,   68,   69,   79,   71,   72,
 /*   500 */    73,   83,   83,   83,   68,   69,   79,   71,   72,   73,
 /*   510 */    83,   83,   83,   83,   83,   79,   68,   69,   83,   71,
 /*   520 */    72,   73,   83,   83,   83,   83,   83,   79,   68,   69,
 /*   530 */    83,   71,   72,   73,   83,   83,   83,   68,   69,   79,
 /*   540 */    71,   72,   73,   83,   83,   83,   68,   69,   79,   71,
 /*   550 */    72,   73,   83,   83,   83,   68,   69,   79,   71,   72,
 /*   560 */    73,   83,   83,   83,   68,   69,   79,   71,   72,   73,
 /*   570 */    83,   83,   83,   68,   69,   79,   71,   72,   73,   83,
 /*   580 */    83,   83,   83,   83,   79,   68,   69,   83,   71,   72,
 /*   590 */    73,   83,   83,   83,   83,   83,   79,   68,   69,   83,
 /*   600 */    71,   72,   73,   83,   83,   83,   68,   69,   79,   71,
 /*   610 */    72,   73,   83,   83,   83,   68,   69,   79,   71,   72,
 /*   620 */    73,   83,   83,   83,   68,   69,   79,   71,   72,   73,
 /*   630 */    83,   83,   83,   68,   69,   79,   71,   72,   73,   83,
 /*   640 */    83,   83,   68,   69,   79,   71,   72,   73,   83,   83,
 /*   650 */    83,   83,   83,   79,   68,   69,   83,   71,   72,   73,
 /*   660 */    83,   83,   83,   83,   83,   79,
};
#define YY_SHIFT_USE_DFLT (-2)
#define YY_SHIFT_COUNT (109)
#define YY_SHIFT_MIN   (-1)
#define YY_SHIFT_MAX   (366)
static const short yy_shift_ofst[] = {
 /*     0 */   324,  126,  302,  366,  366,  183,  183,  366,  129,   86,
 /*    10 */   183,   30,  183,  183,  183,  183,  183,  183,  183,  183,
 /*    20 */   183,  183,  183,  183,  183,  183,  183,  183,  183,  183,
 /*    30 */   183,  183,  183,  183,  183,  183,  183,  183,  185,  179,
 /*    40 */   271,  241,  283,  283,  150,  153,  253,  291,  279,  241,
 /*    50 */    -1,   27,   13,   84,   84,   84,   84,   84,   84,   84,
 /*    60 */    84,   84,   84,   84,  181,  261,  261,   74,   50,   50,
 /*    70 */    50,   50,   50,   50,   21,    9,  130,   99,   99,  239,
 /*    80 */   193,   97,   61,   32,   17,   35,  134,  121,  120,  102,
 /*    90 */    79,  276,   87,  221,   87,   87,  218,  191,  180,  182,
 /*   100 */   188,  155,  136,  168,  139,  109,  113,   82,   87,   68,
};
#define YY_REDUCE_USE_DFLT (-3)
#define YY_REDUCE_COUNT (49)
#define YY_REDUCE_MIN   (-2)
#define YY_REDUCE_MAX   (586)
static const short yy_reduce_ofst[] = {
 /*     0 */   184,  159,  220,  194,   60,  244,  230,  229,  310,  298,
 /*    10 */   286,  586,  574,  565,  556,  547,  538,  529,  517,  505,
 /*    20 */   496,  487,  478,  469,  460,  448,  436,  427,  418,  409,
 /*    30 */   400,  391,  379,  367,  358,  349,  340,  328,   -2,   96,
 /*    40 */   189,   73,  248,  248,  248,  260,  248,  255,  243,  231,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*    10 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*    20 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*    30 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*    40 */   269,  269,  253,  269,  269,  269,  253,  269,  269,  269,
 /*    50 */   269,  213,  269,  212,  211,  210,  209,  197,  196,  252,
 /*    60 */   251,  246,  269,  194,  237,  240,  238,  269,  236,  235,
 /*    70 */   234,  233,  232,  231,  269,  269,  269,  226,  227,  269,
 /*    80 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*    90 */   269,  269,  181,  269,  180,  179,  269,  269,  269,  269,
 /*   100 */   269,  255,  255,  269,  269,  217,  191,  269,  178,  269,
 /*   110 */   176,  182,  215,  216,  214,  208,  207,  206,  201,  200,
 /*   120 */   205,  204,  203,  202,  199,  198,  195,  244,  255,  254,
 /*   130 */   248,  247,  267,  268,  265,  256,  266,  264,  263,  261,
 /*   140 */   260,  262,  259,  258,  257,  250,  249,  245,  243,  242,
 /*   150 */   241,  239,  230,  229,  228,  225,  224,  223,  222,  221,
 /*   160 */   220,  219,  218,  217,  193,  192,  190,  189,  188,  187,
 /*   170 */   186,  185,  184,  183,  177,
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
  "LP",            "INT",           "ROWS",          "RP",          
  "SLIDE",         "SECONDS",       "MINUTES",       "HOURS",       
  "DAYS",          "GROUP",         "INTO",          "BY",          
  "REDUCE",        "AS",            "SQ_STRING",     "DOUBLE",      
  "TRUE",          "FALSE",         "NUL",           "LCB",         
  "RCB",           "DOT_STAR",      "COLON",         "LSB",         
  "RSB",           "DOLLAR",        "DOT",           "error",       
  "commands",      "command",       "stream_cmd",    "filter_cmd",  
  "transform_cmd",  "sliding_count_window_cmd",  "sliding_time_window_cmd",  "time_interval",
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
 /*  13 */ "command ::= sliding_time_window_cmd",
 /*  14 */ "command ::= group_cmd",
 /*  15 */ "command ::= reduce_cmd",
 /*  16 */ "stream_cmd ::= SSTREAM DQ_STRING",
 /*  17 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA SHUFFLE",
 /*  18 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA ALL",
 /*  19 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA expr",
 /*  20 */ "stream_cmd ::= OSTREAM DQ_STRING",
 /*  21 */ "filter_cmd ::= FILTER expr",
 /*  22 */ "transform_cmd ::= TRANSFORM expr",
 /*  23 */ "sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS RP",
 /*  24 */ "sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS SLIDE INT RP",
 /*  25 */ "sliding_time_window_cmd ::= WINDOW_KW LP time_interval RP",
 /*  26 */ "sliding_time_window_cmd ::= WINDOW_KW LP time_interval SLIDE time_interval RP",
 /*  27 */ "time_interval ::= INT SECONDS",
 /*  28 */ "time_interval ::= INT MINUTES",
 /*  29 */ "time_interval ::= INT HOURS",
 /*  30 */ "time_interval ::= INT DAYS",
 /*  31 */ "time_interval ::= time_interval INT HOURS",
 /*  32 */ "time_interval ::= time_interval INT MINUTES",
 /*  33 */ "time_interval ::= time_interval INT SECONDS",
 /*  34 */ "group_cmd ::= GROUP INTO expr",
 /*  35 */ "group_cmd ::= GROUP BY group_exprs INTO expr",
 /*  36 */ "reduce_cmd ::= REDUCE INTO expr",
 /*  37 */ "reduce_cmd ::= REDUCE BY group_exprs INTO expr",
 /*  38 */ "group_expr ::= expr",
 /*  39 */ "group_expr ::= expr AS ID",
 /*  40 */ "group_exprs ::= group_expr",
 /*  41 */ "group_exprs ::= group_exprs COMMA group_expr",
 /*  42 */ "expr ::= pri_expr",
 /*  43 */ "pri_expr ::= LP pri_expr RP",
 /*  44 */ "pri_expr ::= DQ_STRING",
 /*  45 */ "pri_expr ::= SQ_STRING",
 /*  46 */ "pri_expr ::= INT",
 /*  47 */ "pri_expr ::= DOUBLE",
 /*  48 */ "pri_expr ::= TRUE",
 /*  49 */ "pri_expr ::= FALSE",
 /*  50 */ "pri_expr ::= NUL",
 /*  51 */ "pri_expr ::= expr ADD expr",
 /*  52 */ "pri_expr ::= expr SUB expr",
 /*  53 */ "pri_expr ::= expr MUL expr",
 /*  54 */ "pri_expr ::= expr DIV expr",
 /*  55 */ "pri_expr ::= expr MOD expr",
 /*  56 */ "pri_expr ::= expr LT expr",
 /*  57 */ "pri_expr ::= expr GT expr",
 /*  58 */ "pri_expr ::= expr LE expr",
 /*  59 */ "pri_expr ::= expr GE expr",
 /*  60 */ "pri_expr ::= expr EQ expr",
 /*  61 */ "pri_expr ::= expr NE expr",
 /*  62 */ "pri_expr ::= expr OR expr",
 /*  63 */ "pri_expr ::= expr AND expr",
 /*  64 */ "pri_expr ::= NOT expr",
 /*  65 */ "pri_expr ::= expr IN expr",
 /*  66 */ "expr ::= record",
 /*  67 */ "record ::= LCB RCB",
 /*  68 */ "record ::= LCB fields RCB",
 /*  69 */ "fields ::= field",
 /*  70 */ "fields ::= fields COMMA field",
 /*  71 */ "field ::= field_name expr",
 /*  72 */ "field ::= path_expr DOT_STAR",
 /*  73 */ "field_name ::= ID COLON",
 /*  74 */ "expr ::= LSB RSB",
 /*  75 */ "expr ::= LSB exprs RSB",
 /*  76 */ "exprs ::= expr",
 /*  77 */ "exprs ::= exprs COMMA expr",
 /*  78 */ "expr ::= path_expr",
 /*  79 */ "path_expr ::= LP path_expr RP",
 /*  80 */ "path_expr ::= ID",
 /*  81 */ "path_expr ::= DOLLAR",
 /*  82 */ "path_expr ::= path_expr project_name",
 /*  83 */ "project_name ::= DOT ID",
 /*  84 */ "path_expr ::= path_expr LCB project_names RCB",
 /*  85 */ "path_expr ::= path_expr LCB MUL SUB project_names RCB",
 /*  86 */ "project_names ::= project_name",
 /*  87 */ "project_names ::= project_names COMMA project_name",
 /*  88 */ "path_expr ::= path_expr LSB expr RSB",
 /*  89 */ "path_expr ::= path_expr LSB expr COLON expr RSB",
 /*  90 */ "expr ::= func_expr",
 /*  91 */ "func_name ::= ID LP",
 /*  92 */ "func_expr ::= func_name RP",
 /*  93 */ "func_expr ::= func_name exprs RP",
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
  { 76, 1 },
  { 77, 2 },
  { 77, 3 },
  { 78, 1 },
  { 78, 3 },
  { 78, 3 },
  { 78, 5 },
  { 56, 1 },
  { 56, 3 },
  { 57, 1 },
  { 57, 1 },
  { 57, 1 },
  { 57, 1 },
  { 57, 1 },
  { 57, 1 },
  { 57, 1 },
  { 58, 2 },
  { 58, 4 },
  { 58, 4 },
  { 58, 4 },
  { 58, 2 },
  { 59, 2 },
  { 60, 2 },
  { 61, 5 },
  { 61, 7 },
  { 62, 4 },
  { 62, 6 },
  { 63, 2 },
  { 63, 2 },
  { 63, 2 },
  { 63, 2 },
  { 63, 3 },
  { 63, 3 },
  { 63, 3 },
  { 64, 3 },
  { 64, 5 },
  { 65, 3 },
  { 65, 5 },
  { 66, 1 },
  { 66, 3 },
  { 67, 1 },
  { 67, 3 },
  { 68, 1 },
  { 69, 3 },
  { 69, 1 },
  { 69, 1 },
  { 69, 1 },
  { 69, 1 },
  { 69, 1 },
  { 69, 1 },
  { 69, 1 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 3 },
  { 69, 2 },
  { 69, 3 },
  { 68, 1 },
  { 79, 2 },
  { 79, 3 },
  { 80, 1 },
  { 80, 3 },
  { 81, 2 },
  { 81, 2 },
  { 82, 2 },
  { 68, 2 },
  { 68, 3 },
  { 70, 1 },
  { 70, 3 },
  { 68, 1 },
  { 71, 3 },
  { 71, 1 },
  { 71, 1 },
  { 71, 2 },
  { 74, 2 },
  { 71, 4 },
  { 71, 6 },
  { 75, 1 },
  { 75, 3 },
  { 71, 4 },
  { 71, 6 },
  { 68, 1 },
  { 72, 2 },
  { 73, 2 },
  { 73, 3 },
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
#line 69 "speparser.y"
{
      std::cout << "sth like 'c1->operator' is resolved." << std::endl;
      printCommands(yymsp[0].minor.yy137);  
      // g_pCommands = yymsp[0].minor.yy137;    

	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy137
	  ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == bPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  bPoint->addTargetPipeline(yymsp[0].minor.yy137);      
   }
#line 1056 "speparser.c"
        break;
      case 5: /* stmt ::= ID EQ commands */
#line 83 "speparser.y"
{
	  std::cout << "sth like 'c1 = operator->...' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy137); 
	  // g_pCommands = yymsp[0].minor.yy137;  

	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-2].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' already exists." << std::endl;		 
	  }	  
   }
#line 1071 "speparser.c"
        break;
      case 6: /* stmt ::= ID EQ ID PIPE commands */
#line 95 "speparser.y"
{
	  std::cout << "sth like 'c2 = c1->operator' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy137);
	  // g_pCommands = yymsp[0].minor.yy137;  
	  
	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-4].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-4].minor.yy0.z << "' already exists." << std::endl;		 
	  }
	  
	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy137
	  ConnectPointOperator* cPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == cPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  cPoint->addTargetPipeline(yymsp[0].minor.yy137);
   }
#line 1094 "speparser.c"
        break;
      case 7: /* commands ::= command */
#line 115 "speparser.y"
{
      yygotominor.yy137 = new vector<BaseOperator*>();
      yygotominor.yy137->push_back(yymsp[0].minor.yy80);
      
      // set current pipeline, add it into all pipelines
      pParseContext->curPipeline = yygotominor.yy137;
      pParseContext->allPipelines.push_back(yygotominor.yy137);
}
#line 1106 "speparser.c"
        break;
      case 8: /* commands ::= commands PIPE command */
#line 124 "speparser.y"
{
      yygotominor.yy137 = yymsp[-2].minor.yy137;
      yygotominor.yy137->push_back(yymsp[0].minor.yy80);
}
#line 1114 "speparser.c"
        break;
      case 9: /* command ::= stream_cmd */
      case 10: /* command ::= filter_cmd */ yytestcase(yyruleno==10);
      case 11: /* command ::= transform_cmd */ yytestcase(yyruleno==11);
      case 12: /* command ::= sliding_count_window_cmd */ yytestcase(yyruleno==12);
      case 13: /* command ::= sliding_time_window_cmd */ yytestcase(yyruleno==13);
      case 14: /* command ::= group_cmd */ yytestcase(yyruleno==14);
      case 15: /* command ::= reduce_cmd */ yytestcase(yyruleno==15);
#line 129 "speparser.y"
{
      yygotominor.yy80 = yymsp[0].minor.yy80;
}
#line 1127 "speparser.c"
        break;
      case 16: /* stream_cmd ::= SSTREAM DQ_STRING */
#line 155 "speparser.y"
{
      yygotominor.yy80 = new StreamOperator(yymsp[0].minor.yy0.z, 0);
}
#line 1134 "speparser.c"
        break;
      case 17: /* stream_cmd ::= SSTREAM DQ_STRING COMMA SHUFFLE */
#line 159 "speparser.y"
{
      yygotominor.yy80 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionRoundrobin());
}
#line 1141 "speparser.c"
        break;
      case 18: /* stream_cmd ::= SSTREAM DQ_STRING COMMA ALL */
#line 163 "speparser.y"
{
      yygotominor.yy80 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionAll());
}
#line 1148 "speparser.c"
        break;
      case 19: /* stream_cmd ::= SSTREAM DQ_STRING COMMA expr */
#line 167 "speparser.y"
{
      yygotominor.yy80 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionPerValue(yymsp[0].minor.yy10));
}
#line 1155 "speparser.c"
        break;
      case 20: /* stream_cmd ::= OSTREAM DQ_STRING */
#line 171 "speparser.y"
{
      yygotominor.yy80 = new StreamOperator(yymsp[0].minor.yy0.z, 1);
}
#line 1162 "speparser.c"
        break;
      case 21: /* filter_cmd ::= FILTER expr */
#line 175 "speparser.y"
{
      yygotominor.yy80 = new FilterOperator(yymsp[0].minor.yy10);
}
#line 1169 "speparser.c"
        break;
      case 22: /* transform_cmd ::= TRANSFORM expr */
#line 179 "speparser.y"
{
	  yygotominor.yy80 = new TransformOperator(yymsp[0].minor.yy10);
}
#line 1176 "speparser.c"
        break;
      case 23: /* sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS RP */
#line 183 "speparser.y"
{
	yygotominor.yy80 = new SlidingCountWindowOperator(atoi(yymsp[-2].minor.yy0.z), 1);
}
#line 1183 "speparser.c"
        break;
      case 24: /* sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS SLIDE INT RP */
#line 187 "speparser.y"
{
	yygotominor.yy80 = new SlidingCountWindowOperator(atoi(yymsp[-4].minor.yy0.z), atoi(yymsp[-1].minor.yy0.z));
}
#line 1190 "speparser.c"
        break;
      case 25: /* sliding_time_window_cmd ::= WINDOW_KW LP time_interval RP */
#line 191 "speparser.y"
{
	yygotominor.yy80 = new SlidingTimeWindowOperator(yymsp[-1].minor.yy43, 1);	
	if (yymsp[-1].minor.yy43 <= 0) {
		pParseContext->errorCode = -3;
		printf("Query Error: invalid time window size.\n ");	
	} 
}
#line 1201 "speparser.c"
        break;
      case 26: /* sliding_time_window_cmd ::= WINDOW_KW LP time_interval SLIDE time_interval RP */
#line 199 "speparser.y"
{
	yygotominor.yy80 = new SlidingTimeWindowOperator(yymsp[-3].minor.yy43, yymsp[-1].minor.yy43);
	if (yymsp[-3].minor.yy43 <= 0) {
		pParseContext->errorCode = -3;
		printf("Query Error: invalid time window size.\n ");	
	} 
	if (yymsp[-3].minor.yy43 <= 0) {
		pParseContext->errorCode = -4;
		printf("Query Error: invalid time window slide step.\n ");	
	} 			
}
#line 1216 "speparser.c"
        break;
      case 27: /* time_interval ::= INT SECONDS */
#line 211 "speparser.y"
{
    yygotominor.yy43 = atolong64(yymsp[-1].minor.yy0.z) * 1000 * 1000;
}
#line 1223 "speparser.c"
        break;
      case 28: /* time_interval ::= INT MINUTES */
#line 215 "speparser.y"
{
    yygotominor.yy43 = atolong64(yymsp[-1].minor.yy0.z) * 60 * 1000 * 1000;
}
#line 1230 "speparser.c"
        break;
      case 29: /* time_interval ::= INT HOURS */
#line 219 "speparser.y"
{
    yygotominor.yy43 = atolong64(yymsp[-1].minor.yy0.z) * 60* 60 * 1000 * 1000;
}
#line 1237 "speparser.c"
        break;
      case 30: /* time_interval ::= INT DAYS */
#line 223 "speparser.y"
{
    yygotominor.yy43 = atolong64(yymsp[-1].minor.yy0.z) * 24* 60* 60 * 1000 * 1000;
}
#line 1244 "speparser.c"
        break;
      case 31: /* time_interval ::= time_interval INT HOURS */
#line 227 "speparser.y"
{
	yygotominor.yy43 = yymsp[-2].minor.yy43 + atolong64(yymsp[-1].minor.yy0.z) * 60* 60 * 1000 * 1000;
}
#line 1251 "speparser.c"
        break;
      case 32: /* time_interval ::= time_interval INT MINUTES */
#line 231 "speparser.y"
{
	yygotominor.yy43 = yymsp[-2].minor.yy43 + atolong64(yymsp[-1].minor.yy0.z) * 60 * 1000 * 1000;
}
#line 1258 "speparser.c"
        break;
      case 33: /* time_interval ::= time_interval INT SECONDS */
#line 235 "speparser.y"
{
	yygotominor.yy43 = yymsp[-2].minor.yy43 + atolong64(yymsp[-1].minor.yy0.z) * 1000 * 1000;
}
#line 1265 "speparser.c"
        break;
      case 34: /* group_cmd ::= GROUP INTO expr */
#line 240 "speparser.y"
{
    yygotominor.yy80 = new GroupOperator(NULL, yymsp[0].minor.yy10);
}
#line 1272 "speparser.c"
        break;
      case 35: /* group_cmd ::= GROUP BY group_exprs INTO expr */
#line 244 "speparser.y"
{
    yygotominor.yy80 = new GroupOperator(yymsp[-2].minor.yy104, yymsp[0].minor.yy10);
}
#line 1279 "speparser.c"
        break;
      case 36: /* reduce_cmd ::= REDUCE INTO expr */
#line 248 "speparser.y"
{
    yygotominor.yy80 = new ReduceOperator(NULL, yymsp[0].minor.yy10);
}
#line 1286 "speparser.c"
        break;
      case 37: /* reduce_cmd ::= REDUCE BY group_exprs INTO expr */
#line 252 "speparser.y"
{
    yygotominor.yy80 = new ReduceOperator(yymsp[-2].minor.yy104, yymsp[0].minor.yy10);
}
#line 1293 "speparser.c"
        break;
      case 38: /* group_expr ::= expr */
#line 256 "speparser.y"
{
    yygotominor.yy3 = new GroupExpr(yymsp[0].minor.yy10, "");
}
#line 1300 "speparser.c"
        break;
      case 39: /* group_expr ::= expr AS ID */
#line 260 "speparser.y"
{
    yygotominor.yy3 = new GroupExpr(yymsp[-2].minor.yy10, yymsp[0].minor.yy0.z);
}
#line 1307 "speparser.c"
        break;
      case 40: /* group_exprs ::= group_expr */
#line 264 "speparser.y"
{
    yygotominor.yy104 = new vector<GroupExpr*>();
    yygotominor.yy104->push_back(yymsp[0].minor.yy3);
}
#line 1315 "speparser.c"
        break;
      case 41: /* group_exprs ::= group_exprs COMMA group_expr */
#line 269 "speparser.y"
{
    yymsp[-2].minor.yy104->push_back(yymsp[0].minor.yy3);
    yygotominor.yy104 = yymsp[-2].minor.yy104;
}
#line 1323 "speparser.c"
        break;
      case 42: /* expr ::= pri_expr */
      case 90: /* expr ::= func_expr */ yytestcase(yyruleno==90);
#line 274 "speparser.y"
{
	yygotominor.yy10 = yymsp[0].minor.yy10;
}
#line 1331 "speparser.c"
        break;
      case 43: /* pri_expr ::= LP pri_expr RP */
#line 278 "speparser.y"
{
      yygotominor.yy10 = yymsp[-1].minor.yy10;
}
#line 1338 "speparser.c"
        break;
      case 44: /* pri_expr ::= DQ_STRING */
      case 45: /* pri_expr ::= SQ_STRING */ yytestcase(yyruleno==45);
#line 282 "speparser.y"
{
      // strip off the quote char
      std::string s(yymsp[0].minor.yy0.z+1, strlen(yymsp[0].minor.yy0.z)-2);
      yygotominor.yy10 = new StringValue(s.c_str());
}
#line 1348 "speparser.c"
        break;
      case 46: /* pri_expr ::= INT */
#line 294 "speparser.y"
{
      yygotominor.yy10 = new IntValue(atoi(yymsp[0].minor.yy0.z));
}
#line 1355 "speparser.c"
        break;
      case 47: /* pri_expr ::= DOUBLE */
#line 298 "speparser.y"
{
      yygotominor.yy10 = new DoubleValue(atof(yymsp[0].minor.yy0.z));
}
#line 1362 "speparser.c"
        break;
      case 48: /* pri_expr ::= TRUE */
#line 302 "speparser.y"
{
      yygotominor.yy10 = new BoolValue(true);
}
#line 1369 "speparser.c"
        break;
      case 49: /* pri_expr ::= FALSE */
#line 306 "speparser.y"
{
      yygotominor.yy10 = new BoolValue(false);
}
#line 1376 "speparser.c"
        break;
      case 50: /* pri_expr ::= NUL */
#line 309 "speparser.y"
{
      yygotominor.yy10 = new NullValue();
}
#line 1383 "speparser.c"
        break;
      case 51: /* pri_expr ::= expr ADD expr */
#line 313 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_ADD, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1390 "speparser.c"
        break;
      case 52: /* pri_expr ::= expr SUB expr */
#line 317 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_SUB, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1397 "speparser.c"
        break;
      case 53: /* pri_expr ::= expr MUL expr */
#line 321 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_MUL, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1404 "speparser.c"
        break;
      case 54: /* pri_expr ::= expr DIV expr */
#line 324 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_DIV, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1411 "speparser.c"
        break;
      case 55: /* pri_expr ::= expr MOD expr */
#line 328 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_MOD, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1418 "speparser.c"
        break;
      case 56: /* pri_expr ::= expr LT expr */
#line 332 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_LT, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1425 "speparser.c"
        break;
      case 57: /* pri_expr ::= expr GT expr */
#line 336 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_GT, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1432 "speparser.c"
        break;
      case 58: /* pri_expr ::= expr LE expr */
#line 340 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_LE, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1439 "speparser.c"
        break;
      case 59: /* pri_expr ::= expr GE expr */
#line 344 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_GE, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1446 "speparser.c"
        break;
      case 60: /* pri_expr ::= expr EQ expr */
#line 348 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_EQ, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1453 "speparser.c"
        break;
      case 61: /* pri_expr ::= expr NE expr */
#line 352 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_NE, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1460 "speparser.c"
        break;
      case 62: /* pri_expr ::= expr OR expr */
#line 356 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_OR, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1467 "speparser.c"
        break;
      case 63: /* pri_expr ::= expr AND expr */
#line 360 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_AND, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1474 "speparser.c"
        break;
      case 64: /* pri_expr ::= NOT expr */
#line 364 "speparser.y"
{
      yygotominor.yy10 = new UnaryExpr(Expr::OP_NOT, yymsp[0].minor.yy10);
}
#line 1481 "speparser.c"
        break;
      case 65: /* pri_expr ::= expr IN expr */
#line 368 "speparser.y"
{
      yygotominor.yy10 = new BinaryExpr(Expr::OP_IN, yymsp[-2].minor.yy10, yymsp[0].minor.yy10);
}
#line 1488 "speparser.c"
        break;
      case 66: /* expr ::= record */
#line 377 "speparser.y"
{
	yygotominor.yy10 = yymsp[0].minor.yy30;
}
#line 1495 "speparser.c"
        break;
      case 67: /* record ::= LCB RCB */
#line 381 "speparser.y"
{
	yygotominor.yy30 = new RecordConstructExpr( new vector<RecordMemberExpr*>());
}
#line 1502 "speparser.c"
        break;
      case 68: /* record ::= LCB fields RCB */
#line 385 "speparser.y"
{
	yygotominor.yy30 = new RecordConstructExpr(yymsp[-1].minor.yy78);
}
#line 1509 "speparser.c"
        break;
      case 69: /* fields ::= field */
#line 389 "speparser.y"
{
	yygotominor.yy78 = new vector<RecordMemberExpr*>();
	yygotominor.yy78->push_back(yymsp[0].minor.yy129);
}
#line 1517 "speparser.c"
        break;
      case 70: /* fields ::= fields COMMA field */
#line 394 "speparser.y"
{
	yymsp[-2].minor.yy78->push_back(yymsp[0].minor.yy129);
	yygotominor.yy78 = yymsp[-2].minor.yy78;
}
#line 1525 "speparser.c"
        break;
      case 71: /* field ::= field_name expr */
#line 399 "speparser.y"
{
	yygotominor.yy129 = new RecordMemberExpr(yymsp[-1].minor.yy36->c_str(), yymsp[0].minor.yy10);
}
#line 1532 "speparser.c"
        break;
      case 72: /* field ::= path_expr DOT_STAR */
#line 403 "speparser.y"
{
	yygotominor.yy129 = new RecordMemberExpr("*", yymsp[-1].minor.yy149);
}
#line 1539 "speparser.c"
        break;
      case 73: /* field_name ::= ID COLON */
#line 407 "speparser.y"
{
	yygotominor.yy36 = new string(yymsp[-1].minor.yy0.z);
}
#line 1546 "speparser.c"
        break;
      case 74: /* expr ::= LSB RSB */
#line 411 "speparser.y"
{
    yygotominor.yy10 = new ArrayConstructExpr();
}
#line 1553 "speparser.c"
        break;
      case 75: /* expr ::= LSB exprs RSB */
#line 415 "speparser.y"
{
	yygotominor.yy10 = new ArrayConstructExpr(yymsp[-1].minor.yy101);
}
#line 1560 "speparser.c"
        break;
      case 76: /* exprs ::= expr */
#line 419 "speparser.y"
{
	yygotominor.yy101 = new vector<Expr*>();
	yygotominor.yy101->push_back(yymsp[0].minor.yy10);
}
#line 1568 "speparser.c"
        break;
      case 77: /* exprs ::= exprs COMMA expr */
#line 424 "speparser.y"
{
	yygotominor.yy101 = yymsp[-2].minor.yy101;
	yygotominor.yy101->push_back(yymsp[0].minor.yy10);
}
#line 1576 "speparser.c"
        break;
      case 78: /* expr ::= path_expr */
#line 429 "speparser.y"
{
      yygotominor.yy10 = yymsp[0].minor.yy149;
}
#line 1583 "speparser.c"
        break;
      case 79: /* path_expr ::= LP path_expr RP */
#line 433 "speparser.y"
{
	  yygotominor.yy149 = yymsp[-1].minor.yy149;
}
#line 1590 "speparser.c"
        break;
      case 80: /* path_expr ::= ID */
#line 437 "speparser.y"
{
      yygotominor.yy149 = new VarRefExpr(yymsp[0].minor.yy0.z);
}
#line 1597 "speparser.c"
        break;
      case 81: /* path_expr ::= DOLLAR */
#line 440 "speparser.y"
{
      yygotominor.yy149 = new VarRefExpr("$");
}
#line 1604 "speparser.c"
        break;
      case 82: /* path_expr ::= path_expr project_name */
#line 444 "speparser.y"
{
      yymsp[-1].minor.yy149->addChildPathExpr(new StepExpr(yymsp[0].minor.yy36->c_str()) );
      yygotominor.yy149 = yymsp[-1].minor.yy149;
      delete yymsp[0].minor.yy36;
}
#line 1613 "speparser.c"
        break;
      case 83: /* project_name ::= DOT ID */
#line 450 "speparser.y"
{
      yygotominor.yy36 = new string(yymsp[0].minor.yy0.z);
}
#line 1620 "speparser.c"
        break;
      case 84: /* path_expr ::= path_expr LCB project_names RCB */
#line 454 "speparser.y"
{
	  yymsp[-3].minor.yy149->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy115, false));
	  yygotominor.yy149 = yymsp[-3].minor.yy149;
}
#line 1628 "speparser.c"
        break;
      case 85: /* path_expr ::= path_expr LCB MUL SUB project_names RCB */
#line 459 "speparser.y"
{
	  yymsp[-5].minor.yy149->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy115, true));
	  yygotominor.yy149 = yymsp[-5].minor.yy149;
}
#line 1636 "speparser.c"
        break;
      case 86: /* project_names ::= project_name */
#line 464 "speparser.y"
{
      yygotominor.yy115 = new vector<string>();
      yygotominor.yy115->push_back(string(yymsp[0].minor.yy36->c_str()));
      delete yymsp[0].minor.yy36; 
}
#line 1645 "speparser.c"
        break;
      case 87: /* project_names ::= project_names COMMA project_name */
#line 469 "speparser.y"
{
      yymsp[-2].minor.yy115->push_back(string(yymsp[0].minor.yy36->c_str()));
      yygotominor.yy115 = yymsp[-2].minor.yy115;
      delete yymsp[0].minor.yy36;
}
#line 1654 "speparser.c"
        break;
      case 88: /* path_expr ::= path_expr LSB expr RSB */
#line 475 "speparser.y"
{
	yymsp[-3].minor.yy149->addChildPathExpr(new ArrayElementsExpr(yymsp[-1].minor.yy10,NULL));
	yygotominor.yy149 = yymsp[-3].minor.yy149;	  
}
#line 1662 "speparser.c"
        break;
      case 89: /* path_expr ::= path_expr LSB expr COLON expr RSB */
#line 480 "speparser.y"
{  
	yymsp[-5].minor.yy149->addChildPathExpr(new ArrayElementsExpr(yymsp[-3].minor.yy10,yymsp[-1].minor.yy10));
	yygotominor.yy149 = yymsp[-5].minor.yy149;
}
#line 1670 "speparser.c"
        break;
      case 91: /* func_name ::= ID LP */
#line 490 "speparser.y"
{
	yygotominor.yy36 = new string(yymsp[-1].minor.yy0.z);
	printf("recongnize a function %s.\n ", yygotominor.yy36->c_str());
}
#line 1678 "speparser.c"
        break;
      case 92: /* func_expr ::= func_name RP */
#line 495 "speparser.y"
{
    vector<Expr*>* args = new vector<Expr*>();
	yygotominor.yy10 = createBuiltinFunction(yymsp[-1].minor.yy36->c_str(), args);
	if (!yygotominor.yy10) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-1].minor.yy36->c_str());
	}
	delete yymsp[-1].minor.yy36;
}
#line 1691 "speparser.c"
        break;
      case 93: /* func_expr ::= func_name exprs RP */
#line 505 "speparser.y"
{
	yygotominor.yy10 = createBuiltinFunction(yymsp[-2].minor.yy36->c_str(), yymsp[-1].minor.yy101);
	if (!yygotominor.yy10) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-2].minor.yy36->c_str());
	}
	delete yymsp[-2].minor.yy36;
}
#line 1703 "speparser.c"
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
#line 59 "speparser.y"

  /* The tokenizer always gives us a token */
  std::cout << "near at the Token (line #" << TOKEN.line << ", column #" << TOKEN.column << "), Syntax error!" << std::endl;
  pParseContext->errorCode = -1;
#line 1774 "speparser.c"
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
