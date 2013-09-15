%{
#include <math.h>
#include <string.h>
#include "lexglobal.h"
#include "speparser.h"

int mylineno = 1;
int mycolumnno = 1;

YYSTYPE yylval;

 __inline void copyTerminalStr() {
	 strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
	 yylval.sval[TOKEN_MAX_LEN] = 0;
 };

//%option c++
//%option prefix="My"
%}

%option noyywrap


dqstring    \"[^\n"]+\"
sqstring    \'[^\n']+\'
ws        [ \t]+
digit     [0-9]
alpha     [A-Za-z]
int       [-+]?{digit}+
double    [+-]?(({digit}*\.{digit}+)([eE][-+]?{digit}+)?)
id        {alpha}[A-Za-z0-9_]*

%x AFTER_DOT
%%
{ws}     {/* skip blanks and tabs */
            mycolumnno += yyleng;
         }


{int}    {
			mycolumnno += yyleng;
		    // yylval.lval = atol(yytext);
		    strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
			  yylval.sval[TOKEN_MAX_LEN] = 0;
		    return INT;
         }
         
         
{double} {
			mycolumnno += yyleng;
		    // yylval.fval = atof(yytext);
		    strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
			  yylval.sval[TOKEN_MAX_LEN] = 0;
		    return DOUBLE;
         }   

{dqstring} {
			mycolumnno += yyleng;
			strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
			yylval.sval[TOKEN_MAX_LEN] = 0;
			return DQ_STRING;
         }
         
{sqstring} {
			mycolumnno += yyleng;
			strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
			yylval.sval[TOKEN_MAX_LEN] = 0;
			return SQ_STRING;
         }         

<AFTER_DOT>"input_stream" |
<AFTER_DOT>"ouput_stream" |
<AFTER_DOT>"filter" |
<AFTER_DOT>"true" |
<AFTER_DOT>"false" |
<AFTER_DOT>"in"  {
			// printf("*** after DOT\n");
            mycolumnno += yyleng;
            strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
			yylval.sval[TOKEN_MAX_LEN] = 0;
			BEGIN(INITIAL);
            return ID;
         }  

"input_stream" {mycolumnno += yyleng; copyTerminalStr(); return SSTREAM; }
				
"ouput_stream" {mycolumnno += yyleng; copyTerminalStr(); return OSTREAM; }         
				
"filter"       {mycolumnno += yyleng; copyTerminalStr(); return FILTER; }

"transform"    {mycolumnno += yyleng; copyTerminalStr(); return TRANSFORM; }

"window"       {mycolumnno += yyleng; copyTerminalStr(); return WINDOW_KW; }
"row"          {mycolumnno += yyleng; return ROWS; }
"rows"         {mycolumnno += yyleng; return ROWS; }
"slide"        {mycolumnno += yyleng; return SLIDE;}

"second"       {mycolumnno += yyleng; return SECONDS; }
"seconds"      {mycolumnno += yyleng; return SECONDS; }
"minute"       {mycolumnno += yyleng; return MINUTES; }
"minutes"      {mycolumnno += yyleng; return MINUTES; }
"hour"         {mycolumnno += yyleng; return HOURS; }
"hours"        {mycolumnno += yyleng; return HOURS; }
"day"          {mycolumnno += yyleng; return DAYS; }
"days"         {mycolumnno += yyleng; return DAYS; }

"group"        {mycolumnno += yyleng; return GROUP;}
"by"           {mycolumnno += yyleng; return BY;}
"into"         {mycolumnno += yyleng; return INTO;}

"reduce"       {mycolumnno += yyleng; return REDUCE;}

"join"         {mycolumnno += yyleng; return JOIN;}
"where"        {mycolumnno += yyleng; return WHERE;}
"as"           {mycolumnno += yyleng; return AS;}
"preserve"     {mycolumnno += yyleng; return PRESERVE;}

"shuffle"      {mycolumnno += yyleng; return SHUFFLE;}
"all"          {mycolumnno += yyleng; return ALL;}
                
"true"   { mycolumnno += yyleng; return TRUE; }
"false"  { mycolumnno += yyleng; return FALSE; }
"null"   { mycolumnno += yyleng; return NUL; }
         
"+"      { mycolumnno += yyleng; return ADD; }         
"-"      { mycolumnno += yyleng; return SUB; }         
"*"      { mycolumnno += yyleng; return MUL; }  
"/"      { mycolumnno += yyleng; return DIV; }
"%"      { mycolumnno += yyleng; return MOD; } 

"and"    { mycolumnno += yyleng; return AND; } 
"or"     { mycolumnno += yyleng; return OR; }
"not"    { mycolumnno += yyleng; return NOT; }
"in"     { mycolumnno += yyleng; return IN; }
"<"      { mycolumnno += yyleng; return LT; }
">"      { mycolumnno += yyleng; return GT; }
"<="     { mycolumnno += yyleng; return LE; }
">="     { mycolumnno += yyleng; return GE; }
"="      { mycolumnno += yyleng; return EQ; }
"!="     { mycolumnno += yyleng; return NE; } 

"("      { mycolumnno += yyleng; return LP; }
")"      { mycolumnno += yyleng; return RP; }  
"{"      { mycolumnno += yyleng; return LCB; }
"}"      { mycolumnno += yyleng; return RCB; }  
"["      { mycolumnno += yyleng; return LSB; }
"]"      { mycolumnno += yyleng; return RSB; }

";"      { mycolumnno += yyleng; return SEMI; }
":"      { mycolumnno += yyleng; return COLON; }
","      { mycolumnno += yyleng; return COMMA; }
".*"     { mycolumnno += yyleng; return DOT_STAR; }
"."      { mycolumnno += yyleng; BEGIN(AFTER_DOT); return DOT; }

"$"      { mycolumnno += yyleng; return DOLLAR;}

"->"     { mycolumnno += yyleng; return PIPE; }

\n       {mylineno++; mycolumnno=1; }
         
<INITIAL,AFTER_DOT>{id}     {
            mycolumnno += yyleng;
            strncpy(yylval.sval, yytext, TOKEN_MAX_LEN);
			yylval.sval[TOKEN_MAX_LEN] = 0;
			BEGIN(INITIAL);
            return ID;
         }  
%%

/**
 * yyerror() is invoked when the lexer or parser encounter
 * an error.  The error message is passed via *s
 *
 */
void yyerror(char* s) {
    printf("error: %s at line: %d\n", s, mylineno);
}