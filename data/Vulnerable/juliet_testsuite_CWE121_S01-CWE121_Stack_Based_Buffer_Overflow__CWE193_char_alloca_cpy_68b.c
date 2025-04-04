/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68b.c
Label Definition File: CWE121_Stack_Based_Buffer_Overflow__CWE193.label.xml
Template File: sources-sink-68b.tmpl.c
*/
/*
 * @description
 * CWE: 121 Stack Based Buffer Overflow
 * BadSource:  Point data to a buffer that does not have space for a NULL terminator
 * GoodSource: Point data to a buffer that includes space for a NULL terminator
 * Sink: cpy
 *    BadSink : Copy string to data using strcpy()
 * Flow Variant: 68 Data flow: data passed as a global variable from one function to another in different source files
 *
 * */

#include "std_testcase.h"

#ifndef _WIN32
#include <wchar.h>
#endif

/* MAINTENANCE NOTE: The length of this string should equal the 10 */
#define SRC_STRING "AAAAAAAAAA"

extern char * CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68_badData;
extern char * CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68_goodG2BData;

/* all the sinks are the same, we just want to know where the hit originated if a tool flags one */

#ifndef OMITBAD

void CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68b_badSink()
{
    char * data = CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68_badData;
    {
        char source[10+1] = SRC_STRING;
        /* POTENTIAL FLAW: data may not have enough space to hold source */
        strcpy(data, source);
        printLine(data);
    }
}

#endif /* OMITBAD */

#ifndef OMITGOOD

/* goodG2B uses the GoodSource with the BadSink */
void CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68b_goodG2BSink()
{
    char * data = CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_68_goodG2BData;
    {
        char source[10+1] = SRC_STRING;
        /* POTENTIAL FLAW: data may not have enough space to hold source */
        strcpy(data, source);
        printLine(data);
    }
}

#endif /* OMITGOOD */

//						↓↓↓VULNERABLE LINES↓↓↓

// 39,8;39,14

// 55,8;55,14

