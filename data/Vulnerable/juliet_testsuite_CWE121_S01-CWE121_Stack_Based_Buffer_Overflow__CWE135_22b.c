/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE121_Stack_Based_Buffer_Overflow__CWE135_22b.c
Label Definition File: CWE121_Stack_Based_Buffer_Overflow__CWE135.label.xml
Template File: sources-sinks-22b.tmpl.c
*/
/*
 * @description
 * CWE: 121 Stack Based Buffer Overflow
 * BadSource:  Void pointer to a wchar_t array
 * GoodSource: Void pointer to a char array
 * Sinks:
 *    GoodSink: Allocate memory using wcslen() and copy data
 *    BadSink : Allocate memory using strlen() and copy data
 * Flow Variant: 22 Control flow: Flow controlled by value of a global variable. Sink functions are in a separate file from sources.
 *
 * */

#include "std_testcase.h"

#include <wchar.h>

#ifndef OMITBAD

/* The global variable below is used to drive control flow in the sink function */
extern int CWE121_Stack_Based_Buffer_Overflow__CWE135_22_badGlobal;

void CWE121_Stack_Based_Buffer_Overflow__CWE135_22_badSink(void * data)
{
    if(CWE121_Stack_Based_Buffer_Overflow__CWE135_22_badGlobal)
    {
        {
            /* POTENTIAL FLAW: treating pointer as a char* when it may point to a wide string */
            size_t dataLen = strlen((char *)data);
            void * dest = (void *)ALLOCA((dataLen+1) * sizeof(wchar_t));
            (void)wcscpy(dest, data);
            printLine((char *)dest);
        }
    }
}

#endif /* OMITBAD */

#ifndef OMITGOOD

/* The global variables below are used to drive control flow in the sink functions. */
extern int CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodB2G1Global;
extern int CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodB2G2Global;
extern int CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodG2BGlobal;

/* goodB2G1() - use badsource and goodsink by setting the static variable to false instead of true */
void CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodB2G1Sink(void * data)
{
    if(CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodB2G1Global)
    {
        /* INCIDENTAL: CWE 561 Dead Code, the code below will never run */
        printLine("Benign, fixed string");
    }
    else
    {
        {
            /* FIX: treating pointer like a wchar_t*  */
            size_t dataLen = wcslen((wchar_t *)data);
            void * dest = (void *)ALLOCA((dataLen+1) * sizeof(wchar_t));
            (void)wcscpy(dest, data);
            printWLine((wchar_t *)dest);
        }
    }
}

/* goodB2G2() - use badsource and goodsink by reversing the blocks in the if in the sink function */
void CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodB2G2Sink(void * data)
{
    if(CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodB2G2Global)
    {
        {
            /* FIX: treating pointer like a wchar_t*  */
            size_t dataLen = wcslen((wchar_t *)data);
            void * dest = (void *)ALLOCA((dataLen+1) * sizeof(wchar_t));
            (void)wcscpy(dest, data);
            printWLine((wchar_t *)dest);
        }
    }
}

/* goodG2B() - use goodsource and badsink */
void CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodG2BSink(void * data)
{
    if(CWE121_Stack_Based_Buffer_Overflow__CWE135_22_goodG2BGlobal)
    {
        {
            /* POTENTIAL FLAW: treating pointer as a char* when it may point to a wide string */
            size_t dataLen = strlen((char *)data);
            void * dest = (void *)ALLOCA((dataLen+1) * 1);
            (void)strcpy(dest, data);
            printLine((char *)dest);
        }
    }
}

#endif /* OMITGOOD */

//						↓↓↓VULNERABLE LINES↓↓↓

// 94,18;94,24

