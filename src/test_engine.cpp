// #include "speparser.h"
#include "tokendef.h"
#include "lexglobal.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;


    void* ParseAlloc(void* (*allocProc)(size_t));
    void Parse(void*, int, Token, ParseContext* pParseContext);
    void ParseFree(void *p, void (*freeProc)(void*));

extern "C" {
    // if the generated lexer is in c, then it should be 
    // enclosed in extern "C" declaration.
    typedef struct yy_buffer_state *YY_BUFFER_STATE;
    int yylex(void);
    YY_BUFFER_STATE yy_scan_string( const char * );
    void yy_delete_buffer( YY_BUFFER_STATE );
    extern YYSTYPE yylval;
    extern int mylineno;
    extern int mycolumnno;
}

int doparse(const string& s, ParseContext& parseContext) {
   
    YY_BUFFER_STATE bufstate = yy_scan_string(s.c_str());
    
    struct Token t0;
    // struct ParseContext parseContext;
    parseContext.errorCode = 0;
    void* pParser = ParseAlloc(malloc);

    mylineno = 1;
    mycolumnno = 1;

    vector<Token*> tokens;
    int lexCode = 0;
    while ((lexCode = yylex()) !=0) {
        Token* token = new Token();
        strcpy(token->s , yylval.sval);
        token->z = token->s ;
        token->line = mylineno;
        token->column = mycolumnno;

        // cout << "code = " << lexCode << ", z = " << token->z << endl;
        Parse(pParser, lexCode, *token, &parseContext);
        tokens.push_back(token);
        if (parseContext.errorCode != 0)
            break;
    }   
    yy_delete_buffer(bufstate);
    Parse(pParser, 0, t0, &parseContext);
    ParseFree(pParser, free);

    // free tokens
    for (vector<Token*>::const_iterator iter = tokens.begin(); iter<tokens.end(); iter++) {
        delete (*iter);
    }
    tokens.clear();
    return parseContext.errorCode;
}

// global variables defintiion
vector<BaseOperator*> * g_pCommands;
RbSourceStreamManager g_rbSSMgr;


// print a list of operators
void pa(vector<BaseOperator*>* pList) {
    if (pList == NULL)
       return;

    for (int i=0; i< pList->size(); i++) {
       cout <<"op[" << i << "] = " << (*pList)[i]->toString().c_str() << endl;
    }
};

void addEvents(StreamOperator* pInputStream, int num) {
    char evt[] = " { \"symbol\" : \"IBM\", \"value\" : 110 , \"quantity\" : 2000 } "; 
    char temp[] = " { \"symbol\" : \"%s\", \"value\" : %d , \"quantity\" : 2000 } "; 
    char* jt;

    for (int i= 1; i <= num; i++) {
        Event* pe;
       
       jt = (char*) malloc(200);
       sprintf(jt, temp, "IBM", i);
       //printf("json= %s\n", jt);
       pe = new Event(jt);
       pInputStream->addInputEvent(pe);     
    }
};


/**
 * a simple filter case:  compare 2 fields. 
 * input event size : 268 bytes.
 * events are pushed into ring buffer backed source stream.
 */
int test_filter01 () {
    char ss[256];
    printf("want to debug? you can attache the process and process enter to continue\n");
    gets(ss);

    // parse this query    
    // string s("s1 = input_stream \"ss_1\" -> filter ($.usr.is in [100000, 300000, 500000]  and $.evt.er = '4') -> ouput_stream  \"os_01\" ; ");
    string s("s1 = input_stream \"ss_1\" -> filter ($.usr.is % 20000 = 1 or (abs($.hdr.in) = 1 and ($.evt.bsmc in [1,0,32,33,255] or $.evt.ccc in [38, 42, 34, 111, 88, 102, 57, 2, 17, 98, 63])) )-> ouput_stream  \"os_01\" ; ");
    
    cout << s << endl;
    ParseContext parseCtx;   
    doparse(s, parseCtx);

    CommandsExecutor queryWorker(g_pCommands);
    queryWorker.prepare();

    string inputStreamName = "\"ss_1\"";
    RbSourceStream* pStream = g_rbSSMgr.getStream(inputStreamName);

    // event template    
    // a tuk event template, 268 chars
    // char eventTemplate[] = "{\"hdr\":{\"tn\":\"-201859242\",\"ts1\":\"1353491350\",\"ts2\":\"1353491370\",\"in\":\"2\"},\"usr\":{\"is\":%16lld,\"ie\":\"353486049348281\",\"ms\":\"447872420974\"},\"loc\":{\"l1\":\"518\",\"r1\":\"160\",\"c\":\"65535\",\"l\":\"518\",\"r\":\"160\"},\"evt\":{\"cn\":\"873283548\",\"ty\":\"10\",\"ev\":\"0\",\"er\":\"4\"}}";
    
    // 200 chars,  
    // param1: %in, (%in = 1)      5%
    // param2: %is, (%is %100 = 1) 1%
    // param3: %ev  
    char eventTemplate[] = "{\"hdr\": {\"tn\": \"1000000000\",\"ts1\": \"1350316243\",\"ts2\": \"1350316269\",\"in\": %lld},\"usr\": {\"is\": %16lld}, \"loc\": {\"c\": 18751,\"s\": 13588, \"l\": 35390}, \"evt\": {\"ty\": 2, \"ev\": %d, \"ccc\":12, \"bsmc\": 35}}";
    char jsonText [300];

    queryWorker.start();
    for (long l =0; l < 500000; l++) {
        for (int i=0; i<100; i++) {
            long k = l*100L + i;
            sprintf(jsonText, eventTemplate, (k % 20000), k, k);   
           // if (k % 200000 == 1) {
           //     printf("****: %s\n", jsonText);
           // }
            pStream->publish(jsonText);
        }
    }
    
    printf("press enter to stop. \n");
    gets(ss);

    queryWorker.stop();
    return 0;
};


/**
 * a simple filter case:  compare 2 fields. 
 * input event size : 268 bytes.
 * events are pushed into ring buffer backed source stream.
 */
int test_filter01_parallel (int nParallel) {
    char ss[256];
    printf("want to debug? you can attache the process and process enter to continue\n");
    gets(ss);

    // parse this query    
    // string s("s1 = input_stream \"ss_1\" -> filter ($.usr.is in [100000, 300000, 500000]  and $.evt.er = '4') -> ouput_stream  \"os_01\" ; ");
    string s("s1 = input_stream \"ss_1\" -> filter ($.usr.is % 200000 = 1  or ($.hdr.in = 1 and ($.evt.bsmc in [1,0,32,33,255] or $.evt.ccc in [38, 42, 34, 111, 88, 102, 57, 2, 17, 98, 63])) )-> ouput_stream  \"os_01\" ; ");
    // string s("s1 = input_stream \"ss_1\" -> filter ($.usr.is % 200000 in [10, 11, 12, 14, 15 ] ) -> ouput_stream  \"os_01\" ; ");
    cout << s << endl;
    ParseContext parseCtx;   
    if (0 != doparse(s, parseCtx) ) {
        // there are errors in the query
        return 1;
    }

    ThreadedCommandsExecutor queryWorker(&(parseCtx.allPipelines), nParallel);
    queryWorker.doStaticAnalysis();
    queryWorker.prepare();

    string inputStreamName = "\"ss_1\"";
    int streamId = queryWorker.getStreamId(inputStreamName);


    // event template    
    // a tuk event template, 268 chars
    // char eventTemplate[] = "{\"hdr\":{\"tn\":\"-201859242\",\"ts1\":\"1353491350\",\"ts2\":\"1353491370\",\"in\":\"2\"},\"usr\":{\"is\":%16lld,\"ie\":\"353486049348281\",\"ms\":\"447872420974\"},\"loc\":{\"l1\":\"518\",\"r1\":\"160\",\"c\":\"65535\",\"l\":\"518\",\"r\":\"160\"},\"evt\":{\"cn\":\"873283548\",\"ty\":\"10\",\"ev\":\"0\",\"er\":\"4\"}}";
    
    // 200 chars,  
    // param1: %in, (%in = 1)      5%
    // param2: %is, (%is %100 = 1) 1%
    // param3: %ev  
    char eventTemplate[] = "{\"hdr\": {\"tn\": \"1000000000\",\"ts1\": \"1350316243\",\"ts2\": \"1350316269\",\"in\": %lld},\"usr\": {\"is\": %16lld}, \"loc\": {\"c\": 18751,\"s\": 13588, \"l\": 35390}, \"evt\": {\"ty\": 2, \"ev\": %d, \"ccc\":12, \"bsmc\": 35}}";
    // char eventTemplate[] = "{\"hdr\": {\"in\": %lld},\"usr\": {\"is\": %16lld}, \"evt\": {\"ty\": 2, \"ev\": %d, \"ccc\":12, \"bsmc\": 35}}";
    char jsonText [300];
    char* jsonTextArray[100];
    sprintf(jsonText, eventTemplate, 0, 0, 0);
    for (int i =0 ; i< 100; i++) {
        jsonTextArray[i] = jsonText;
    }

    char* param1 = strstr(eventTemplate, "%lld");
    char* param2 = strstr(param1, "%16lld");
    char* param3 = strstr(param2, "%d");
    size_t lastSubstrLen = strlen(eventTemplate) - (param3 + 2 - eventTemplate);


    queryWorker.start();
    for (long l =0; l < 500000; l++) {
        for (int i=0; i<100; i++) {
            long k = l*100L + i;
            // sprintf is too slow to generate enough workloads
            // I have to manually concatenate the string
            // sprintf(jsonText, eventTemplate, (k % 20000), k, k);
            
            size_t pos = param1 - eventTemplate ; 
            strncpy(jsonText, eventTemplate, pos);
            pos += sprintf(jsonText + pos, "%lld", (k % 20000));
            strncpy(jsonText + pos, param1 + 4, param2 - param1 - 4);
            pos += param2 - param1 - 4;
            pos += sprintf(jsonText + pos, "%16lld", k);
            strncpy(jsonText + pos, param2 + 6, param3 - param2 - 6);
            pos += param3 - param2 - 6;
            pos += sprintf(jsonText + pos, "%d", k);
            strncpy(jsonText + pos, param3 + 2, lastSubstrLen + 1);            
            

            // if (k % 200000 == 1) {
            //     printf("****: %s\n", jsonText);
            // }            

             queryWorker.publishEvent(streamId, jsonText);
            // pStream->publish(jsonTextArray, 10);
            // pStream->publish(jsonTextArray[0]);
        }
    }
    
    printf("press enter to stop. \n");
    gets(ss);

    queryWorker.stop();
    return 0;
};

/**
 *  The simple filter case run in single thread mode.
 *  events are stored inside a buffer within stream operator
 */
int test_filter01_pullmode() {
    char ss[256];
    printf("want to debug? you can attache the process and process enter to continue\n");
    gets(ss);

    // parse this query
    string s("s1 = input_stream \"ss_1\" -> filter ($.usr.is = 100000 and $.evt.er = '4') -> ouput_stream  \"os_01\" ; ");
    cout << s << endl;

    ParseContext parseCtx;   
    doparse(s, parseCtx);

    CommandsExecutor queryWorker(g_pCommands);
    queryWorker.prepare();
    StreamOperator* pInputStream = static_cast<StreamOperator*>((*g_pCommands)[0]);


    int batch = 100;
    long loop = 10000;
    char temp[] = "{\"hdr\":{\"tn\":\"-201859242\",\"ts1\":\"1353491350\",\"ts2\":\"1353491370\",\"in\":\"2\"},\"usr\":{\"is\":%16lld,\"ie\":\"353486049348281\",\"ms\":\"447872420974\"},\"loc\":{\"l1\":\"518\",\"r1\":\"160\",\"c\":\"65535\",\"l\":\"518\",\"r\":\"160\"},\"evt\":{\"cn\":\"873283548\",\"ty\":\"10\",\"ev\":\"0\",\"er\":\"4\"}}";
    char** jt = (char**) malloc(batch * sizeof(char*));
    Event* eventBuffer = new Event[batch];

    for (int i=0; i< batch; i++) {
        jt[i]= (char*) malloc(300);
    }
    char ssa[300];
    int64_t t0 = currentMicroSeconds();
    for (long l=0; l<loop; l++) {
        //addEvents(pInputStream, 101);        
        for (int i= 0; i < batch; i++) {
           // sprintf(jt[i], temp, "IBM", i+102);
           sprintf(ssa, temp, l*batch + i);
           
           Event* pe = eventBuffer + i;
           // int errCode = pe->update(jt[i]);
            int errCode = pe->update(ssa);
           if (0 != errCode) {
               printf("error code = %d \n", errCode);
              return errCode;
           }
           
           // Event* pe = new Event(jt[i]);
           pInputStream->addInputEvent(pe);     
        }

          queryWorker.execute();   
    }
    int64_t t1 = currentMicroSeconds();
    printf("processed %9d events in %10.2f s\n", loop*batch, (t1-t0)/1000000.0);

    delete[] eventBuffer;
    return 0;
};


/**
 * a simple filter case with following functions 
 *  1) abs
 * input event size : 268 bytes.
 * events are pushed into ring buffer backed source stream.
 */
int test_functions () {
    char ss[256];
    printf("want to debug? you can attache the process and process enter to continue\n");
    gets(ss);

    string s("s1 = input_stream \"ss_1\" -> filter (abs($.usr.is % 20000) = 1 or (abs($.hdr.in) = 1 and ($.evt.bsmc in [1,0,32,33,255] or $.evt.ccc in [38, 42, 34, 111, 88, 102, 57, 2, 17, 98, 63])) )-> ouput_stream  \"os_01\" ; ");
    
    cout << s << endl;
    ParseContext parseCtx;   
    doparse(s, parseCtx);

    ThreadedCommandsExecutor queryWorker(&(parseCtx.allPipelines), 1);     
    queryWorker.prepare();

    string inputStreamName = "\"ss_1\"";
    RbSourceStream* pStream = g_rbSSMgr.getStream(inputStreamName);

    // event template    
    // a tuk event template, 268 chars
    // char eventTemplate[] = "{\"hdr\":{\"tn\":\"-201859242\",\"ts1\":\"1353491350\",\"ts2\":\"1353491370\",\"in\":\"2\"},\"usr\":{\"is\":%16lld,\"ie\":\"353486049348281\",\"ms\":\"447872420974\"},\"loc\":{\"l1\":\"518\",\"r1\":\"160\",\"c\":\"65535\",\"l\":\"518\",\"r\":\"160\"},\"evt\":{\"cn\":\"873283548\",\"ty\":\"10\",\"ev\":\"0\",\"er\":\"4\"}}";
    
    // 200 chars,  
    // param1: %in, (%in = 1)      5%
    // param2: %is, (%is %100 = 1) 1%
    // param3: %ev  
    char eventTemplate[] = "{\"hdr\": {\"tn\": \"1000000000\",\"ts1\": \"1350316243\",\"ts2\": \"1350316269\",\"in\": %ld},\"usr\": {\"is\": %ld}, \"loc\": {\"c\": 18751,\"s\": 13588, \"l\": 35390}, \"evt\": {\"ty\": 2, \"ev\": %ld, \"ccc\":12, \"bsmc\": 35}}";
    char jsonText [300];

    queryWorker.start();
    for (long l =0; l < 5000; l++) {
        for (int i=0; i<100; i++) {
            long k = l*100L + i;
            sprintf(jsonText, eventTemplate, (k % 20000), k * (-1L), k);   
           // if (k % 200000 == 1) {
           //     printf("****: %s\n", jsonText);
           // }
            pStream->publish(jsonText);
        }
    }
    
    printf("press enter to stop. \n");
    gets(ss);

    queryWorker.stop();
    return 0;
};


/**
 * a simple filter case with following functions,  and then do transformation 
 *  1) abs
 * input event size : 268 bytes.
 * events are pushed into ring buffer backed source stream.
 */
int test_functions_transform () {
    char ss[256];
    printf("want to debug? you can attache the process and process enter to continue\n");
    gets(ss);

    string s("s1 = input_stream \"ss_1\" -> filter (abs($.usr.is % 20000) = 1 or (abs($.hdr.in) = 1 and ($.evt.bsmc in [1,0,32,33,255] or $.evt.ccc in [38, 42, 34, 111, 88, 102, 57, 2, 17, 98, 63])) )-> transform {userinfo: $.usr, event: $.evt} -> ouput_stream  \"os_01\" ; ");
    
    cout << s << endl;

    ParseContext parseCtx;   
    if (0 != doparse(s, parseCtx) ) {
        // there are errors in the query
        return 1;
    }

    ThreadedCommandsExecutor queryWorker(&(parseCtx.allPipelines), 1);
    queryWorker.prepare();

    string inputStreamName = "\"ss_1\"";
    int streamId = queryWorker.getStreamId(inputStreamName);

    // event template    
    // a tuk event template, 268 chars
    // char eventTemplate[] = "{\"hdr\":{\"tn\":\"-201859242\",\"ts1\":\"1353491350\",\"ts2\":\"1353491370\",\"in\":\"2\"},\"usr\":{\"is\":%16lld,\"ie\":\"353486049348281\",\"ms\":\"447872420974\"},\"loc\":{\"l1\":\"518\",\"r1\":\"160\",\"c\":\"65535\",\"l\":\"518\",\"r\":\"160\"},\"evt\":{\"cn\":\"873283548\",\"ty\":\"10\",\"ev\":\"0\",\"er\":\"4\"}}";
    
    // 200 chars,  
    // param1: %in, (%in = 1)      5%
    // param2: %is, (%is %100 = 1) 1%
    // param3: %ev  
    char eventTemplate[] = "{\"hdr\": {\"tn\": \"1000000000\",\"ts1\": \"1350316243\",\"ts2\": \"1350316269\",\"in\": %ld},\"usr\": {\"is\": %ld}, \"loc\": {\"c\": 18751,\"s\": 13588, \"l\": 35390}, \"evt\": {\"ty\": 2, \"ev\": %ld, \"ccc\":12, \"bsmc\": 35}}";
    char jsonText [300];

    queryWorker.start();
    for (long l =0; l < 5000; l++) {
        for (int i=0; i<100; i++) {
            long k = l*100L + i;
            sprintf(jsonText, eventTemplate, (k % 20000), k * (-1L), k);   
           // if (k % 200000 == 1) {
           //     printf("****: %s\n", jsonText);
           // }
            queryWorker.publishEvent(streamId, jsonText);
        }
    }
    
    printf("press enter to stop. \n");
    gets(ss);

    queryWorker.stop();
    return 0;
};

int main () {
    int parallelism, nitems;
    printf("Enter a number for this query's parallelism: ");

    nitems = scanf("%d", &parallelism);
    if (nitems == EOF || nitems == 0) {        
        printf("About to exist as you didn't enter a valid number. \n");
    } else {
        // test_filter01();      // ?
        test_filter01_parallel(parallelism);  // ok
        // test_filter01_pullmode(); // ?
        // test_functions ();    // ?
        // test_functions_transform();  // ok

        char ss[256];
        printf("after debug,  enter to continue\n");
        gets(ss);
    }

    return 0;

}