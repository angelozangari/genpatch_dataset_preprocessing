/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*  

$Workfile: lib_alarm_msg.c $
$Revision: 24 $

*******************************************************************************/

char lib_alarm_msg_sccsid[] = "@(#)$Workfile: lib_alarm_msg.c $ $Revision: 24 $";

/******************************************************************************
$NoKeywords: $
********************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h"

#include <stdarg.h>
#include <errno.h>

#include "config.h"
#include "autostore.h"
#include "hostvarsc.h"

#include "lib_log_fprint_extern.h"
#include "lib_string_extern.h"
#include "lib_trace_extern.h"
#include "libmsg_extern.h"

/* Moved into lib_alarm_msg.c from libmsg, thus libcsa can control libora without having a link dependancy on it. */
int alert_registered = FALSE;

extern int debug;

void invalidate_alert(const char *prog_name, int step)
/*****************************************************************
Invalidate the oracle alert, such that next time around the scanning loop, 
libmsg will re-register the alert.
*****************************************************************/
{
    alert_registered = FALSE; 
    lib_log_fprint(prog_name, step, __func__, " Oracle Database alert_registered flag invalidated.");
}

static int lib_alarm_msg_vprintf(const char *prog_name, int step, int whscde, const char *alarm_code,
                                 char *almtxt_out, const char *format, va_list args)
/*****************************************************************
 Format an alarm text string consisting of the warehouse code, the
 alarm number, then an arbitrary formatted string ("vector" version).

 This doesn't send any messages, it just builds the string.  If you
 want to send the message to alarmm afterwards, you can just send it
 directly as an alarmm msg 2.

 alarm_code - a valid almnum string, normally of the form prcnam.xx
 almtxt_out - should point to a buffer that will receive the
 formatted alarm text; the buffer must have a capacity
 of at least ALMTXT_SIZE.
 format - a printf-style format string used to build the body of the
 alarm text.
 args - a valid va_list of arguments corresponding to the format string.
*****************************************************************/
{
  int capacity = ALMTXT_SIZE;
  int wrote = 0;

  /* Zero-padded signed whscde followed by space-padded almnum */
  wrote = lib_snprintf(almtxt_out, capacity, "%+010d%-*s", whscde, ALMNUM_ACTSIZE, alarm_code);

  if(wrote < 0 || wrote >= capacity)
  {
    lib_log_fprint(prog_name, step, __func__, "Error: snprintf returned %d", wrote);
    return FAILURE;
  }
  
  capacity -= wrote;

  /* Append the rest of the arguments */
  wrote = vsnprintf(&almtxt_out[wrote], capacity, format, args);

  if(wrote < 0 || wrote >= capacity)
  {
    lib_log_fprint(prog_name, step, __func__, "Error: vsnprintf returned %d, trying to format [%s]", wrote, format);
    return FAILURE;
  }

  if(debug > 2)
    lib_trace(prog_name, "%s: formatted almtxt[%s]", __func__, almtxt_out);

  return SUCCESS;
}


/* Enable string format checking for lib_alarm_msg if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
int lib_alarm_msg(const char *prog_name, int step, int whscde, const char *alarm_code,
                  const char *format, ...)
__attribute__(( format(printf,5,6) ));
#endif
/*#{CHEAD_CP_END}*/

int lib_alarm_msg(const char *prog_name, int step, int whscde, const char *alarm_code,
                  const char *format, ...)
/*****************************************************************
*                                                                *
* Function Name : lib_alarm_msg                                  *
*                                                                *
* Function      : To send alarms to ALARMM process               *
*                 returns FAILURE if function fails, otherwise   *
*                   SUCCESS                                      *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name   Calling program name              *
*            int   proc_step   Process step number               *
*            int   whscde      Warehouse code                    *
*            char *alarm_code  Number of message in 'alarms' tbl.*
*            char *format      PRINTF format of any data to be   *
*                              inserted into message             *
*            ...               Data to be inserted in message    *
*                              Must match format string          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/
{

  va_list  args;
  char almtxt[ALMTXT_SIZE] = "";
  int retval = SUCCESS;    /* Assume succesful return */

  va_start(args, format);
  retval = lib_alarm_msg_vprintf(prog_name, step, whscde, alarm_code, almtxt, format, args);
  va_end(args);

  if(retval != SUCCESS)
  {
    lib_log_fprint(prog_name, step, __func__, "Failed in lib_alarm_msg_vprintf; whscde[%d] alarm_code[%s] format[%s]", whscde, alarm_code, format);
    return FAILURE;
  }

  /* Always trace alarm */
  lib_trace(prog_name, "%s: Raising [%s] using [%s]", 
            __func__, alarm_code, almtxt); 

  /* Send buffer as function 2 to 'alarmm' process using qsnd - so as to
   not corrupt sqlcode etc. */
  {
    char qsnd[FILENAME_MAX];
    char cmd[512];

#if defined _WIN64
    lib_makePath("executablesX64/qsnd", NULL, qsnd, sizeof(qsnd));
#else 
  #if defined _WIN32
    lib_makePath("executables/qsnd", NULL, qsnd, sizeof(qsnd));
  #else
    lib_makePath("exe/qsnd", NULL, qsnd, sizeof(qsnd));
  #endif
#endif

    lib_snprintf(cmd, sizeof(cmd), "%s 1 %s alarmm 2 \"%s\"",
                 qsnd, prog_name, almtxt);

    if (system(cmd) == -1)
    {
      lib_log_fprint(prog_name, step, __func__,
        "Failed to run qsnd to send alarm; errno = %d", errno); 
      return FAILURE;
    }
  }

  if (strcmp(alarm_code, "sqllib.02") == 0)
  {
	  invalidate_alert(prog_name,step);
  }

  /* Cleanup variable arguments and return */
  return SUCCESS;
}


/* Enable string format checking for lib_alarm_intalm_msg if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
int lib_alarm_intalm_msg(const char *prog_name, int proc_step, int whscde, int intalm,
                         const char *alarm_code, const char *format, ...)
__attribute__(( format(printf,6,7) ));
#endif
/*#{CHEAD_CP_END}*/

int lib_alarm_intalm_msg(const char *prog_name, int proc_step, int whscde, int intalm,
                         const char *alarm_code, const char *format, ...)
/*****************************************************************
*                                                                *
* Function Name : lib_alarm_intalm_msg                           *
*                                                                *
* Function      : To send alarms to ALARMM process               *
*                 returns FAILURE if function fails, otherwise   *
*                   SUCCESS                                      *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name   Calling program name              *
*            int   proc_step   Process step number               *
*            int   whscde      Warehouse code                    *
*            int   intalm      Internal Alarm Number             *
*            char *alarm_code  Number of message in 'alarms' tbl.*
*            char *format      PRINTF format of any data to be   *
*			       inserted into message             *
*            ...	       Data to be inserted in message    *
*                              Must match format string          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{

  va_list  args;
  struct msgbuffer sndbuf; /* Message send buffer */
  struct msgbuffer msg; /* Message send buffer */
  int idx;              /* Index into mtext string */
  int ret = SUCCESS;    /* Assume succesful return */

  va_start(args, format);

  /* 
  ** Copy alarm code into send buffer and pad it with blanks upto ALMNUM_SIZE
  ** characters
  */

  strcpy(sndbuf.msgtxt, alarm_code);
  for (idx = (int)strlen(alarm_code) ; idx < ALMNUM_SIZE-1 ; ++idx)
    sndbuf.msgtxt[idx] = ' ';

  /* Use 'format' to process rest of arguments into send buffer */
  vsprintf(&sndbuf.msgtxt[ALMNUM_SIZE-1], format, args);

  sprintf(msg.msgtxt, "%+010d%+010d%s", whscde, intalm, sndbuf.msgtxt); 

  /* Send buffer as function 2 to 'alarmm' process */

  if (lib_snd_msg(prog_name, proc_step, ALARMM_Q, ALARMM, 4, &msg)
                                                         == FAILURE)
  {
    lib_log_fprint(prog_name, proc_step, "lib_alarm_msg",
		    "Failure in lib_snd_msg to ALARMM_Q errno = %d", errno); 
    ret = FAILURE;
  }

  /* Cleanup variable arguments and return */

  va_end(args);
  return ret;
}


char * lib_sql_alarm_id (const char *prcnam)
/*****************************************************************
*                                                                *
* Function Name : lib_sql_alarm_id                               *
*                                                                *
* Function      : To retrieve the SQL alarm ID for a process     *
*                 (for automating the raising of SQL alarm       *
                   messages)                                     *
* Parameters                                                     *
* Inputs -   char *prog_name   Calling program name              *
*                                                                *
* Outputs -  the alarm ID to be used for SQL alarms              *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
static char id [40];

  /* All process SQL error mestxt.almnum'stest have a 6 character process name,
    so truncate to 6 (e.g rdtusr0372 > rdtusr) */
  sprintf (id, "%6.6s.01", prcnam);
  return id;
}


/* Enable string format checking for lib_alarm_auto if using gcc. */
/*#{CHEAD_CP_START(extern)}*/
#ifdef __GNUC__
int lib_alarm_auto(const char *prog_name, int proc_step, int whscde, const char *rakusr,
                   const char *alarm_code, const char *format, ...)
__attribute__(( format(printf,6,7) ));
#endif
/*#{CHEAD_CP_END}*/

int lib_alarm_auto(const char *prog_name, int proc_step, int whscde, const char *rakusr,
                   const char *alarm_code, const char *format, ...)
/*****************************************************************
*                                                                *
* Function Name : lib_alarm_auto                                 *
*                                                                *
* Function      : To send auto acknowledge alarms                * 
*                 to ALARMM process                              *
*                 returns FAILURE if function fails, otherwise   *
*                   SUCCESS                                      *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name   Calling program name              *
*            int   proc_step   Process step number               *
*            char *alarm_code  Number of message in 'alarms' tbl.*
*            char *format      PRINTF format of any data to be   *
*			       inserted into message             *
*            ...	       Data to be inserted in message    *
*                              Must match format string          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  va_list  args;

  struct msgbuffer sndbuf; /* Message send buffer */
  struct msgbuffer msg; /* Message send buffer */
  int ret = SUCCESS;    /* Assume succesful return */

  va_start(args, format);

  /* Use 'format' to process arguments into send buffer */
  vsprintf(sndbuf.msgtxt, format, args);


  sprintf (msg.msgtxt,"%+10.9d%-*.*s%-*.*s%-*.*s", 
    whscde,
    RAKUSR_SIZE -1,
    RAKUSR_SIZE -1,
    rakusr,
    ALMNUM_SIZE -1,
    ALMNUM_SIZE -1,
    alarm_code,
    150,
    150,
    sndbuf.msgtxt);


  /* Put msg 1 onto alarmm queue */

  if (FAILURE == lib_snd_msg(prog_name, proc_step, ALARMM_Q, ALARMM, 1, &msg))
  {
    lib_log_fprint (prog_name, proc_step, "lib_alarm_auto", "Failure to send message 1 to queue ALARMM_Q, errno = [%d] [%s]", errno, msg.msgtxt);
    return FAILURE;
  }


  /* Cleanup variable arguments and return */

  va_end(args);
  return ret;
}



int lib_alarm_render(const char *prcnam, int step,
                     const char *almmsg, const char *msgbuf,
                     char *out_almtxt)
/*****************************************************************

 Given an almmsg and a string of packed variables, produces a
 readable almtxt.

 Essentially, this is a caveman's implementation of sprintf
 that uses numbers in square brackets instead of % escapes,
 only supports fixed-length strings, and expects all replacement
 parameters to be concatenated together into one frankenstring.

 Scans the [almmsg] string, and replaces all numbers in square
 brackets with the corresponding number of bytes copied
 sequentially from [msgbuf], writing the result to [out_almtxt].

 You can include a literal '[' in the almmsg by doubling it
 up: [[ => [.
 You don't need to double up ']'.

 The result is subject to HTML-ish whitespace consolidation, i.e.
 multiple spaces are turned into single spaces.

 [out_almtxt] must point to a buffer of at least ALMTXT_SIZE bytes.

*****************************************************************/
{
  char *out_point = out_almtxt;
  const char * const out_end = out_almtxt + ALMTXT_ACTSIZE;
  const char * const buf_end = msgbuf + strlen(msgbuf);
  const char *buf_point = msgbuf;
  const char* msg_point;

  if(debug > 1)
    lib_trace(prcnam, "%s: almmsg[%s] msgbuf[%s]", __func__, almmsg, msgbuf);

  /* Iterate through almmsg, copying each char into out_almtxt.  Whenever we find an
     opening square bracket, scan to the corresponding closing bracket, pull out
     the number it contains, and copy that number of bytes from msgbuf, then continue
     from just after the closing bracket. */
  for(msg_point = almmsg; *msg_point != '\0'; msg_point++)
  {
    if(*msg_point == '[')
    {
      if(*(msg_point+1) == '[')
      {
        *out_point++ = '[';
        msg_point++;
        continue;
      }

      const char *close = strchr(msg_point+1, ']');
      if(close == NULL)
      {
        lib_log_fprint(prcnam, step, __func__, "No matching ']' in almmsg:\n%s\n%*s%c", almmsg, (int)(msg_point-almmsg), " ", '^');
        *out_point++ = '[';
        continue;
      }

      int consume = atoi(msg_point+1);
      int write = consume;
      /* Don't write past the end of the buffer */
      if(out_point + consume >= out_end)
        write = (int)(out_end - out_point - 1);

      /* Output a fixed-length string from msgbuf to tmpstr */
      lib_snprintf(out_point, out_end - out_point, "%-*.*s",
                   write, write, buf_point);
      /* Interim-trim */
      while(*(out_point+write-1) == ' ' && write > 0)
      {
        write--;
        *(out_point+write) = '\0';
      }

      /* Jump input pointer to the closing bracket */
      msg_point = close;
      /* Increment output pointer by however much we wrote */
      out_point += write;
      /* Increment buf pointer by however many placeholder bytes we consumed */
      buf_point += consume;
      if(buf_point > buf_end)
        buf_point = buf_end;

    }
    else
    {
      *out_point++ = *msg_point;
    }
  }

  /* Terminate the output string */
  *out_point = '\0';

  /* Remove excessive spaces from the alarm text */
  /* No longer needed since we trim as we go */
  /*lib_alarm_consolidate(out_almtxt, tmptxt);*/

  if(debug > 2)
    lib_trace(prcnam, "Alarm text rendered as [%s]", out_almtxt);
  return SUCCESS;
}



//						↓↓↓VULNERABLE LINES↓↓↓

// 247,2;247,8

// 254,2;254,9

// 296,2;296,9

// 350,2;350,9

