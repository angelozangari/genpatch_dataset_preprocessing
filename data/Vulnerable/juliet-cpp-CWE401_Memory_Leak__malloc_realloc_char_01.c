/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE401_Memory_Leak__malloc_realloc_char_01.c
Label Definition File: CWE401_Memory_Leak__malloc_realloc.label.xml
Template File: point-flaw-01.tmpl.c
*/
/*
 * @description
 * CWE: 401 Memory Leak
 * Sinks:
 *    GoodSink: Ensure the memory block pointed to by data is always freed
 *    BadSink : malloc() and use then realloc() and use data before free()
 * Flow Variant: 01 Baseline
 *
 * */

#include "std_testcase.h"

#ifndef _WIN32
#include <wchar.h>
#endif

#ifndef OMITBAD

void CWE401_Memory_Leak__malloc_realloc_char_01_bad()
{
    {
        char * data = (char *)malloc(100*sizeof(char));
        if (data == NULL) {exit(-1);}
        /* Initialize and make use of data */
        strcpy(data, "A String");
        printLine(data);
        /* FLAW: If realloc() fails, the initial memory block will not be freed() */
        data = (char *)realloc(data, (130000)*sizeof(char));
        if (data != NULL)
        {
            /* Reinitialize and make use of data */
            strcpy(data, "New String");
            printLine(data);
            free(data);
        }
    }
}

#endif /* OMITBAD */

#ifndef OMITGOOD

static void good1()
{
    {
        char * data = (char *)malloc(100*sizeof(char));
        if (data == NULL) {exit(-1);}
        char * tmpData;
        /* Initialize and make use of data */
        strcpy(data, "A String");
        printLine(data);
        tmpData = (char *)realloc(data, (130000)*sizeof(char));
        /* FIX: Ensure realloc() was successful before assigning data to the memory block
        * allocated with realloc() */
        if (tmpData != NULL)
        {
            data = tmpData;
            /* Reinitialize and make use of data */
            strcpy(data, "New String");
            printLine(data);
        }
        free(data);
    }
}

void CWE401_Memory_Leak__malloc_realloc_char_01_good()
{
    good1();
}

#endif /* OMITGOOD */

/* Below is the main(). It is only used when building this testcase on
   its own for testing or for building a binary to use in testing binary
   analysis tools. It is not used when compiling all the testcases as one
   application, which is how source code analysis tools are tested. */

#ifdef INCLUDEMAIN

int main(int argc, char * argv[])
{
    /* seed randomness */
    srand( (unsigned)time(NULL) );
#ifndef OMITGOOD
    printLine("Calling good()...");
    CWE401_Memory_Leak__malloc_realloc_char_01_good();
    printLine("Finished good()");
#endif /* OMITGOOD */
#ifndef OMITBAD
    printLine("Calling bad()...");
    CWE401_Memory_Leak__malloc_realloc_char_01_bad();
    printLine("Finished bad()");
#endif /* OMITBAD */
    return 0;
}

#endif

//						↓↓↓VULNERABLE LINES↓↓↓

// 30,8;30,14

// 37,12;37,18

