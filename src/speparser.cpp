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
#include "join.h"
#include "speparser.h"

using namespace std;

	void printCommands(vector<BaseOperator*>* pList) {
	    if (pList == NULL)
	       return;

        for (int i=0; i< pList->size(); i++) {
           cout <<"op[" << i << "] = " << (*pList)[i]->toString().c_str() << endl;
        }
	};
	
#line 33 "speparser.c"
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
#define YYNOCODE 90
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE Token
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  vector<string>* yy5;
  JoinDataSource* yy14;
  vector<GroupExpr*>* yy20;
  RecordMemberExpr* yy49;
  string* yy54;
  RecordConstructExpr* yy55;
  BaseOperator* yy56;
  vector<RecordMemberExpr*>* yy78;
  PathExpr* yy87;
  vector<Expr*>* yy127;
  vector<BaseOperator*> * yy129;
  Expr* yy136;
  long64 yy141;
  vector<JoinDataSource*>* yy143;
  JoinOperator* yy149;
  GroupExpr* yy175;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL ParseContext* pParseContext;
#define ParseARG_PDECL ,ParseContext* pParseContext
#define ParseARG_FETCH ParseContext* pParseContext = yypParser->pParseContext
#define ParseARG_STORE yypParser->pParseContext = pParseContext
#define YYNSTATE 192
#define YYNRULE 101
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
#define YY_ACTTAB_COUNT (724)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    27,   26,   25,   33,   32,   31,   30,   29,   28,   39,
 /*    10 */    37,   36,   35,   34,   27,   26,   25,   33,   32,   31,
 /*    20 */    30,   29,   28,   39,   37,   36,   35,   34,   27,   26,
 /*    30 */    25,   33,   32,   31,   30,   29,   28,   39,   37,   36,
 /*    40 */    35,   34,  191,  190,  189,  188,  187,  186,  185,  108,
 /*    50 */   184,  183,   20,    7,  155,   26,   25,   33,   32,   31,
 /*    60 */    30,   29,   28,   39,   37,   36,   35,   34,  154,  113,
 /*    70 */    27,   26,   25,   33,   32,   31,   30,   29,   28,   39,
 /*    80 */    37,   36,   35,   34,  145,   48,   24,   17,  114,  118,
 /*    90 */    12,   68,  180,   98,   49,    8,  151,  178,  182,  181,
 /*   100 */    48,   42,  167,  166,   38,  176,  179,  119,  152,    6,
 /*   110 */    22,  112,   19,   18,  111,   22,  177,  175,  174,  173,
 /*   120 */   172,   40,  150,  152,   92,    9,   90,  152,   41,   27,
 /*   130 */    26,   25,   33,   32,   31,   30,   29,   28,   39,   37,
 /*   140 */    36,   35,   34,  129,   91,   24,  162,  114,   58,  180,
 /*   150 */    11,   49,    8,  151,  160,  165,  178,   85,  180,  167,
 /*   160 */    49,    8,  151,   38,  176,   36,   35,   34,  167,  132,
 /*   170 */   133,  134,   24,  153,  114,  177,  175,  174,  173,  172,
 /*   180 */    40,  113,   44,  178,    9,  163,  152,  147,  142,  110,
 /*   190 */    38,  176,   83,  149,  136,   53,  143,   48,   47,   24,
 /*   200 */    55,  114,  177,  175,  174,  173,  172,   40,  164,   23,
 /*   210 */   178,    9,   43,  152,  148,   55,   21,   38,  176,  116,
 /*   220 */   152,   39,   37,   36,   35,   34,   47,  159,   54,  177,
 /*   230 */   175,  174,  173,  172,   40,   99,  144,   23,    9,   83,
 /*   240 */   152,  135,  157,  115,  120,  128,  189,  188,  187,  186,
 /*   250 */   185,  109,  184,  183,   16,  130,  127,   10,  131,   14,
 /*   260 */    58,  180,   10,   49,    8,  151,  105,  294,    1,  100,
 /*   270 */   141,  167,   33,   32,   31,   30,   29,   28,   39,   37,
 /*   280 */    36,   35,   34,  126,  140,  139,  138,  137,  116,  120,
 /*   290 */   128,  189,  188,  187,  186,  185,   50,  184,  183,    3,
 /*   300 */    15,  127,  156,   96,  107,  128,  189,  188,  187,  186,
 /*   310 */   185,  122,  184,  183,  121,   94,  127,    2,  140,  139,
 /*   320 */   138,  137,    5,  156,   97,  104,   87,    4,  101,  128,
 /*   330 */   189,  188,  187,  186,  185,  192,  184,  183,  158,   84,
 /*   340 */   127,  103,  128,  189,  188,  187,  186,  185,  116,  184,
 /*   350 */   183,  106,   88,  127,   42,   43,   75,   51,   93,   21,
 /*   360 */   119,  102,  116,   42,  112,   19,   18,  111,  124,  119,
 /*   370 */    88,   82,   42,  112,   19,   18,  111,   92,  119,   90,
 /*   380 */   125,  161,  112,   19,   18,  111,   92,   46,   90,   68,
 /*   390 */   180,   95,   49,    8,  151,   92,  123,   90,  129,   89,
 /*   400 */   167,  295,  295,   58,  180,  119,   49,    8,  151,  112,
 /*   410 */    19,   18,  111,  295,  167,   70,  117,  295,   45,    8,
 /*   420 */   151,   13,   92,  295,   90,   52,  167,  295,   86,  180,
 /*   430 */   295,   49,    8,  151,  295,  295,  295,  295,  295,  167,
 /*   440 */   171,  180,  295,   49,    8,  151,  295,  295,  295,  170,
 /*   450 */   180,  167,   49,    8,  151,  295,  295,  295,  169,  180,
 /*   460 */   167,   49,    8,  151,  295,  295,  295,   81,  180,  167,
 /*   470 */    49,    8,  151,  295,  295,  295,  295,  295,  167,   80,
 /*   480 */   180,  295,   49,    8,  151,  295,  295,  295,  295,  295,
 /*   490 */   167,   79,  180,  295,   49,    8,  151,  295,  295,  295,
 /*   500 */   295,  295,  167,   78,  180,  295,   49,    8,  151,  295,
 /*   510 */   295,  295,  295,  295,  167,   77,  180,  295,   49,    8,
 /*   520 */   151,  295,  295,  295,   76,  180,  167,   49,    8,  151,
 /*   530 */   295,  295,  295,   72,  180,  167,   49,    8,  151,  295,
 /*   540 */   295,  295,   74,  180,  167,   49,    8,  151,  295,  295,
 /*   550 */   295,  295,  295,  167,   73,  180,  295,   49,    8,  151,
 /*   560 */   295,  295,  295,  295,  295,  167,  168,  180,  295,   49,
 /*   570 */     8,  151,  295,  295,  295,  295,  295,  167,   69,  180,
 /*   580 */   295,   49,    8,  151,  295,  295,  295,  295,  295,  167,
 /*   590 */    67,  180,  295,   49,    8,  151,  295,  295,  295,   56,
 /*   600 */   180,  167,   49,    8,  151,  295,  295,  295,   59,  180,
 /*   610 */   167,   49,    8,  151,  295,  295,  295,   66,  180,  167,
 /*   620 */    49,    8,  151,  295,  295,  295,  295,  295,  167,   65,
 /*   630 */   180,  295,   49,    8,  151,  295,  295,  295,  295,  295,
 /*   640 */   167,   64,  180,  295,   49,    8,  151,  295,  295,  295,
 /*   650 */   295,  295,  167,   63,  180,  295,   49,    8,  151,  295,
 /*   660 */   295,  295,  295,  295,  167,   62,  180,  295,   49,    8,
 /*   670 */   151,  295,  295,  295,   61,  180,  167,   49,    8,  151,
 /*   680 */   295,  295,  295,   57,  180,  167,   49,    8,  151,  295,
 /*   690 */   295,  295,   60,  180,  167,   49,    8,  151,  295,  295,
 /*   700 */   295,  295,  146,  167,   71,  180,  295,   49,    8,  151,
 /*   710 */   295,  295,  295,  295,  295,  167,   43,  295,  295,  295,
 /*   720 */    21,  295,  295,  116,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,    1,    2,    3,    4,    5,    6,
 /*    20 */     7,    8,    9,   10,   11,   12,   13,   14,    1,    2,
 /*    30 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    40 */    13,   14,   16,   60,   61,   62,   63,   64,   65,   22,
 /*    50 */    67,   68,   53,   18,   55,    2,    3,    4,    5,    6,
 /*    60 */     7,    8,    9,   10,   11,   12,   13,   14,   55,   17,
 /*    70 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    80 */    11,   12,   13,   14,   17,   33,   15,   21,   17,   26,
 /*    90 */    21,   74,   75,   76,   77,   78,   79,   26,   27,   28,
 /*   100 */    33,   19,   85,   51,   33,   34,   36,   25,   56,   43,
 /*   110 */    24,   29,   30,   31,   32,   24,   45,   46,   47,   48,
 /*   120 */    49,   50,   36,   56,   42,   54,   44,   56,   24,    1,
 /*   130 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   140 */    12,   13,   14,   69,   70,   15,   55,   17,   74,   75,
 /*   150 */    24,   77,   78,   79,   17,   51,   26,   74,   75,   85,
 /*   160 */    77,   78,   79,   33,   34,   12,   13,   14,   85,   38,
 /*   170 */    39,   40,   15,   33,   17,   45,   46,   47,   48,   49,
 /*   180 */    50,   17,   11,   26,   54,   55,   56,   53,   36,   37,
 /*   190 */    33,   34,   34,   36,   36,   37,   26,   33,   77,   15,
 /*   200 */    24,   17,   45,   46,   47,   48,   49,   50,   87,   88,
 /*   210 */    26,   54,   50,   56,   52,   24,   54,   33,   34,   57,
 /*   220 */    56,   10,   11,   12,   13,   14,   77,   51,   33,   45,
 /*   230 */    46,   47,   48,   49,   50,   86,   87,   88,   54,   34,
 /*   240 */    56,   36,   51,   12,   59,   60,   61,   62,   63,   64,
 /*   250 */    65,   34,   67,   68,   21,   69,   71,   24,   17,   21,
 /*   260 */    74,   75,   24,   77,   78,   79,   22,   82,   83,   84,
 /*   270 */    36,   85,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   280 */    12,   13,   14,   17,   38,   39,   40,   41,   57,   59,
 /*   290 */    60,   61,   62,   63,   64,   65,   24,   67,   68,   18,
 /*   300 */    21,   71,   80,   81,   59,   60,   61,   62,   63,   64,
 /*   310 */    65,   16,   67,   68,   84,   35,   71,    8,   38,   39,
 /*   320 */    40,   41,   43,   80,   81,   72,   73,   18,   59,   60,
 /*   330 */    61,   62,   63,   64,   65,    0,   67,   68,   80,   66,
 /*   340 */    71,   59,   60,   61,   62,   63,   64,   65,   57,   67,
 /*   350 */    68,   17,   17,   71,   19,   50,   34,   23,   66,   54,
 /*   360 */    25,   17,   57,   19,   29,   30,   31,   32,   72,   25,
 /*   370 */    17,   34,   19,   29,   30,   31,   32,   42,   25,   44,
 /*   380 */    72,   80,   29,   30,   31,   32,   42,   77,   44,   74,
 /*   390 */    75,   76,   77,   78,   79,   42,   72,   44,   69,   70,
 /*   400 */    85,   89,   89,   74,   75,   25,   77,   78,   79,   29,
 /*   410 */    30,   31,   32,   89,   85,   74,   75,   89,   77,   78,
 /*   420 */    79,   20,   42,   89,   44,   24,   85,   89,   74,   75,
 /*   430 */    89,   77,   78,   79,   89,   89,   89,   89,   89,   85,
 /*   440 */    74,   75,   89,   77,   78,   79,   89,   89,   89,   74,
 /*   450 */    75,   85,   77,   78,   79,   89,   89,   89,   74,   75,
 /*   460 */    85,   77,   78,   79,   89,   89,   89,   74,   75,   85,
 /*   470 */    77,   78,   79,   89,   89,   89,   89,   89,   85,   74,
 /*   480 */    75,   89,   77,   78,   79,   89,   89,   89,   89,   89,
 /*   490 */    85,   74,   75,   89,   77,   78,   79,   89,   89,   89,
 /*   500 */    89,   89,   85,   74,   75,   89,   77,   78,   79,   89,
 /*   510 */    89,   89,   89,   89,   85,   74,   75,   89,   77,   78,
 /*   520 */    79,   89,   89,   89,   74,   75,   85,   77,   78,   79,
 /*   530 */    89,   89,   89,   74,   75,   85,   77,   78,   79,   89,
 /*   540 */    89,   89,   74,   75,   85,   77,   78,   79,   89,   89,
 /*   550 */    89,   89,   89,   85,   74,   75,   89,   77,   78,   79,
 /*   560 */    89,   89,   89,   89,   89,   85,   74,   75,   89,   77,
 /*   570 */    78,   79,   89,   89,   89,   89,   89,   85,   74,   75,
 /*   580 */    89,   77,   78,   79,   89,   89,   89,   89,   89,   85,
 /*   590 */    74,   75,   89,   77,   78,   79,   89,   89,   89,   74,
 /*   600 */    75,   85,   77,   78,   79,   89,   89,   89,   74,   75,
 /*   610 */    85,   77,   78,   79,   89,   89,   89,   74,   75,   85,
 /*   620 */    77,   78,   79,   89,   89,   89,   89,   89,   85,   74,
 /*   630 */    75,   89,   77,   78,   79,   89,   89,   89,   89,   89,
 /*   640 */    85,   74,   75,   89,   77,   78,   79,   89,   89,   89,
 /*   650 */    89,   89,   85,   74,   75,   89,   77,   78,   79,   89,
 /*   660 */    89,   89,   89,   89,   85,   74,   75,   89,   77,   78,
 /*   670 */    79,   89,   89,   89,   74,   75,   85,   77,   78,   79,
 /*   680 */    89,   89,   89,   74,   75,   85,   77,   78,   79,   89,
 /*   690 */    89,   89,   74,   75,   85,   77,   78,   79,   89,   89,
 /*   700 */    89,   89,   36,   85,   74,   75,   89,   77,   78,   79,
 /*   710 */    89,   89,   89,   89,   89,   85,   50,   89,   89,   89,
 /*   720 */    54,   89,   89,   57,
};
#define YY_SHIFT_USE_DFLT (-2)
#define YY_SHIFT_COUNT (121)
#define YY_SHIFT_MIN   (-1)
#define YY_SHIFT_MAX   (666)
static const short yy_shift_ofst[] = {
 /*     0 */   353,  335,  344,   82,   82,  184,  184,  380,  157,  130,
 /*    10 */   184,   71,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    20 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    30 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    40 */    52,  164,  334,  231,  291,  666,  666,  162,   67,  305,
 /*    50 */   334,  334,  334,  337,  322,  291,   -1,   69,   27,   13,
 /*    60 */   128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
 /*    70 */   128,  128,   53,  268,  268,  280,  211,  211,  211,  211,
 /*    80 */   211,  211,  246,  131,  158,  153,  153,  401,  309,  238,
 /*    90 */   279,  233,   66,  205,  152,   86,  191,  176,   91,  104,
 /*   100 */   295,   35,  281,   35,  272,  266,  244,   35,  241,  234,
 /*   110 */   217,  195,  170,  134,  140,  171,  137,   70,  126,   63,
 /*   120 */    35,   26,
};
#define YY_REDUCE_USE_DFLT (-18)
#define YY_REDUCE_COUNT (55)
#define YY_REDUCE_MIN   (-17)
#define YY_REDUCE_MAX   (630)
static const short yy_reduce_ofst[] = {
 /*     0 */   185,  230,  282,  269,  245,  329,   74,  -17,  315,   17,
 /*    10 */   186,  630,  618,  609,  600,  591,  579,  567,  555,  543,
 /*    20 */   534,  525,  516,  504,  492,  480,  468,  459,  450,  441,
 /*    30 */   429,  417,  405,  393,  384,  375,  366,  354,  341,   83,
 /*    40 */   149,  121,  253,  243,  222,  301,  301,  301,  310,  301,
 /*    50 */   324,  308,  296,  292,  273,  258,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   293,  293,  293,  293,  293,  293,  293,  293,  293,  293,
 /*    10 */   293,  293,  293,  293,  293,  293,  293,  293,  293,  293,
 /*    20 */   293,  293,  293,  293,  293,  293,  293,  293,  293,  293,
 /*    30 */   293,  293,  293,  293,  293,  293,  293,  293,  293,  293,
 /*    40 */   293,  293,  293,  293,  293,  277,  293,  293,  293,  277,
 /*    50 */   293,  293,  293,  293,  293,  293,  293,  293,  237,  293,
 /*    60 */   202,  236,  235,  234,  233,  221,  220,  276,  275,  270,
 /*    70 */   293,  218,  261,  264,  262,  293,  260,  259,  258,  257,
 /*    80 */   256,  255,  293,  293,  293,  250,  251,  293,  293,  293,
 /*    90 */   293,  293,  293,  293,  293,  293,  293,  293,  293,  293,
 /*   100 */   293,  198,  293,  197,  293,  293,  203,  196,  293,  293,
 /*   110 */   293,  293,  293,  279,  279,  293,  293,  241,  215,  293,
 /*   120 */   195,  293,  193,  206,  207,  205,  204,  201,  199,  239,
 /*   130 */   240,  238,  232,  231,  230,  225,  224,  229,  228,  227,
 /*   140 */   226,  223,  222,  219,  268,  279,  278,  272,  271,  291,
 /*   150 */   292,  289,  280,  290,  288,  287,  285,  284,  286,  283,
 /*   160 */   282,  281,  274,  273,  269,  267,  266,  265,  263,  254,
 /*   170 */   253,  252,  249,  248,  247,  246,  245,  244,  243,  242,
 /*   180 */   241,  217,  216,  214,  213,  212,  211,  210,  209,  208,
 /*   190 */   200,  194,
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
  "SEMI",          "ID",            "PIPE",          "JOIN",        
  "WHERE",         "INTO",          "AS",            "PRESERVE",    
  "COMMA",         "SSTREAM",       "DQ_STRING",     "SHUFFLE",     
  "ALL",           "OSTREAM",       "FILTER",        "TRANSFORM",   
  "WINDOW_KW",     "LP",            "INT",           "ROWS",        
  "RP",            "SLIDE",         "SECONDS",       "MINUTES",     
  "HOURS",         "DAYS",          "GROUP",         "BY",          
  "REDUCE",        "SQ_STRING",     "DOUBLE",        "TRUE",        
  "FALSE",         "NUL",           "LCB",           "RCB",         
  "DOT_STAR",      "COLON",         "LSB",           "RSB",         
  "DOLLAR",        "DOT",           "error",         "commands",    
  "command",       "stream_cmd",    "filter_cmd",    "transform_cmd",
  "sliding_count_window_cmd",  "sliding_time_window_cmd",  "time_interval",  "group_cmd",   
  "reduce_cmd",    "group_expr",    "group_exprs",   "join_cmd",    
  "join_ds",       "join_ds_list",  "expr",          "pri_expr",    
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
 /*   9 */ "commands ::= join_cmd",
 /*  10 */ "join_cmd ::= JOIN join_ds_list WHERE expr INTO expr",
 /*  11 */ "join_ds ::= ID",
 /*  12 */ "join_ds ::= ID AS ID",
 /*  13 */ "join_ds ::= PRESERVE join_ds",
 /*  14 */ "join_ds_list ::= join_ds COMMA join_ds",
 /*  15 */ "join_ds_list ::= join_ds_list COMMA join_ds",
 /*  16 */ "command ::= stream_cmd",
 /*  17 */ "command ::= filter_cmd",
 /*  18 */ "command ::= transform_cmd",
 /*  19 */ "command ::= sliding_count_window_cmd",
 /*  20 */ "command ::= sliding_time_window_cmd",
 /*  21 */ "command ::= group_cmd",
 /*  22 */ "command ::= reduce_cmd",
 /*  23 */ "stream_cmd ::= SSTREAM DQ_STRING",
 /*  24 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA SHUFFLE",
 /*  25 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA ALL",
 /*  26 */ "stream_cmd ::= SSTREAM DQ_STRING COMMA expr",
 /*  27 */ "stream_cmd ::= OSTREAM DQ_STRING",
 /*  28 */ "filter_cmd ::= FILTER expr",
 /*  29 */ "transform_cmd ::= TRANSFORM expr",
 /*  30 */ "sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS RP",
 /*  31 */ "sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS SLIDE INT RP",
 /*  32 */ "sliding_time_window_cmd ::= WINDOW_KW LP time_interval RP",
 /*  33 */ "sliding_time_window_cmd ::= WINDOW_KW LP time_interval SLIDE time_interval RP",
 /*  34 */ "time_interval ::= INT SECONDS",
 /*  35 */ "time_interval ::= INT MINUTES",
 /*  36 */ "time_interval ::= INT HOURS",
 /*  37 */ "time_interval ::= INT DAYS",
 /*  38 */ "time_interval ::= time_interval INT HOURS",
 /*  39 */ "time_interval ::= time_interval INT MINUTES",
 /*  40 */ "time_interval ::= time_interval INT SECONDS",
 /*  41 */ "group_cmd ::= GROUP INTO expr",
 /*  42 */ "group_cmd ::= GROUP BY group_exprs INTO expr",
 /*  43 */ "reduce_cmd ::= REDUCE INTO expr",
 /*  44 */ "reduce_cmd ::= REDUCE BY group_exprs INTO expr",
 /*  45 */ "group_expr ::= expr",
 /*  46 */ "group_expr ::= expr AS ID",
 /*  47 */ "group_exprs ::= group_expr",
 /*  48 */ "group_exprs ::= group_exprs COMMA group_expr",
 /*  49 */ "expr ::= pri_expr",
 /*  50 */ "pri_expr ::= LP pri_expr RP",
 /*  51 */ "pri_expr ::= DQ_STRING",
 /*  52 */ "pri_expr ::= SQ_STRING",
 /*  53 */ "pri_expr ::= INT",
 /*  54 */ "pri_expr ::= DOUBLE",
 /*  55 */ "pri_expr ::= TRUE",
 /*  56 */ "pri_expr ::= FALSE",
 /*  57 */ "pri_expr ::= NUL",
 /*  58 */ "pri_expr ::= expr ADD expr",
 /*  59 */ "pri_expr ::= expr SUB expr",
 /*  60 */ "pri_expr ::= expr MUL expr",
 /*  61 */ "pri_expr ::= expr DIV expr",
 /*  62 */ "pri_expr ::= expr MOD expr",
 /*  63 */ "pri_expr ::= expr LT expr",
 /*  64 */ "pri_expr ::= expr GT expr",
 /*  65 */ "pri_expr ::= expr LE expr",
 /*  66 */ "pri_expr ::= expr GE expr",
 /*  67 */ "pri_expr ::= expr EQ expr",
 /*  68 */ "pri_expr ::= expr NE expr",
 /*  69 */ "pri_expr ::= expr OR expr",
 /*  70 */ "pri_expr ::= expr AND expr",
 /*  71 */ "pri_expr ::= NOT expr",
 /*  72 */ "pri_expr ::= expr IN expr",
 /*  73 */ "expr ::= record",
 /*  74 */ "record ::= LCB RCB",
 /*  75 */ "record ::= LCB fields RCB",
 /*  76 */ "fields ::= field",
 /*  77 */ "fields ::= fields COMMA field",
 /*  78 */ "field ::= field_name expr",
 /*  79 */ "field ::= path_expr DOT_STAR",
 /*  80 */ "field_name ::= ID COLON",
 /*  81 */ "expr ::= LSB RSB",
 /*  82 */ "expr ::= LSB exprs RSB",
 /*  83 */ "exprs ::= expr",
 /*  84 */ "exprs ::= exprs COMMA expr",
 /*  85 */ "expr ::= path_expr",
 /*  86 */ "path_expr ::= LP path_expr RP",
 /*  87 */ "path_expr ::= ID",
 /*  88 */ "path_expr ::= DOLLAR",
 /*  89 */ "path_expr ::= path_expr project_name",
 /*  90 */ "project_name ::= DOT ID",
 /*  91 */ "path_expr ::= path_expr LCB project_names RCB",
 /*  92 */ "path_expr ::= path_expr LCB MUL SUB project_names RCB",
 /*  93 */ "project_names ::= project_name",
 /*  94 */ "project_names ::= project_names COMMA project_name",
 /*  95 */ "path_expr ::= path_expr LSB expr RSB",
 /*  96 */ "path_expr ::= path_expr LSB expr COLON expr RSB",
 /*  97 */ "expr ::= func_expr",
 /*  98 */ "func_name ::= ID LP",
 /*  99 */ "func_expr ::= func_name RP",
 /* 100 */ "func_expr ::= func_name exprs RP",
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
  { 82, 1 },
  { 83, 2 },
  { 83, 3 },
  { 84, 1 },
  { 84, 3 },
  { 84, 3 },
  { 84, 5 },
  { 59, 1 },
  { 59, 3 },
  { 59, 1 },
  { 71, 6 },
  { 72, 1 },
  { 72, 3 },
  { 72, 2 },
  { 73, 3 },
  { 73, 3 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 61, 2 },
  { 61, 4 },
  { 61, 4 },
  { 61, 4 },
  { 61, 2 },
  { 62, 2 },
  { 63, 2 },
  { 64, 5 },
  { 64, 7 },
  { 65, 4 },
  { 65, 6 },
  { 66, 2 },
  { 66, 2 },
  { 66, 2 },
  { 66, 2 },
  { 66, 3 },
  { 66, 3 },
  { 66, 3 },
  { 67, 3 },
  { 67, 5 },
  { 68, 3 },
  { 68, 5 },
  { 69, 1 },
  { 69, 3 },
  { 70, 1 },
  { 70, 3 },
  { 74, 1 },
  { 75, 3 },
  { 75, 1 },
  { 75, 1 },
  { 75, 1 },
  { 75, 1 },
  { 75, 1 },
  { 75, 1 },
  { 75, 1 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 3 },
  { 75, 2 },
  { 75, 3 },
  { 74, 1 },
  { 85, 2 },
  { 85, 3 },
  { 86, 1 },
  { 86, 3 },
  { 87, 2 },
  { 87, 2 },
  { 88, 2 },
  { 74, 2 },
  { 74, 3 },
  { 76, 1 },
  { 76, 3 },
  { 74, 1 },
  { 77, 3 },
  { 77, 1 },
  { 77, 1 },
  { 77, 2 },
  { 80, 2 },
  { 77, 4 },
  { 77, 6 },
  { 81, 1 },
  { 81, 3 },
  { 77, 4 },
  { 77, 6 },
  { 74, 1 },
  { 78, 2 },
  { 79, 2 },
  { 79, 3 },
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
#line 75 "speparser.y"
{
      std::cout << "sth like 'c1->operator' is resolved." << std::endl;
      printCommands(yymsp[0].minor.yy129);  
      // g_pCommands = yymsp[0].minor.yy129;    

	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy129
	  ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == bPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  bPoint->addTargetPipeline(yymsp[0].minor.yy129);      
   }
#line 1093 "speparser.c"
        break;
      case 5: /* stmt ::= ID EQ commands */
#line 89 "speparser.y"
{
	  std::cout << "sth like 'c1 = operator->...' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy129); 
	  // g_pCommands = yymsp[0].minor.yy129;  

	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-2].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' already exists." << std::endl;		 
	  }	  
   }
#line 1108 "speparser.c"
        break;
      case 6: /* stmt ::= ID EQ ID PIPE commands */
#line 101 "speparser.y"
{
	  std::cout << "sth like 'c2 = c1->operator' is resolved." << std::endl;
	  printCommands(yymsp[0].minor.yy129);
	  // g_pCommands = yymsp[0].minor.yy129;  
	  
	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(yymsp[-4].minor.yy0.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << yymsp[-4].minor.yy0.z << "' already exists." << std::endl;		 
	  }
	  
	  // wire yymsp[-2].minor.yy0 to yymsp[0].minor.yy129
	  ConnectPointOperator* cPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);
	  if (NULL == cPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << yymsp[-2].minor.yy0.z << "' is not defined." << std::endl;		 	  
	  }
	  cPoint->addTargetPipeline(yymsp[0].minor.yy129);
   }
#line 1131 "speparser.c"
        break;
      case 7: /* commands ::= command */
#line 121 "speparser.y"
{
      yygotominor.yy129 = new vector<BaseOperator*>();
      yygotominor.yy129->push_back(yymsp[0].minor.yy56);
      
      // set current pipeline, add it into all pipelines
      pParseContext->curPipeline = yygotominor.yy129;
      pParseContext->allPipelines.push_back(yygotominor.yy129);
}
#line 1143 "speparser.c"
        break;
      case 8: /* commands ::= commands PIPE command */
#line 130 "speparser.y"
{
      yygotominor.yy129 = yymsp[-2].minor.yy129;
      yygotominor.yy129->push_back(yymsp[0].minor.yy56);
}
#line 1151 "speparser.c"
        break;
      case 9: /* commands ::= join_cmd */
#line 136 "speparser.y"
{
      // generate a proxy pipeline for each join datasource
      for (int i=0; i< yymsp[0].minor.yy149->getDatasSourceNum(); i++) {
         JoinInputProxyOperator* proxyOperator = new JoinInputProxyOperator(i, yymsp[0].minor.yy149->getDataSource(i)->getConnectPointName().c_str());
         proxyOperator->setJoinOperator(yymsp[0].minor.yy149);
         vector<BaseOperator*>* thePoxyPipeline = new vector<BaseOperator*>();
         thePoxyPipeline->push_back(proxyOperator);
         
         // wire the proxy pipeline together with the corresponding ConnectPoint
         ConnectPointOperator* theConnectPoint = pParseContext->getConnectPointOperator(proxyOperator->getConnectPointName().c_str());
         theConnectPoint->addTargetPipeline(thePoxyPipeline);    
         
         pParseContext->curPipeline = thePoxyPipeline;
         pParseContext->allPipelines.push_back(thePoxyPipeline);
      }
      
      yygotominor.yy129 = new vector<BaseOperator*>();
      yygotominor.yy129->push_back(yymsp[0].minor.yy149);
      
      // set current pipeline, add it into all pipelines
      pParseContext->curPipeline = yygotominor.yy129;
      pParseContext->allPipelines.push_back(yygotominor.yy129);      
}
#line 1178 "speparser.c"
        break;
      case 10: /* join_cmd ::= JOIN join_ds_list WHERE expr INTO expr */
#line 160 "speparser.y"
{
       yygotominor.yy149 = new JoinOperator(yymsp[-4].minor.yy143, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);       
}
#line 1185 "speparser.c"
        break;
      case 11: /* join_ds ::= ID */
#line 164 "speparser.y"
{
       ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(yymsp[0].minor.yy0.z);       
       yygotominor.yy14 = new JoinDataSource(yymsp[0].minor.yy0.z, yymsp[0].minor.yy0.z, false, !bPoint->isFromWindow());
}
#line 1193 "speparser.c"
        break;
      case 12: /* join_ds ::= ID AS ID */
#line 169 "speparser.y"
{
       ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(yymsp[-2].minor.yy0.z);       
       yygotominor.yy14 = new JoinDataSource(yymsp[-2].minor.yy0.z, yymsp[0].minor.yy0.z, false, !bPoint->isFromWindow());
}
#line 1201 "speparser.c"
        break;
      case 13: /* join_ds ::= PRESERVE join_ds */
#line 174 "speparser.y"
{
       yygotominor.yy14 = yymsp[0].minor.yy14;
       yygotominor.yy14->setPreserve(true);
}
#line 1209 "speparser.c"
        break;
      case 14: /* join_ds_list ::= join_ds COMMA join_ds */
#line 179 "speparser.y"
{
       yygotominor.yy143 = new vector<JoinDataSource*>();
       yygotominor.yy143->push_back(yymsp[-2].minor.yy14);
       yygotominor.yy143->push_back(yymsp[0].minor.yy14);
}
#line 1218 "speparser.c"
        break;
      case 15: /* join_ds_list ::= join_ds_list COMMA join_ds */
#line 185 "speparser.y"
{
       yygotominor.yy143 = yymsp[-2].minor.yy143;
       yygotominor.yy143->push_back(yymsp[0].minor.yy14);
}
#line 1226 "speparser.c"
        break;
      case 16: /* command ::= stream_cmd */
      case 17: /* command ::= filter_cmd */ yytestcase(yyruleno==17);
      case 18: /* command ::= transform_cmd */ yytestcase(yyruleno==18);
      case 19: /* command ::= sliding_count_window_cmd */ yytestcase(yyruleno==19);
      case 20: /* command ::= sliding_time_window_cmd */ yytestcase(yyruleno==20);
      case 21: /* command ::= group_cmd */ yytestcase(yyruleno==21);
      case 22: /* command ::= reduce_cmd */ yytestcase(yyruleno==22);
#line 190 "speparser.y"
{
      yygotominor.yy56 = yymsp[0].minor.yy56;
}
#line 1239 "speparser.c"
        break;
      case 23: /* stream_cmd ::= SSTREAM DQ_STRING */
#line 216 "speparser.y"
{
      yygotominor.yy56 = new StreamOperator(yymsp[0].minor.yy0.z, 0);
}
#line 1246 "speparser.c"
        break;
      case 24: /* stream_cmd ::= SSTREAM DQ_STRING COMMA SHUFFLE */
#line 220 "speparser.y"
{
      yygotominor.yy56 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionRoundrobin());
}
#line 1253 "speparser.c"
        break;
      case 25: /* stream_cmd ::= SSTREAM DQ_STRING COMMA ALL */
#line 224 "speparser.y"
{
      yygotominor.yy56 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionAll());
}
#line 1260 "speparser.c"
        break;
      case 26: /* stream_cmd ::= SSTREAM DQ_STRING COMMA expr */
#line 228 "speparser.y"
{
      yygotominor.yy56 = new StreamOperator(yymsp[-2].minor.yy0.z, 0, new PartitionPerValue(yymsp[0].minor.yy136));
}
#line 1267 "speparser.c"
        break;
      case 27: /* stream_cmd ::= OSTREAM DQ_STRING */
#line 232 "speparser.y"
{
      yygotominor.yy56 = new StreamOperator(yymsp[0].minor.yy0.z, 1);
}
#line 1274 "speparser.c"
        break;
      case 28: /* filter_cmd ::= FILTER expr */
#line 236 "speparser.y"
{
      yygotominor.yy56 = new FilterOperator(yymsp[0].minor.yy136);
}
#line 1281 "speparser.c"
        break;
      case 29: /* transform_cmd ::= TRANSFORM expr */
#line 240 "speparser.y"
{
	  yygotominor.yy56 = new TransformOperator(yymsp[0].minor.yy136);
}
#line 1288 "speparser.c"
        break;
      case 30: /* sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS RP */
#line 244 "speparser.y"
{
	yygotominor.yy56 = new SlidingCountWindowOperator(atoi(yymsp[-2].minor.yy0.z), 1);
}
#line 1295 "speparser.c"
        break;
      case 31: /* sliding_count_window_cmd ::= WINDOW_KW LP INT ROWS SLIDE INT RP */
#line 248 "speparser.y"
{
	yygotominor.yy56 = new SlidingCountWindowOperator(atoi(yymsp[-4].minor.yy0.z), atoi(yymsp[-1].minor.yy0.z));
}
#line 1302 "speparser.c"
        break;
      case 32: /* sliding_time_window_cmd ::= WINDOW_KW LP time_interval RP */
#line 252 "speparser.y"
{
	yygotominor.yy56 = new SlidingTimeWindowOperator(yymsp[-1].minor.yy141, 1);	
	if (yymsp[-1].minor.yy141 <= 0) {
		pParseContext->errorCode = -3;
		printf("Query Error: invalid time window size.\n ");	
	} 
}
#line 1313 "speparser.c"
        break;
      case 33: /* sliding_time_window_cmd ::= WINDOW_KW LP time_interval SLIDE time_interval RP */
#line 260 "speparser.y"
{
	yygotominor.yy56 = new SlidingTimeWindowOperator(yymsp[-3].minor.yy141, yymsp[-1].minor.yy141);
	if (yymsp[-3].minor.yy141 <= 0) {
		pParseContext->errorCode = -3;
		printf("Query Error: invalid time window size.\n ");	
	} 
	if (yymsp[-3].minor.yy141 <= 0) {
		pParseContext->errorCode = -4;
		printf("Query Error: invalid time window slide step.\n ");	
	} 			
}
#line 1328 "speparser.c"
        break;
      case 34: /* time_interval ::= INT SECONDS */
#line 272 "speparser.y"
{
    yygotominor.yy141 = atolong64(yymsp[-1].minor.yy0.z) * 1000 * 1000;
}
#line 1335 "speparser.c"
        break;
      case 35: /* time_interval ::= INT MINUTES */
#line 276 "speparser.y"
{
    yygotominor.yy141 = atolong64(yymsp[-1].minor.yy0.z) * 60 * 1000 * 1000;
}
#line 1342 "speparser.c"
        break;
      case 36: /* time_interval ::= INT HOURS */
#line 280 "speparser.y"
{
    yygotominor.yy141 = atolong64(yymsp[-1].minor.yy0.z) * 60* 60 * 1000 * 1000;
}
#line 1349 "speparser.c"
        break;
      case 37: /* time_interval ::= INT DAYS */
#line 284 "speparser.y"
{
    yygotominor.yy141 = atolong64(yymsp[-1].minor.yy0.z) * 24* 60* 60 * 1000 * 1000;
}
#line 1356 "speparser.c"
        break;
      case 38: /* time_interval ::= time_interval INT HOURS */
#line 288 "speparser.y"
{
	yygotominor.yy141 = yymsp[-2].minor.yy141 + atolong64(yymsp[-1].minor.yy0.z) * 60* 60 * 1000 * 1000;
}
#line 1363 "speparser.c"
        break;
      case 39: /* time_interval ::= time_interval INT MINUTES */
#line 292 "speparser.y"
{
	yygotominor.yy141 = yymsp[-2].minor.yy141 + atolong64(yymsp[-1].minor.yy0.z) * 60 * 1000 * 1000;
}
#line 1370 "speparser.c"
        break;
      case 40: /* time_interval ::= time_interval INT SECONDS */
#line 296 "speparser.y"
{
	yygotominor.yy141 = yymsp[-2].minor.yy141 + atolong64(yymsp[-1].minor.yy0.z) * 1000 * 1000;
}
#line 1377 "speparser.c"
        break;
      case 41: /* group_cmd ::= GROUP INTO expr */
#line 301 "speparser.y"
{
    yygotominor.yy56 = new GroupOperator(NULL, yymsp[0].minor.yy136);
}
#line 1384 "speparser.c"
        break;
      case 42: /* group_cmd ::= GROUP BY group_exprs INTO expr */
#line 305 "speparser.y"
{
    yygotominor.yy56 = new GroupOperator(yymsp[-2].minor.yy20, yymsp[0].minor.yy136);
}
#line 1391 "speparser.c"
        break;
      case 43: /* reduce_cmd ::= REDUCE INTO expr */
#line 309 "speparser.y"
{
    yygotominor.yy56 = new ReduceOperator(NULL, yymsp[0].minor.yy136);
}
#line 1398 "speparser.c"
        break;
      case 44: /* reduce_cmd ::= REDUCE BY group_exprs INTO expr */
#line 313 "speparser.y"
{
    yygotominor.yy56 = new ReduceOperator(yymsp[-2].minor.yy20, yymsp[0].minor.yy136);
}
#line 1405 "speparser.c"
        break;
      case 45: /* group_expr ::= expr */
#line 317 "speparser.y"
{
    yygotominor.yy175 = new GroupExpr(yymsp[0].minor.yy136, "");
}
#line 1412 "speparser.c"
        break;
      case 46: /* group_expr ::= expr AS ID */
#line 321 "speparser.y"
{
    yygotominor.yy175 = new GroupExpr(yymsp[-2].minor.yy136, yymsp[0].minor.yy0.z);
}
#line 1419 "speparser.c"
        break;
      case 47: /* group_exprs ::= group_expr */
#line 325 "speparser.y"
{
    yygotominor.yy20 = new vector<GroupExpr*>();
    yygotominor.yy20->push_back(yymsp[0].minor.yy175);
}
#line 1427 "speparser.c"
        break;
      case 48: /* group_exprs ::= group_exprs COMMA group_expr */
#line 330 "speparser.y"
{
    yymsp[-2].minor.yy20->push_back(yymsp[0].minor.yy175);
    yygotominor.yy20 = yymsp[-2].minor.yy20;
}
#line 1435 "speparser.c"
        break;
      case 49: /* expr ::= pri_expr */
      case 97: /* expr ::= func_expr */ yytestcase(yyruleno==97);
#line 335 "speparser.y"
{
	yygotominor.yy136 = yymsp[0].minor.yy136;
}
#line 1443 "speparser.c"
        break;
      case 50: /* pri_expr ::= LP pri_expr RP */
#line 339 "speparser.y"
{
      yygotominor.yy136 = yymsp[-1].minor.yy136;
}
#line 1450 "speparser.c"
        break;
      case 51: /* pri_expr ::= DQ_STRING */
      case 52: /* pri_expr ::= SQ_STRING */ yytestcase(yyruleno==52);
#line 343 "speparser.y"
{
      // strip off the quote char
      std::string s(yymsp[0].minor.yy0.z+1, strlen(yymsp[0].minor.yy0.z)-2);
      yygotominor.yy136 = new StringValue(s.c_str());
}
#line 1460 "speparser.c"
        break;
      case 53: /* pri_expr ::= INT */
#line 355 "speparser.y"
{
      yygotominor.yy136 = new IntValue(atoi(yymsp[0].minor.yy0.z));
}
#line 1467 "speparser.c"
        break;
      case 54: /* pri_expr ::= DOUBLE */
#line 359 "speparser.y"
{
      yygotominor.yy136 = new DoubleValue(atof(yymsp[0].minor.yy0.z));
}
#line 1474 "speparser.c"
        break;
      case 55: /* pri_expr ::= TRUE */
#line 363 "speparser.y"
{
      yygotominor.yy136 = new BoolValue(true);
}
#line 1481 "speparser.c"
        break;
      case 56: /* pri_expr ::= FALSE */
#line 367 "speparser.y"
{
      yygotominor.yy136 = new BoolValue(false);
}
#line 1488 "speparser.c"
        break;
      case 57: /* pri_expr ::= NUL */
#line 370 "speparser.y"
{
      yygotominor.yy136 = new NullValue();
}
#line 1495 "speparser.c"
        break;
      case 58: /* pri_expr ::= expr ADD expr */
#line 374 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_ADD, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1502 "speparser.c"
        break;
      case 59: /* pri_expr ::= expr SUB expr */
#line 378 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_SUB, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1509 "speparser.c"
        break;
      case 60: /* pri_expr ::= expr MUL expr */
#line 382 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_MUL, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1516 "speparser.c"
        break;
      case 61: /* pri_expr ::= expr DIV expr */
#line 385 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_DIV, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1523 "speparser.c"
        break;
      case 62: /* pri_expr ::= expr MOD expr */
#line 389 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_MOD, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1530 "speparser.c"
        break;
      case 63: /* pri_expr ::= expr LT expr */
#line 393 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_LT, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1537 "speparser.c"
        break;
      case 64: /* pri_expr ::= expr GT expr */
#line 397 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_GT, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1544 "speparser.c"
        break;
      case 65: /* pri_expr ::= expr LE expr */
#line 401 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_LE, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1551 "speparser.c"
        break;
      case 66: /* pri_expr ::= expr GE expr */
#line 405 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_GE, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1558 "speparser.c"
        break;
      case 67: /* pri_expr ::= expr EQ expr */
#line 409 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_EQ, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1565 "speparser.c"
        break;
      case 68: /* pri_expr ::= expr NE expr */
#line 413 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_NE, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1572 "speparser.c"
        break;
      case 69: /* pri_expr ::= expr OR expr */
#line 417 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_OR, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1579 "speparser.c"
        break;
      case 70: /* pri_expr ::= expr AND expr */
#line 421 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_AND, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1586 "speparser.c"
        break;
      case 71: /* pri_expr ::= NOT expr */
#line 425 "speparser.y"
{
      yygotominor.yy136 = new UnaryExpr(Expr::OP_NOT, yymsp[0].minor.yy136);
}
#line 1593 "speparser.c"
        break;
      case 72: /* pri_expr ::= expr IN expr */
#line 429 "speparser.y"
{
      yygotominor.yy136 = new BinaryExpr(Expr::OP_IN, yymsp[-2].minor.yy136, yymsp[0].minor.yy136);
}
#line 1600 "speparser.c"
        break;
      case 73: /* expr ::= record */
#line 438 "speparser.y"
{
	yygotominor.yy136 = yymsp[0].minor.yy55;
}
#line 1607 "speparser.c"
        break;
      case 74: /* record ::= LCB RCB */
#line 442 "speparser.y"
{
	yygotominor.yy55 = new RecordConstructExpr( new vector<RecordMemberExpr*>());
}
#line 1614 "speparser.c"
        break;
      case 75: /* record ::= LCB fields RCB */
#line 446 "speparser.y"
{
	yygotominor.yy55 = new RecordConstructExpr(yymsp[-1].minor.yy78);
}
#line 1621 "speparser.c"
        break;
      case 76: /* fields ::= field */
#line 450 "speparser.y"
{
	yygotominor.yy78 = new vector<RecordMemberExpr*>();
	yygotominor.yy78->push_back(yymsp[0].minor.yy49);
}
#line 1629 "speparser.c"
        break;
      case 77: /* fields ::= fields COMMA field */
#line 455 "speparser.y"
{
	yymsp[-2].minor.yy78->push_back(yymsp[0].minor.yy49);
	yygotominor.yy78 = yymsp[-2].minor.yy78;
}
#line 1637 "speparser.c"
        break;
      case 78: /* field ::= field_name expr */
#line 460 "speparser.y"
{
	yygotominor.yy49 = new RecordMemberExpr(yymsp[-1].minor.yy54->c_str(), yymsp[0].minor.yy136);
}
#line 1644 "speparser.c"
        break;
      case 79: /* field ::= path_expr DOT_STAR */
#line 464 "speparser.y"
{
	yygotominor.yy49 = new RecordMemberExpr("*", yymsp[-1].minor.yy87);
}
#line 1651 "speparser.c"
        break;
      case 80: /* field_name ::= ID COLON */
#line 468 "speparser.y"
{
	yygotominor.yy54 = new string(yymsp[-1].minor.yy0.z);
}
#line 1658 "speparser.c"
        break;
      case 81: /* expr ::= LSB RSB */
#line 472 "speparser.y"
{
    yygotominor.yy136 = new ArrayConstructExpr();
}
#line 1665 "speparser.c"
        break;
      case 82: /* expr ::= LSB exprs RSB */
#line 476 "speparser.y"
{
	yygotominor.yy136 = new ArrayConstructExpr(yymsp[-1].minor.yy127);
}
#line 1672 "speparser.c"
        break;
      case 83: /* exprs ::= expr */
#line 480 "speparser.y"
{
	yygotominor.yy127 = new vector<Expr*>();
	yygotominor.yy127->push_back(yymsp[0].minor.yy136);
}
#line 1680 "speparser.c"
        break;
      case 84: /* exprs ::= exprs COMMA expr */
#line 485 "speparser.y"
{
	yygotominor.yy127 = yymsp[-2].minor.yy127;
	yygotominor.yy127->push_back(yymsp[0].minor.yy136);
}
#line 1688 "speparser.c"
        break;
      case 85: /* expr ::= path_expr */
#line 490 "speparser.y"
{
      yygotominor.yy136 = yymsp[0].minor.yy87;
}
#line 1695 "speparser.c"
        break;
      case 86: /* path_expr ::= LP path_expr RP */
#line 494 "speparser.y"
{
	  yygotominor.yy87 = yymsp[-1].minor.yy87;
}
#line 1702 "speparser.c"
        break;
      case 87: /* path_expr ::= ID */
#line 498 "speparser.y"
{
      yygotominor.yy87 = new VarRefExpr(yymsp[0].minor.yy0.z);
}
#line 1709 "speparser.c"
        break;
      case 88: /* path_expr ::= DOLLAR */
#line 501 "speparser.y"
{
      yygotominor.yy87 = new VarRefExpr("$");
}
#line 1716 "speparser.c"
        break;
      case 89: /* path_expr ::= path_expr project_name */
#line 505 "speparser.y"
{
      yymsp[-1].minor.yy87->addChildPathExpr(new StepExpr(yymsp[0].minor.yy54->c_str()) );
      yygotominor.yy87 = yymsp[-1].minor.yy87;
      delete yymsp[0].minor.yy54;
}
#line 1725 "speparser.c"
        break;
      case 90: /* project_name ::= DOT ID */
#line 511 "speparser.y"
{
      yygotominor.yy54 = new string(yymsp[0].minor.yy0.z);
}
#line 1732 "speparser.c"
        break;
      case 91: /* path_expr ::= path_expr LCB project_names RCB */
#line 515 "speparser.y"
{
	  yymsp[-3].minor.yy87->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy5, false));
	  yygotominor.yy87 = yymsp[-3].minor.yy87;
}
#line 1740 "speparser.c"
        break;
      case 92: /* path_expr ::= path_expr LCB MUL SUB project_names RCB */
#line 520 "speparser.y"
{
	  yymsp[-5].minor.yy87->addChildPathExpr(new ProjectExpr(yymsp[-1].minor.yy5, true));
	  yygotominor.yy87 = yymsp[-5].minor.yy87;
}
#line 1748 "speparser.c"
        break;
      case 93: /* project_names ::= project_name */
#line 525 "speparser.y"
{
      yygotominor.yy5 = new vector<string>();
      yygotominor.yy5->push_back(string(yymsp[0].minor.yy54->c_str()));
      delete yymsp[0].minor.yy54; 
}
#line 1757 "speparser.c"
        break;
      case 94: /* project_names ::= project_names COMMA project_name */
#line 530 "speparser.y"
{
      yymsp[-2].minor.yy5->push_back(string(yymsp[0].minor.yy54->c_str()));
      yygotominor.yy5 = yymsp[-2].minor.yy5;
      delete yymsp[0].minor.yy54;
}
#line 1766 "speparser.c"
        break;
      case 95: /* path_expr ::= path_expr LSB expr RSB */
#line 536 "speparser.y"
{
	yymsp[-3].minor.yy87->addChildPathExpr(new ArrayElementsExpr(yymsp[-1].minor.yy136,NULL));
	yygotominor.yy87 = yymsp[-3].minor.yy87;	  
}
#line 1774 "speparser.c"
        break;
      case 96: /* path_expr ::= path_expr LSB expr COLON expr RSB */
#line 541 "speparser.y"
{  
	yymsp[-5].minor.yy87->addChildPathExpr(new ArrayElementsExpr(yymsp[-3].minor.yy136,yymsp[-1].minor.yy136));
	yygotominor.yy87 = yymsp[-5].minor.yy87;
}
#line 1782 "speparser.c"
        break;
      case 98: /* func_name ::= ID LP */
#line 551 "speparser.y"
{
	yygotominor.yy54 = new string(yymsp[-1].minor.yy0.z);
	printf("recongnize a function %s.\n ", yygotominor.yy54->c_str());
}
#line 1790 "speparser.c"
        break;
      case 99: /* func_expr ::= func_name RP */
#line 556 "speparser.y"
{
    vector<Expr*>* args = new vector<Expr*>();
	yygotominor.yy136 = createBuiltinFunction(yymsp[-1].minor.yy54->c_str(), args);
	if (!yygotominor.yy136) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-1].minor.yy54->c_str());
	}
	delete yymsp[-1].minor.yy54;
}
#line 1803 "speparser.c"
        break;
      case 100: /* func_expr ::= func_name exprs RP */
#line 566 "speparser.y"
{
	yygotominor.yy136 = createBuiltinFunction(yymsp[-2].minor.yy54->c_str(), yymsp[-1].minor.yy127);
	if (!yygotominor.yy136) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", yymsp[-2].minor.yy54->c_str());
	}
	delete yymsp[-2].minor.yy54;
}
#line 1815 "speparser.c"
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
#line 65 "speparser.y"

  /* The tokenizer always gives us a token */
  std::cout << "near at the Token (line #" << TOKEN.line << ", column #" << TOKEN.column << "), Syntax error!" << std::endl;
  pParseContext->errorCode = -1;
#line 1886 "speparser.c"
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
