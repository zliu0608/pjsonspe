%include {
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
	
}

%token_type {Token}
%default_type {Token}
%extra_argument {ParseContext* pParseContext}

%type commands {vector<BaseOperator*> *}
%type command {BaseOperator*}
%type stream_cmd {BaseOperator*}
%type filter_cmd {BaseOperator*}
%type transform_cmd {BaseOperator*}
%type sliding_count_window_cmd {BaseOperator*}
%type sliding_time_window_cmd  {BaseOperator*}
%type time_interval {long64}
%type group_cmd {BaseOperator*}
%type reduce_cmd {BaseOperator*}
%type group_expr {GroupExpr*}
%type group_exprs {vector<GroupExpr*>*}
%type expr {Expr*}
%type pri_expr {Expr*}
%type exprs {vector<Expr*>*}
%type path_expr {PathExpr*}
%type func_name {string*}
%type func_expr {Expr*}
%type project_name {string*}
%type project_names {vector<string>*}


%left OR.
%left AND IN.
%left LT GT LE GE EQ NE.
%left ADD SUB.
%left MUL DIV MOD.
%right NOT.

%syntax_error {
  /* The tokenizer always gives us a token */
  std::cout << "near at the Token (line #" << TOKEN.line << ", column #" << TOKEN.column << "), Syntax error!" << std::endl;
  pParseContext->errorCode = -1;
}

main ::= epn.
epn ::= stmt SEMI.
epn ::= epn stmt SEMI.
stmt ::= commands.
stmt ::= ID(B) PIPE commands(C). {
      std::cout << "sth like 'c1->operator' is resolved." << std::endl;
      printCommands(C);  
      // g_pCommands = C;    

	  // wire B to C
	  ConnectPointOperator* bPoint = pParseContext->getConnectPointOperator(B.z);
	  if (NULL == bPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << B.z << "' is not defined." << std::endl;		 	  
	  }
	  bPoint->addTargetPipeline(C);      
   }

stmt ::= ID(B) EQ commands(C). {
	  std::cout << "sth like 'c1 = operator->...' is resolved." << std::endl;
	  printCommands(C); 
	  // g_pCommands = C;  

	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(B.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << B.z << "' already exists." << std::endl;		 
	  }	  
   }
   
stmt ::= ID(B) EQ ID(C) PIPE commands(D). {
	  std::cout << "sth like 'c2 = c1->operator' is resolved." << std::endl;
	  printCommands(D);
	  // g_pCommands = D;  
	  
	  // make a ConnectPoint operator, add it into the pipeline
	  if ( NULL == pParseContext->addConnectPointOperator(B.z)) {	   
		 pParseContext->errorCode = -10;
		 std::cout << "Connect operator '" << B.z << "' already exists." << std::endl;		 
	  }
	  
	  // wire C to D
	  ConnectPointOperator* cPoint = pParseContext->getConnectPointOperator(C.z);
	  if (NULL == cPoint) {
		 pParseContext->errorCode = -11;
		 std::cout << "Connect operator '" << C.z << "' is not defined." << std::endl;		 	  
	  }
	  cPoint->addTargetPipeline(D);
   }

commands(A) ::= command(B). {
      A = new vector<BaseOperator*>();
      A->push_back(B);
      
      // set current pipeline, add it into all pipelines
      pParseContext->curPipeline = A;
      pParseContext->allPipelines.push_back(A);
}

commands(A) ::= commands(B) PIPE command(C). {
      A = B;
      A->push_back(C);
}

command(A) ::= stream_cmd(B). {
      A = B;
}
command(A) ::= filter_cmd(B). {
      A = B;
}
command(A) ::= transform_cmd(B). {
      A = B;
}

command(A) ::=sliding_count_window_cmd(B). {
      A = B;
}

command(A) ::=sliding_time_window_cmd(B). {
      A = B;
}

command(A) ::= group_cmd(B). {
      A = B;
}

command(A) ::= reduce_cmd(B). {
      A = B;
}

stream_cmd(A) ::= SSTREAM DQ_STRING(B). {
      A = new StreamOperator(B.z, 0);
}

stream_cmd(A) ::= SSTREAM DQ_STRING(B) COMMA SHUFFLE. {
      A = new StreamOperator(B.z, 0, new PartitionRoundrobin());
}

stream_cmd(A) ::= SSTREAM DQ_STRING(B) COMMA ALL. {
      A = new StreamOperator(B.z, 0, new PartitionAll());
}

stream_cmd(A) ::= SSTREAM DQ_STRING(B) COMMA expr(C). {
      A = new StreamOperator(B.z, 0, new PartitionPerValue(C));
}

stream_cmd(A) ::= OSTREAM DQ_STRING(B). {
      A = new StreamOperator(B.z, 1);
}

filter_cmd(A) ::= FILTER expr(B). {
      A = new FilterOperator(B);
}

transform_cmd(A) ::= TRANSFORM expr(B). {
	  A = new TransformOperator(B);
}

sliding_count_window_cmd(A) ::= WINDOW_KW LP INT(B) ROWS RP. {
	A = new SlidingCountWindowOperator(atoi(B.z), 1);
}

sliding_count_window_cmd(A) ::= WINDOW_KW LP INT(B) ROWS SLIDE INT(C) RP. {
	A = new SlidingCountWindowOperator(atoi(B.z), atoi(C.z));
}

sliding_time_window_cmd(A) ::= WINDOW_KW LP time_interval(B) RP. {
	A = new SlidingTimeWindowOperator(B, 1);	
	if (B <= 0) {
		pParseContext->errorCode = -3;
		printf("Query Error: invalid time window size.\n ");	
	} 
}

sliding_time_window_cmd(A) ::= WINDOW_KW LP time_interval(B) SLIDE time_interval(C) RP. {
	A = new SlidingTimeWindowOperator(B, C);
	if (B <= 0) {
		pParseContext->errorCode = -3;
		printf("Query Error: invalid time window size.\n ");	
	} 
	if (B <= 0) {
		pParseContext->errorCode = -4;
		printf("Query Error: invalid time window slide step.\n ");	
	} 			
}

time_interval(A) ::= INT(B) SECONDS. {
    A = atolong64(B.z) * 1000 * 1000;
}

time_interval(A) ::= INT(B) MINUTES. {
    A = atolong64(B.z) * 60 * 1000 * 1000;
}

time_interval(A) ::= INT(B) HOURS. {
    A = atolong64(B.z) * 60* 60 * 1000 * 1000;
}

time_interval(A) ::= INT(B) DAYS. {
    A = atolong64(B.z) * 24* 60* 60 * 1000 * 1000;
}

time_interval(A) ::= time_interval(B) INT(C) HOURS. {
	A = B + atolong64(C.z) * 60* 60 * 1000 * 1000;
}

time_interval(A) ::= time_interval(B) INT(C) MINUTES. {
	A = B + atolong64(C.z) * 60 * 1000 * 1000;
}

time_interval(A) ::= time_interval(B) INT(C) SECONDS. {
	A = B + atolong64(C.z) * 1000 * 1000;
}


group_cmd(A) ::= GROUP INTO expr(B). {
    A = new GroupOperator(NULL, B);
}

group_cmd(A) ::= GROUP BY group_exprs(B) INTO expr(C). {
    A = new GroupOperator(B, C);
}

reduce_cmd(A) ::= REDUCE INTO expr(B). {
    A = new ReduceOperator(NULL, B);
}

reduce_cmd(A) ::= REDUCE BY group_exprs(B) INTO expr(C). {
    A = new ReduceOperator(B, C);
}

group_expr(A) ::= expr(B). {
    A = new GroupExpr(B, "");
}

group_expr(A) ::= expr(B) AS ID(C). {
    A = new GroupExpr(B, C.z);
}

group_exprs(A) ::= group_expr(B). {
    A = new vector<GroupExpr*>();
    A->push_back(B);
}

group_exprs(A) ::= group_exprs(B) COMMA group_expr(C). {
    B->push_back(C);
    A = B;
}

expr(A) ::= pri_expr(B). {
	A = B;
}

pri_expr(A) ::=  LP pri_expr(B) RP. {
      A = B;
}

pri_expr(A) ::= DQ_STRING(B). {
      // strip off the quote char
      std::string s(B.z+1, strlen(B.z)-2);
      A = new StringValue(s.c_str());
}

pri_expr(A) ::= SQ_STRING(B). {
      // strip off the quote char
      std::string s(B.z+1, strlen(B.z)-2);
      A = new StringValue(s.c_str());
}

pri_expr(A) ::= INT(B). {
      A = new IntValue(atoi(B.z));
}

pri_expr(A) ::= DOUBLE(B). {
      A = new DoubleValue(atof(B.z));
}

pri_expr(A) ::= TRUE. {
      A = new BoolValue(true);
}

pri_expr(A) ::= FALSE. {
      A = new BoolValue(false);
}
pri_expr(A) ::= NUL. {
      A = new NullValue();
}

pri_expr(A) ::= expr(B) ADD expr(C). {
      A = new BinaryExpr(Expr::OP_ADD, B, C);
}

pri_expr(A) ::= expr(B) SUB expr(C). {
      A = new BinaryExpr(Expr::OP_SUB, B, C);
}

pri_expr(A) ::= expr(B) MUL expr(C). {
      A = new BinaryExpr(Expr::OP_MUL, B, C);
}
pri_expr(A) ::= expr(B) DIV expr(C). {
      A = new BinaryExpr(Expr::OP_DIV, B, C);
}

pri_expr(A) ::= expr(B) MOD expr(C). {
      A = new BinaryExpr(Expr::OP_MOD, B, C);
}

pri_expr(A) ::= expr(B) LT expr(C). {
      A = new BinaryExpr(Expr::OP_LT, B, C);
}

pri_expr(A) ::= expr(B) GT expr(C). {
      A = new BinaryExpr(Expr::OP_GT, B, C);
} 

pri_expr(A) ::= expr(B) LE expr(C). {
      A = new BinaryExpr(Expr::OP_LE, B, C);
}
 
pri_expr(A) ::= expr(B) GE expr(C). {
      A = new BinaryExpr(Expr::OP_GE, B, C);
}

pri_expr(A) ::= expr(B) EQ expr(C). {
      A = new BinaryExpr(Expr::OP_EQ, B, C);
} 

pri_expr(A) ::= expr(B) NE expr(C). {
      A = new BinaryExpr(Expr::OP_NE, B, C);
} 

pri_expr(A) ::= expr(B) OR expr(C).{
      A = new BinaryExpr(Expr::OP_OR, B, C);
}

pri_expr(A) ::= expr(B) AND expr(C). {
      A = new BinaryExpr(Expr::OP_AND, B, C);
}

pri_expr(A) ::= NOT expr(B). {
      A = new UnaryExpr(Expr::OP_NOT, B);
}

pri_expr(A) ::= expr(B) IN expr(C). {
      A = new BinaryExpr(Expr::OP_IN, B, C);
}

%type record {RecordConstructExpr*}
%type fields {vector<RecordMemberExpr*>*}
%type field {RecordMemberExpr*}
%type field_name {string*}

expr(A) ::= record(B). {
	A = B;
}

record(A) ::= LCB RCB. {
	A = new RecordConstructExpr( new vector<RecordMemberExpr*>());
}

record(A) ::= LCB fields(B) RCB. {
	A = new RecordConstructExpr(B);
}

fields(A) ::= field(B). {
	A = new vector<RecordMemberExpr*>();
	A->push_back(B);
}

fields(A) ::= fields(B) COMMA field(C). {
	B->push_back(C);
	A = B;
}

field(A)  ::= field_name(B) expr(C). {
	A = new RecordMemberExpr(B->c_str(), C);
}

field(A)  ::= path_expr(B) DOT_STAR. {
	A = new RecordMemberExpr("*", B);
}

field_name(A) ::= ID(B) COLON. {
	A = new string(B.z);
}

expr(A) ::= LSB RSB. {
    A = new ArrayConstructExpr();
}

expr(A) ::= LSB exprs(B) RSB. {
	A = new ArrayConstructExpr(B);
}

exprs(A) ::= expr(B). {
	A = new vector<Expr*>();
	A->push_back(B);
}

exprs(A) ::= exprs(B) COMMA expr(C). {
	A = B;
	A->push_back(C);
}

expr(A) ::= path_expr(B). {
      A = B;
}

path_expr(A) ::= LP path_expr(B) RP. {
	  A = B;
}

path_expr(A) ::= ID(B). {
      A = new VarRefExpr(B.z);
}
path_expr(A) ::= DOLLAR(B). {
      A = new VarRefExpr("$");
}

path_expr(A) ::= path_expr(B) project_name(C). {
      B->addChildPathExpr(new StepExpr(C->c_str()) );
      A = B;
      delete C;
}

project_name(A) ::= DOT ID(B). {
      A = new string(B.z);
}

path_expr(A) ::= path_expr(B) LCB project_names(C) RCB. {
	  B->addChildPathExpr(new ProjectExpr(C, false));
	  A = B;
}

path_expr(A) ::= path_expr(B) LCB MUL SUB project_names(C) RCB. {
	  B->addChildPathExpr(new ProjectExpr(C, true));
	  A = B;
}

project_names(A) ::= project_name(B). {
      A = new vector<string>();
      A->push_back(string(B->c_str()));
      delete B; 
}
project_names(A) ::= project_names(B) COMMA project_name(C). {
      B->push_back(string(C->c_str()));
      A = B;
      delete C;
}

path_expr(A) ::= path_expr(B) LSB expr(C) RSB. {
	B->addChildPathExpr(new ArrayElementsExpr(C,NULL));
	A = B;	  
}

path_expr(A) ::= path_expr(B) LSB expr(C) COLON expr(D) RSB. {  
	B->addChildPathExpr(new ArrayElementsExpr(C,D));
	A = B;
}


expr(A) ::= func_expr(B). {
	A = B;
}

func_name(A) ::= ID(B) LP. {
	A = new string(B.z);
	printf("recongnize a function %s.\n ", A->c_str());
}

func_expr(A) ::= func_name(B) RP. {
    vector<Expr*>* args = new vector<Expr*>();
	A = createBuiltinFunction(B->c_str(), args);
	if (!A) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", B->c_str());
	}
	delete B;
}

func_expr(A) ::= func_name(B) exprs(C) RP. {
	A = createBuiltinFunction(B->c_str(), C);
	if (!A) {
		pParseContext->errorCode = -2;
		printf("Query Error: no such function %s.\n ", B->c_str());
	}
	delete B;
}