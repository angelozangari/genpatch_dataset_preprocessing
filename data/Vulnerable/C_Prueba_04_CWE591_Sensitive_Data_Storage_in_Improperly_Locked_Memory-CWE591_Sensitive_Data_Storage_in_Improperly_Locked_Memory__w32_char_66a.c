/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66a.c
Label Definition File: CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32.label.xml
Template File: sources-sink-66a.tmpl.c
*/
/*
 * @description
 * CWE: 591 Sensitive Data Storage in Improperly Locked Memory
 * BadSource:  Allocate memory for sensitive data without using VirtualLock() to lock the buffer into memory
 * GoodSource: Allocate memory for sensitive data and use VirtualLock() to lock the buffer into memory
 * Sinks:
 *    BadSink : Authenticate the user using LogonUserA()
 * Flow Variant: 66 Data flow: data passed in an array from one function to another in different source files
 *
 * */

#include "std_testcase.h"

#include <wchar.h>
#include <windows.h>

#pragma comment(lib, "advapi32.lib")

#ifndef OMITBAD

/* bad function declaration */
void CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66b_badSink(char * passwordArray[]);

void CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66_bad()
{
    char * password;
    char * passwordArray[5];
    /* Initialize Data */
    password = "";
    password = (char *)malloc(100*sizeof(char));
    if (password == NULL)
    {
        printLine("Memory could not be allocated");
        exit(1);
    }
    /* FLAW: Do not lock the memory */
    /* INCIDENTAL FLAW: CWE-259 Hardcoded Password */
    strcpy(password, "Password1234!");
    /* put password in array */
    passwordArray[2] = password;
    CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66b_badSink(passwordArray);
}

#endif /* OMITBAD */

#ifndef OMITGOOD

/* goodG2B uses the GoodSource with the BadSink */
void CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66b_goodG2BSink(char * passwordArray[]);

static void goodG2B()
{
    char * password;
    char * passwordArray[5];
    /* Initialize Data */
    password = "";
    password = (char *)malloc(100*sizeof(char));
    if (password == NULL)
    {
        printLine("Memory could not be allocated");
        exit(1);
    }
    /* FIX: Use VirtualLock() to lock the buffer into memory */
    if(!VirtualLock(password, 100*sizeof(char)))
    {
        printLine("Memory could not be locked");
        exit(1);
    }
    /* INCIDENTAL FLAW: CWE-259 Hardcoded Password */
    strcpy(password, "Password1234!");
    passwordArray[2] = password;
    CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66b_goodG2BSink(passwordArray);
}

void CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66_good()
{
    goodG2B();
}

#endif /* OMITGOOD */

/* Below is the main(). It is only used when building this testcase on
 * its own for testing or for building a binary to use in testing binary
 * analysis tools. It is not used when compiling all the testcases as one
 * application, which is how source code analysis tools are tested.
 */

#ifdef INCLUDEMAIN

int main(int argc, char * argv[])
{
    /* seed randomness */
    srand( (unsigned)time(NULL) );
#ifndef OMITGOOD
    printLine("Calling good()...");
    CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66_good();
    printLine("Finished good()");
#endif /* OMITGOOD */
#ifndef OMITBAD
    printLine("Calling bad()...");
    CWE591_Sensitive_Data_Storage_in_Improperly_Locked_Memory__w32_char_66_bad();
    printLine("Finished bad()");
#endif /* OMITBAD */
    return 0;
}

#endif

//						↓↓↓VULNERABLE LINES↓↓↓

// 43,4;43,10

// 75,4;75,10

