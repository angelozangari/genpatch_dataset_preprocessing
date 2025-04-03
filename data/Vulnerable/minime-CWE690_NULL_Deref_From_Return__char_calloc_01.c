/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE690_NULL_Deref_From_Return__char_calloc_01.c
Label Definition File: CWE690_NULL_Deref_From_Return.free.label.xml
Template File: source-sinks-01.tmpl.c
*/
/*
 * @description
 * CWE: 690 Unchecked Return Value To NULL Pointer
 * BadSource: calloc Allocate data using calloc()
 * Sinks:
 *    GoodSink: Check to see if the data allocation failed and if not, use data
 *    BadSink : Don't check for NULL and use data
 * Flow Variant: 01 Baseline
 *
 * */

#include "std_testcase.h"

#include <wchar.h>

#ifndef OMITBAD

void CWE690_NULL_Deref_From_Return__char_calloc_01_bad()
{
    char * data;
    data = NULL; /* Initialize data */
    /* POTENTIAL FLAW: Allocate memory without checking if the memory allocation function failed */
    data = (char *)calloc(20, sizeof(char));
    /* FLAW: Initialize memory buffer without checking to see if the memory allocation function failed */
    strcpy(data, "Initialize");
    printLine(data);
    free(data);
}

#endif /* OMITBAD */

#ifndef OMITGOOD

/* goodB2G() uses the BadSource with the GoodSink */
static void goodB2G()
{
    char * data;
    data = NULL; /* Initialize data */
    /* POTENTIAL FLAW: Allocate memory without checking if the memory allocation function failed */
    data = (char *)calloc(20, sizeof(char));
    /* FIX: Check to see if the memory allocation function was successful before initializing the memory buffer */
    if (data != NULL)
    {
        strcpy(data, "Initialize");
        printLine(data);
        free(data);
    }
}

void CWE690_NULL_Deref_From_Return__char_calloc_01_good()
{
    goodB2G();
}

#endif /* OMITGOOD */

//						↓↓↓VULNERABLE LINES↓↓↓

// 30,4;30,10

// 49,8;49,14

