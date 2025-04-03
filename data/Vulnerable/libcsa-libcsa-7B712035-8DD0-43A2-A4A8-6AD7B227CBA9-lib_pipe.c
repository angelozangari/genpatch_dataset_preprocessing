/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_pipe.c $
$Revision: 11 $

*******************************************************************************/

char lib_pipe_sccsid[] = "@(#)$Workfile: lib_pipe.c $ $Revision: 11 $";

/******************************************************************************
$NoKeywords: $
********************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h"

#if defined _WIN32 || defined _WIN64
#include <process.h>
#include <Windows.h>
#include "astlib.h"
#else
#include <unistd.h>
#endif

#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "config.h"
#include "autostore.h"

#include "lib_log_fprint_extern.h"
#include "lib_pipe.h"

/* Local defines */

#define NUM_PIPE_PROCESSES 20    /* Maximum number of pipe process that can
                                   be sent to by one process */

/* Local static pipe info */

struct Pipe_info {
  char server[10];
  int  write_fd;
  int  read_fd;
};

static struct Pipe_info pipe_info[NUM_PIPE_PROCESSES] = 
   { {"",0,0},
     {"",0,0},
     {"",0,0},
     {"",0,0},
     {"",0,0}
   };

int lib_snd_pipe_msg (const char *prog_name,
                      int   process_step,
                      const char *destination,
                      int   function_num,
                      struct msgbuffer *sndbuf)
/*****************************************************************
*                                                                *
* Function Name : lib_snd_pipe_msg                               *
*                                                                *
* Function      : Send message buffer to specified FIFO pipe.    *
*                 Returns SUCCESS if sent OK, else FAILURE.      *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *destination   Destination process             *
*            int   function_num  Message function number         *
*            struct msg_bug sndbuf  Pointer to send buffer       *
*                                                                *
* Outputs -  None                                                *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int  write_fd;
  int  msg_length;
  char full_msg_buf[11+11+7+7+11+MAX_MES_LEN];

  /* Get file descriptor for write end of pipe for this process */

  write_fd = lib_get_pipe_fd(destination, TRUE);
  if (write_fd == 0)      /* FIFO pipe not yet open */
  {
    /* Start up FIFO for communication with destination process.
    ** Open FIFO file for write.
    ** Should already have been created and opened for read by
    ** destination process.
    */
    if (lib_open_pipe(prog_name, process_step, destination,
                           TRUE, &write_fd) == FAILURE)
      return FAILURE;
  }

  /* Set up bits of message buffer thath are already known. */

  sndbuf->msgtyp = MSG_DATA;
  strncpy(sndbuf->dstprc, destination, 7);
  strncpy(sndbuf->sndprc, prog_name, 7);
  sndbuf->fctnum = function_num;

  /* Will send WHOLE message buffer down pipe.
  ** Because msgtxt bit is of variable length, work out length
  ** of WHOLE message buffer, and send this figure down pipe
  ** as first piece of info. (This size excludes the size of
  ** the size itself.)
  */
  msg_length = 11 + 7 +7 + 11 + (int)strlen(sndbuf->msgtxt);

  sprintf(full_msg_buf, "%+10.9d %+10.9ld %6s %6s %+10.9ld %s", 
                        msg_length,
                        sndbuf->msgtyp,
                        sndbuf->dstprc,
                        sndbuf->sndprc,
                        sndbuf->fctnum,
                        sndbuf->msgtxt);

  if (lib_write_pipe(prog_name, process_step, write_fd, full_msg_buf)
                                                                == FAILURE)
  {
    lib_log_fprint(prog_name, process_step, "lib_snd_pipe_msg",
                    "Failed to send message to FIFO pipe errno %d",errno);
    return FAILURE;
  }
  return SUCCESS;
}

int lib_rcv_pipe_msg (const char *prog_name,
                      int process_step,
                      const char *server,
                      struct msgbuffer *rcvbuf)
/*****************************************************************
*                                                                *
* Function Name : lib_rcv_pipe_msg                               *
*                                                                *
* Function      : Receive message buffer from FIFO pipe.         *
*                 Returns SUCCESS if received OK                 *
*                         NOT_FOUND if nothing read              *
*                         else FAILURE.                          *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*                                                                *
* Outputs -  struct msg_bug rcvbuf  Pointer to receive buffer    *
*                                                                *
* Ext Requirements : FIFO file must be open for read.            *
*                    Use lib_open_pipe() to open it.             *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int read_fd;
  int  retval;
  char full_msg_buf[11+7+7+11+MAX_MES_LEN];

  /* Get file descriptor for read end of pipe for this process */

  read_fd = lib_get_pipe_fd(server, FALSE);
  if ( read_fd == 0)
  {
    lib_log_fprint(prog_name, process_step, "lib_rcv_pipe_msg",
       "Read attempted from unopened pipe, call lib_start_server_pipe first");
    return FAILURE;
  }

  retval = lib_read_pipe(prog_name, process_step, read_fd, full_msg_buf);

  if (retval == NOT_FOUND)
    return NOT_FOUND;
  
  if (retval == FAILURE)
  {
    lib_log_fprint(prog_name, process_step, "lib_rcv_pipe_msg",
       "Failed to receive message from FIFO pipe errno %d",errno);
    return FAILURE;
  }

  /* Unscramble text read from pipe into message buffer.
  ** Read first 4 fields straight into message buffer.
  ** Whatever is left must be the msgtxt string.
  ** (Done this way, as msgtxt may contain spaces.)
  */
  sscanf(full_msg_buf, "%10ld %6s %6s %10ld", 
                       &rcvbuf->msgtyp,
                        rcvbuf->dstprc,
                        rcvbuf->sndprc,
                       &rcvbuf->fctnum);

  strcpy(rcvbuf->msgtxt, &full_msg_buf[11+7+7+11]);

  return SUCCESS;
}

static int lib_get_pipe_name(const char *prog_name,
                            int   process_step,
                            const char *server,
                            char *fname)
/*****************************************************************
*                                                                *
* Function Name : lib_get_pipe_name                              *
*                                                                *
* Function      : Get specified file name from static global     *
*                 structure.  If not there, set it up.           *
*                 Returns pointer to file name.                  *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *server        Server to get file name for     *
*                                                                *
* Outputs -  char *fname         File name of fifo pipe          *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int  i;

  for (i = 0; i < NUM_PIPE_PROCESSES && pipe_info[i].server[0] != '\0' ; i++)
  {
    if ((strcmp(pipe_info[i].server, server) == 0))
    {
      sprintf(fname, "%s/dat/%s.fifo", getenv("ENVDIR"), server);
      return SUCCESS;
    }
  }
  if (i < NUM_PIPE_PROCESSES)
  {
    strcpy(pipe_info[i].server,server);
    sprintf(fname, "%s/dat/%s.fifo", getenv("ENVDIR"), server);
    return SUCCESS;
  }
  else
    lib_log_fprint(prog_name, process_step, "lib_get_pipe_name",
                  "More than %d pipes defined",NUM_PIPE_PROCESSES); 
  return FAILURE;
}

int lib_close_pipe(const char *prog_name,
                   int   process_step,
                   const char *server)
/*****************************************************************
*                                                                *
* Function Name : lib_close_pipe                                 *
*                                                                *
* Function      : Close sepecified file descriptor held in static*
*                 global structure.                              *
*                 Returns FAILURE if server not known, else      *
*                 SUCCESS.                                       *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *server        Server to close fd for          *
*                                                                *
* Outputs - none                                                 *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int  i;

  for (i = 0; i < NUM_PIPE_PROCESSES; i++)
  {
    if ((strcmp(pipe_info[i].server, server) == 0))
    {
      if (pipe_info[i].read_fd != 0)
        close(pipe_info[i].read_fd);
      if (pipe_info[i].write_fd != 0)
        close(pipe_info[i].write_fd);
      return SUCCESS;
    }
  }

  lib_log_fprint(prog_name, process_step, "lib_close_pipe",
                  "Unknown server [%s]", server) ;
  return FAILURE;
}

int lib_start_server_pipe(const char *prog_name,
                          int   process_step,
                          const char *server,
                          int *filedesc)
/*****************************************************************
*                                                                *
* Function Name : lib_start_server_pipe                          *
*                                                                *
* Function      : Start server end of specified fifo pipe.       *
*                 Create FIFO pipe & open it twice - once for    *
*                 read, once for write.                          *  
*                 Note : Never actually use write pipe.  Opened  *
*                 so read pipe remains opened until explicitly   *
*                 closed.                                        *
*                 Returns FAILURE if server not known or fail    *
*                 at any time, otherwise SUCCESS.                *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *server        Server to start pipe for        *
*                                                                *
* Outputs -  int *filedesc       Read port file description      *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
int w_fd;

  if (lib_create_pipe(server, process_step, server) == FAILURE)
    return FAILURE;

  if (lib_open_pipe(server, process_step, server, FALSE,filedesc) == FAILURE)
    return FAILURE;

  if (lib_open_pipe(server, process_step, server, TRUE, &w_fd) == FAILURE)
   return FAILURE;

  return SUCCESS;
}


static int lib_create_pipe (const char *prog_name,
                                 int   process_step,
                            const char *server_prog)
/*****************************************************************
*                                                                *
* Function Name : lib_create_pipe                                *
*                                                                *
* Function      : Create FIFO pipe for specified server process, *
*                 if it doesn't al;ready exist.                  *
*                 Returns SUCCESS if file created, else FAILURE. *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *server_prog   Name of server process          *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  char fname[256];

  if (lib_get_pipe_name(prog_name, process_step, server_prog, fname)
                                                  == FAILURE)
    return FAILURE;

/* TODO :Need windows equivalent for mknod */
#if !defined _WIN32 && !defined _WIN64
  if ((mknod(fname, S_IFIFO | 0666, 0) < 0) && (errno != EEXIST))
  {
    lib_log_fprint(prog_name, process_step, "lib_create_pipe",
     "Failed to create FIFO pipe %s errno %d", fname,errno);
    return FAILURE;
  }
#endif

  return SUCCESS;
}

int lib_open_pipe (const char *prog_name,
                   int   process_step,
                   const char *server_prog,
                   int   write_mode,
                   int  *filedesc)
/*****************************************************************
*                                                                *
* Function Name : lib_open_pipe                                  *
*                                                                *
* Function      : Open FIFO pipe for specified server process in *
*                 specified mode (read or write).                *
*                 Returns SUCCESS if file opened, else FAILURE.  *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *server_prog   Name of server process          *
*            int   write_mode    File mode (read or write)       *
*                                                                *
* Outputs -  int   filedesc      File descriptor opened          *
*                                                                *
* Ext Requirements : FIFO file must exist.                       *
*                    Use lib_create_pipe() to create it.         *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  char fname[256];

  if (lib_get_pipe_name(prog_name, process_step, server_prog, fname)
                                                  == FAILURE)
    return FAILURE;

/* TODO ? O_NDELAY (no blocking on read) not available on windows */
#if defined _WIN32 || defined _WIN64
#define O_NDELAY 0
#endif

  if ((*filedesc = open(fname,(write_mode ? O_WRONLY:O_RDONLY)|O_NDELAY)) < 0)
  {
    lib_log_fprint(prog_name, process_step, "lib_open_pipe",
     "Failed to open FIFO pipe %s for %s",fname,(write_mode ? "write":"read"));
    return FAILURE;
  }

  if (lib_set_pipe_fd(prog_name, process_step, server_prog,
                           write_mode, *filedesc) == FAILURE)
    return FAILURE;
  return SUCCESS;
}

static int lib_write_pipe (const char *prog_name,
                                   int process_step,
                                   int filedesc,
                           const char *msg_text)
/*****************************************************************
*                                                                *
* Function Name : lib_write_pipe                            *
*                                                                *
* Function      : Write to specified FILE pipe.                  *
*                 Returns SUCCESS if written to OK, else FAILURE.*
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            int   filedesc      File descriptor to use          *
*            char *msg_text      Message to write to pipe        *
*                                                                *
* Outputs -  None                                                *
*                                                                *
* Ext Requirements : FIFO file must be open for write.           *
*                    Use lib_open_pipe() to open it.             *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int msg_size;

  msg_size = (int)strlen(msg_text);

  if (write(filedesc, msg_text, msg_size) != msg_size)
  {
    lib_log_fprint(prog_name, process_step, "lib_write_pipe",
                    "Failed to write to FIFO pipe errno %d",errno);
    return FAILURE;
  }

  return SUCCESS;
}

static int lib_read_pipe (const char *prog_name,
                          int   process_step,
                          int   filedesc,
                          char *msg_text)
/*****************************************************************
*                                                                *
* Function Name : lib_read_pipe                                  *
*                                                                *
* Function      : Read from specified FILE pipe.                 *
*                 Returns SUCCESS if read from pipe OK;          *
*                         NOT_FOUND if no data in pipe;          *
*                         else FAILURE.                          *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            int   filedesc      File descriptor to use          *
*                                                                *
* Outputs -  char *msg_text      Message read from pipe          *
*                                                                *
* Ext Requirements : FIFO file must be open for read.            *
*                    Use lib_open_pipe() to open it.             *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int  num_chars;
  int  msg_length;
  char msg_len_str[12];

  /* First field in message is length of WHOLE message buffer which follows. */
  
  num_chars = read(filedesc, msg_len_str, 11);

  if (num_chars == 0)
    return NOT_FOUND;

  if (num_chars != 11)
  {
    lib_log_fprint(prog_name, process_step, "lib_read_pipe",
      "Failed to read message size from FIFO pipe errno %d num chars %d",
         errno,num_chars);
    return FAILURE;
  }

  msg_len_str[11] = '\0';
  msg_length = atoi(msg_len_str);

  /* Read WHOLE message buffer (now that size is known) */

  if (read(filedesc, msg_text, msg_length) != msg_length)
  {
    lib_log_fprint(prog_name, process_step, "lib_read_pipe",
                    "Failed to read from FIFO pipe errno %d",errno);
    return FAILURE;
  }

  msg_text[msg_length] = '\0';

  return SUCCESS;
}

static int lib_get_pipe_fd(const char *server,
                           int   write_mode)
/*****************************************************************
*                                                                *
* Function Name : lib_get_pipe_fd                                *
*                                                                *
* Function      : Get specified file descriptor from static      *
*                 global structure.                              *
*                 Returns FAILURE if server not known, else      *
*                 returns required file descriptor.              *
*                                                                *
* Parameters                                                     *
* Inputs -   char *server        Server to get fd for            *
*            int   write_mode    Mode of fd to get (read;write)  *
*                                                                *
* Outputs - none                                                 *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int  retval;
  int  i;

  for (i = 0; i < NUM_PIPE_PROCESSES; i++)
  {
    if ((strcmp(pipe_info[i].server,server)==0))
    {
      if (write_mode)
        retval = pipe_info[i].write_fd;
      else                           /* read fd */
        retval = pipe_info[i].read_fd;
      return retval;
    }
  }

  return 0;
}

static int lib_set_pipe_fd(const char *prog_name,
                           int   process_step,
                           const char *server,
                           int   write_mode,
                           int   fd)
/*****************************************************************
*                                                                *
* Function Name : lib_set_pipe_fd                                *
*                                                                *
* Function      : Set specified file descriptor in static        *
*                 global structure.                              *
*                 Returns FAILURE if server not known, else      *
*                 SUCCESS.                                       *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prog_name     Calling program name            *
*            int   process_step  Process step number             *
*            char *server        Server to set fd for            *
*            int   write_mode    Mode of fd to set (read;write)  *
*            int   fd            Value of fd to be stored        *
*                                                                *
* Outputs - none                                                 *
*                                                                *
* Ext Requirements : none                                        *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int  i;

  for (i = 0; i < NUM_PIPE_PROCESSES ; i++)
  {
    if ((strcmp(pipe_info[i].server,server) == 0))
    {
      if (write_mode)
        pipe_info[i].write_fd = fd;
      else
        pipe_info[i].read_fd = fd;
      return SUCCESS;
    }
  }

  lib_log_fprint(prog_name, process_step, "lib_set_pipe_fd",
                  "Unknown server [%s] in %s mode",
                  server,(write_mode ? "write" : "read")) ;
  return FAILURE;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 128,2;128,9

// 209,2;209,8

// 245,6;245,13

// 251,4;251,10

// 252,4;252,11

