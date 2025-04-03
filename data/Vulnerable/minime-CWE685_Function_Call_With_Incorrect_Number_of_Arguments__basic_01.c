/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE685_Function_Call_With_Incorrect_Number_of_Arguments__basic_01.c
Label Definition File: CWE685_Function_Call_With_Incorrect_Number_of_Arguments__basic.label.xml
Template File: point-flaw-01.tmpl.c
*/
/*
 * @description
 * CWE: 685 Function Call With Incorrect Number of Arguments
 * Sinks:
 *    GoodSink: Use the correct number of arguments
 *    BadSink : Incorrect number of arguments
 * Flow Variant: 01 Baseline
 *
 * */

#include "std_testcase.h"

#define DEST_SIZE 100 /* maintenance note: ensure this is > 2*SOURCE_STRING to avoid buffer overflow issues */
#define SOURCE_STRING "AAA"

#ifndef OMITBAD

void CWE685_Function_Call_With_Incorrect_Number_of_Arguments__basic_01_bad()
{
    {
        char dest[DEST_SIZE];
        /* FLAW: Incorrect number of arguments */
        sprintf(dest, "%s %s", SOURCE_STRING);
        printLine(dest);
    }
}

#endif /* OMITBAD */

#ifndef OMITGOOD

static void good1()
{
    {
        char dest[DEST_SIZE];
        /* FIX: Use the correct number of arguments */
        sprintf(dest, "%s %s", SOURCE_STRING, SOURCE_STRING);
        printLine(dest);
    }
}

void CWE685_Function_Call_With_Incorrect_Number_of_Arguments__basic_01_good()
{
    good1();
}

#endif /* OMITGOOD */

//						↓↓↓VULNERABLE LINES↓↓↓

// 28,8;28,15

// 42,8;42,15

