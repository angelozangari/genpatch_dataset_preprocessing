/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67b.c
Label Definition File: CWE121_Stack_Based_Buffer_Overflow__CWE193.label.xml
Template File: sources-sink-67b.tmpl.c
*/
/*
 * @description
 * CWE: 121 Stack Based Buffer Overflow
 * BadSource:  Point data to a buffer that does not have space for a NULL terminator
 * GoodSource: Point data to a buffer that includes space for a NULL terminator
 * Sinks: cpy
 *    BadSink : Copy string to data using strcpy()
 * Flow Variant: 67 Data flow: data passed in a struct from one function to another in different source files
 *
 * */

#include "std_testcase.h"

#ifndef _WIN32
#include <wchar.h>
#endif

/* MAINTENANCE NOTE: The length of this string should equal the 10 */
#define SRC_STRING "AAAAAAAAAA"

typedef struct _CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67_structType
{
    char * structFirst;
} CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67_structType;

#ifndef OMITBAD

void CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67b_badSink(CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67_structType myStruct)
{
    char * data = myStruct.structFirst;
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
void CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67b_goodG2BSink(CWE121_Stack_Based_Buffer_Overflow__CWE193_char_alloca_cpy_67_structType myStruct)
{
    char * data = myStruct.structFirst;
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

