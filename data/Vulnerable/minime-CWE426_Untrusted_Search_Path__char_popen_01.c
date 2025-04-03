/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE426_Untrusted_Search_Path__char_popen_01.c
Label Definition File: CWE426_Untrusted_Search_Path.label.xml
Template File: sources-sink-01.tmpl.c
*/
/*
 * @description
 * CWE: 426 Untrusted Search Path
 * BadSource:  Don't specify the full path in the OS command
 * GoodSource: Specify the full path in the OS command
 * Sink: popen
 *    BadSink : Execute the popen function
 * Flow Variant: 01 Baseline
 *
 * */

#include "std_testcase.h"

#include <wchar.h>
#define BAD_OS_COMMAND "ls -la"
#define GOOD_OS_COMMAND "/usr/bin/ls -la"

#ifndef OMITBAD

void CWE426_Untrusted_Search_Path__char_popen_01_bad()
{
    char * data;
    char dataBuffer[100] = "";
    data = dataBuffer;
    /* FLAW: the full path is not specified */
    strcpy(data, BAD_OS_COMMAND);
    {
        FILE *pipe;
        /* POTENTIAL FLAW: Executing the popen() function without specifying the full path to the executable
         * can allow an attacker to run their own program */
        pipe = popen(data, "wb");
        if (pipe != NULL)
        {
            pclose(pipe);
        }
    }
}

#endif /* OMITBAD */

#ifndef OMITGOOD

/* goodG2B uses the GoodSource with the BadSink */
static void goodG2B()
{
    char * data;
    char dataBuffer[100] = "";
    data = dataBuffer;
    /* FIX: full path is specified */
    strcpy(data, GOOD_OS_COMMAND);
    {
        FILE *pipe;
        /* POTENTIAL FLAW: Executing the popen() function without specifying the full path to the executable
         * can allow an attacker to run their own program */
        pipe = popen(data, "wb");
        if (pipe != NULL)
        {
            pclose(pipe);
        }
    }
}

void CWE426_Untrusted_Search_Path__char_popen_01_good()
{
    goodG2B();
}

#endif /* OMITGOOD */

//						↓↓↓VULNERABLE LINES↓↓↓

// 31,4;31,10

// 55,4;55,10

