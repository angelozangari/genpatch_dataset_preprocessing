/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_trace.c $
$Revision: 10 $

*******************************************************************************/

char lib_trace_sccsid[] = "@(#)$Workfile: lib_trace.c $ $Revision: 10 $";

/******************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h"

#include <stdarg.h>
#include <time.h>

/*#{CHEAD_CP_START(extern)}*/
#include <stdio.h>
/*#{CHEAD_CP_END}*/


#include "autostore.h"
#include "lib_date_extern.h"
#include "lib_log_output_extern.h"
#include "lib_ringbuffer_extern.h"
#include "libcsa_AI.h"

#if defined _WIN64 || defined _WIN32
#include "astlib.h"
#else
#include <sys/time.h>
#endif


/* external variables */
extern int debug;

/* Enable string format checking for lib_trace if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
void lib_trace(const char *prcnam, const char *format, ...)
__attribute__(( format(printf,2,3) ));
#endif
/*#{CHEAD_CP_END}*/

void lib_trace(const char *prog_name, const char *format, ...)
/*****************************************************************
*                                                                *
* Function Name : lib_trace                                      *
*                                                                *
* Function      : To output debug/trace information to a trace   *
*                 file as per 'printf'.                          *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name   Calling program name              *
*            char *format      PRINTF format of any data to be   *
*                               inserted into message            *
*            ...               Data to be inserted in message    *
*                              Must match format string          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Effects : Creates or appends to file 'process.mmmdd'       *
*                                                                *
*****************************************************************/   
{
  va_list args;
  char stack_buf[1024];
  size_t capacity = sizeof(stack_buf);
  char *buf = stack_buf;
  int required;
  int justTrace = TRUE; /* If FALSE, log to daily trace too */


  /* Write into the local buffer */
  va_start(args, format);
  required = vsnprintf(stack_buf, capacity, format, args);
  va_end(args);

  if(required < 0)
  {
    snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: first call to vsnprintf returned %d]", __func__, required);
    justTrace = FALSE;
    goto output;
  }

  /* If the return value is >= the buffer size, the string was truncated,
     so allocate exactly the required space and try again. */
  if((unsigned)required >= capacity)
  {
    capacity = required + 1;

    if(!(buf = (char*)malloc(capacity)))
    {
      snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: out of memory]", __func__);
      buf = stack_buf;
      justTrace = FALSE;
      goto output;
    }

    va_start(args, format);
    required = vsnprintf(buf, capacity, format, args);
    va_end(args);
    
    if(required < 0)
    {
      snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: second call to vsnprintf returned %d]", __func__, required);
      buf = stack_buf;
      justTrace = FALSE;
      goto output;
    }
  }

output:

  /* Output trace to file */
  lib_log_output(prog_name, buf, justTrace);

  /* If buf isn't pointing at our local array, it's
     pointing at some memory that needs freeing. */
  if(buf != stack_buf)
    free(buf);
}


/* Enable string format checking for lib_ntrace if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
void lib_ntrace(int debug_lvl, const char *prog_name, const char *format, ...)
__attribute__(( format(printf,3,4) ));
#endif
/*#{CHEAD_CP_END}*/

void lib_ntrace(int debug_lvl, const char *prog_name,
                 const char *format, ...)
/*****************************************************************
*                                                                *
* Function Name : lib_ntrace                                     *
*                                                                *
* Function      : Conditionally output debug/trace information   *
*                 to a trace file as per 'printf'.               *
*                 Trace is output if and only if                 *
*                 debug >= debug_lvl                             *
*                                                                *
*                 This provides a shorthand for the common       *
*                 pattern:                                       *
*                    if(debug > n){ lib_trace(prcnam...);} ,     *
*                                                                *
*                 the equivalent being:                          *
*                    lib_ntrace(n+1, prcnam...);                 *
*                                                                *
*                 Caveat: function call overhead even if debug   *
*                 level is low                                   *
*                                                                *
* Parameters                                                     *
* Inputs -   int debug_lvl    If debug < debug_lvl, do nothing   *
*            char *prog_name  Calling program name               *
*            char *format     PRINTF format of any data to be    *
*                               inserted into message            *
*            ...              Data to be inserted in message     *
*                              Must match format string          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Effects : Creates or appends to file 'process.mmmdd'       *
*                                                                *
*****************************************************************/
{
  va_list args;
  char stack_buf[1024];
  size_t capacity = sizeof(stack_buf);
  char *buf = stack_buf;
  int required;
  int justTrace = TRUE; /* If FALSE, log to daily trace too */

  if(debug < debug_lvl)
  {
    /* Debug level is too low, so don't do anything */
    return;
  }

  /* Write into the local buffer */
  va_start(args, format);
  required = vsnprintf(stack_buf, capacity, format, args);
  va_end(args);

  if(required < 0)
  {
    snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: first call to vsnprintf returned %d]", __func__, required);
    justTrace = FALSE;
    goto output;
  }

  /* If the return value is >= the buffer size, the string was truncated,
     so make a bigger buffer and try again. */
  if((unsigned)required >= capacity)
  {
    capacity = required + 1;

    if(!(buf = (char*)malloc(capacity)))
    {
      snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: out of memory]", __func__);
      buf = stack_buf;
      justTrace = FALSE;
      goto output;
    }

    va_start(args, format);
    required = vsnprintf(buf, capacity, format, args);
    va_end(args);

    if(required < 0)
    {
      snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: second call to vsnprintf returned %d]", __func__, required);
      buf = stack_buf;
      justTrace = FALSE;
      goto output;
    }
  }

output:

  /* Output trace to file */
  lib_log_output(prog_name, buf, justTrace);

  /* If buf isn't pointing at our local array, it's
     pointing at some memory that needs freeing. */
  if(buf != stack_buf)
    free(buf);
}


/* Enable string format checking for lib_showtrace if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
void lib_showtrace(const char *prog_name, const char *format, ...)
__attribute__(( format(printf,2,3) ));
#endif
/*#{CHEAD_CP_END}*/

void lib_showtrace(const char *prog_name, const char *format, ...)
/*****************************************************************
*                                                                *
* Function Name : lib_showtrace                                  *
*                                                                *
* Function      : To output debug/trace information to a trace   *
*                 file as per 'printf' and stdout.               *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name   Calling program name              *
*            char *format      PRINTF format of any data to be   *
*                               inserted into message            *
*            ...               Data to be inserted in message    *
*                              Must match format string          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Effects : Creates or appends to file 'process.mmmdd'       *
*                                                                *
*****************************************************************/   
{
  va_list args;
  char stack_buf[1024];
  size_t capacity = sizeof(stack_buf);
  char *buf = stack_buf;
  int required;
  int justTrace = TRUE; /* If FALSE, log to daily trace too */

  /* Write into the local buffer */
  va_start(args, format);
  required = vsnprintf(stack_buf, capacity, format, args);
  va_end(args);

  if(required < 0)
  {
    snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: first call to vsnprintf returned %d]", __func__, required);
    justTrace = FALSE;
    goto output;
  }

  /* If the return value is >= the buffer size, the string was truncated,
     so allocate exactly the required space and try again. */
  if((unsigned)required >= capacity)
  {
    capacity = required + 1;

    if(!(buf = (char*)malloc(capacity)))
    {
      snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: out of memory]", __func__);
      buf = stack_buf;
      justTrace = FALSE;
      goto output;
    }

    va_start(args, format);
    required = vsnprintf(buf, capacity, format, args);
    va_end(args);

    if(required < 0)
    {
      snprintf(stack_buf, sizeof(stack_buf), "[Error in %s: second call to vsnprintf returned %d]", __func__, required);
      buf = stack_buf;
      justTrace = FALSE;
      goto output;
    }
  }

output:

  /* Output trace to file */
  lib_log_output(prog_name, buf, justTrace);
  /* And to stdout */
  fprintf(stdout, "%s\n", buf);

  /* If buf isn't pointing at our local array, it's
     pointing at some memory that needs freeing. */
  if(buf != stack_buf)
    free(buf);
}




char *lib_rtrim(char *cp)
/*================================================================
    Purpose : Strip spaces off end of a string
  ================================================================*/
{
    register char *c;
    register int i;

    if (cp != NULL && *cp != '\0')
    {
        for (c = cp + strlen(cp) - 1, i= 0; c >= cp; c--, i++)
          if (!isspace(*c))
            break;

        if (i > 0)
          *++c = '\0';
    }

    return cp;
}


char *lib_ltrim(char *cp)
/*================================================================
    Purpose : Strip spaces off front of a string
  ================================================================*/
{
    register char *c;

    if (cp != NULL && *cp != '\0')
    {
        for (c = cp ; *c != '\0'; c++)
            if (!isspace(*c)) break;
        if (c != cp)
        {
          register char *l = cp;

          for ( ; *c != '\0' ; c++)
            *l++ = *c;
          *l = '\0';
        }
    }
    return cp;
}

void strup(char *s1)
/*=============================================================================
  Function : strup()
   Purpose : Surprised not to see this not included with standard DEC C 
	          function but here it is a noddy call that converts to higher
	          case if you really need to.
=============================================================================*/
{
 extern int toupper(int);
 register char *gopher = s1;

 while( *gopher != '\0')
 {
  *gopher = toupper(*gopher);
  gopher++;
 }
}

void lib_rtrim_s(char *string, short num)
  /*======================================================================
   
    Function : lib_rtrim_s();
     Purpose : Used to right trim a string, num gives the option to trim
               after num of spaces, so if you want to trim after 3 spaces, 
               not just the first space in the string.
   
   ======================================================================*/
{
  register char *s1 = string;

  while (*s1 != '\0')
  {
    if (*s1 == ' ')
    {
      register char *s2 = s1;
      short count = 0;

      while (count < num && *s2 == ' ' )
      {
        s2++;
        count++;
      }

      if (count == num)
      {
        *s1 = '\0';
        return;
      }

    }
    s1++;
  }

}


/*#{CHEAD_CP_START}*/
struct _CsaRamFile;
typedef struct _CsaRamFile CsaRamFile;
/*#{CHEAD_CP_END}*/

struct _CsaRamFile
{
  char *name;      /* The name of this buffer - has no meaning whatsoever. */
  CsaRingBuffer *lines;
};


CsaRamFile *csa_ramfile_open(const char *name, size_t max_lines)
/*
 Return a new CsaRamFile object that will keep the latest {max_lines} lines
 of trace in memory.  To write to it, use csa_ramfile_trace().  To write its contents
 to a file, use csa_ramfile_dump.  When you've finished with it, use csa_ramfile_destroy().
 {name} is stored, but isn't used.  It can be NULL if you don't care.
*/
{
  CsaRamFile *file = (CsaRamFile*)malloc(sizeof(*file));

  if(!name)
    name = "unnamed";

  file->name = (char*)malloc(strlen(name)+1);
  strcpy(file->name, name);
  file->lines = csa_rb_create(max_lines, free);

  return file;
}

void csa_ramfile_close(CsaRamFile *file)
/* Deallocate the given CsaRamFile. */
{
  if(file)
  {
    free(file->name);
    csa_rb_destroy(file->lines);
    free(file);
  }
}

void csa_ramfile_dump(CsaRamFile *file, FILE *destination)
/* Dump the contents of the CsaRamFile {file} to {destination}. */
{
  if(!destination)
    return;

  if(!file)
    fprintf(destination, "%s: No buffer specified\n", __func__);

  size_t length = csa_rb_length(file->lines);
  size_t i;
  for(i=0; i<length; i++)
  {
    const char *line = (const char*)csa_rb_at(file->lines, i);
    fprintf(destination, "%s\n", line ? line : "Error: NULL trace line in buffer");
  }
}


/* Enable string format checking for csa_ramfile_trace if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
void lib_ramfile_trace(CsaRamFile *file, int level, const char *format, ...)
__attribute__((format(printf, 3, 4)));
#endif
/*#{CHEAD_CP_END}*/


int csa_ramfile_trace(CsaRamFile *file, int level, const char *format, ...)
/* Write a line of trace to the given CsaRamFile.  If the file has reached
   its maximum capacity, this will cause the oldest line to be discarded.
   {level} is currently ignored.
*/
{
  va_list args;

  /* Check how much space is required. */
  va_start(args, format);
  int msg_len = vsnprintf(NULL, 0, format, args);
  va_end(args);

  if(msg_len < 0)
    return -1;


  /* Get current date/time and format into 'ext' */
  struct timeval now;
  gettimeofday(&now, NULL);
  char timestr[26];
#if defined _WIN64 || defined _WIN32
  /* Format the timestamp prefix for the trace message */
  lib_ctime(timestr, sizeof(timestr), &now.tv_sec);
  memmove(timestr, timestr + 4, 15);
  timestr[15] = '\0';
#else
  {
    struct tm nowtm;
    /* Format the timestamp prefix for the trace message */
    localtime_r(&now.tv_sec, &nowtm);
    strftime(timestr, sizeof(timestr), "%b %d %H:%M:%S", &nowtm); /* TODO: Check whether we want zero padding on the day (use %e for space padding) */
  }
#endif

  int time_len = (int)strlen(timestr) + 6; /* Include space for ".nnn :" */

  /* Allocate the required space */
  char *line = (char*)malloc(time_len + msg_len + 1);

  /* Start with the date/time prefix */
  sprintf(line, "%s.%03ld :", timestr, (long)now.tv_usec / 1000);

  /* Append the actual message */
  va_start(args, format);
  vsnprintf(line + time_len, msg_len + 1, format, args);
  va_end(args);

  return csa_rb_push(file->lines, line);
}


const char *csa_ramfile_get_name(CsaRamFile *file)
/* Return the name of the ramfile (or "unnamed" if it doesn't have one). */
{
  return file->name;
}


//						↓↓↓VULNERABLE LINES↓↓↓

// 461,2;461,8

// 547,2;547,9

