/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE475_Undefined_Behavior_for_Input_to_API__char_01.c
Label Definition File: CWE475_Undefined_Behavior_for_Input_to_API.label.xml
Template File: point-flaw-01.tmpl.c
*/
/*
 * @description
 * CWE: 475 Undefined Behavior for Input to API
 * Sinks:
 *    GoodSink: Copy overlapping memory regions using memmove()
 *    BadSink : Copy overlapping memory regions using memcpy()
 * Flow Variant: 01 Baseline
 *
 * */

#include "std_testcase.h"

#include <wchar.h>

#ifndef OMITBAD

void CWE475_Undefined_Behavior_for_Input_to_API__char_01_bad()
{
    {
        char dataBuffer[100] = "";
        char * data = dataBuffer;
        strcpy(data, "abcdefghijklmnopqrstuvwxyz");
        /* FLAW: Copy overlapping memory regions using memcpy() for which the result is undefined */
        memcpy(data + 6, data + 4, 10*sizeof(char));
        printLine(data);
    }
}

#endif /* OMITBAD */

#ifndef OMITGOOD

static void good1()
{
    {
        char dataBuffer[100] = "";
        char * data = dataBuffer;
        strcpy(data, "abcdefghijklmnopqrstuvwxyz");
        /* FIX: Copy overlapping memory regions using memmove() */
        memmove(data + 6, data + 4, 10*sizeof(char));
        printLine(data);
    }
}

void CWE475_Undefined_Behavior_for_Input_to_API__char_01_good()
{
    good1();
}

#endif /* OMITGOOD */

//						↓↓↓VULNERABLE LINES↓↓↓

// 27,8;27,14

// 43,8;43,14

