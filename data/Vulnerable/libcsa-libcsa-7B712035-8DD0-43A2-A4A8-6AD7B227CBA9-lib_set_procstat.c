/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_set_procstat.c $
$Revision: 9 $

*******************************************************************************/

char lib_set_procstat_sccsid[] = "@(#)$Workfile: lib_set_procstat.c $ $Revision: 9 $";

/******************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h"

#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#if defined _WIN64 || defined _WIN32
#include <process.h>
#include "astlib.h"
#else
#include <unistd.h>
#include <sys/resource.h>
#endif

/* Stop config,h redefining signal */
#define no_csa_signal 1

#include "autostore.h"
#include "config.h"


#include "lib_log_fprint_extern.h"
#include "lib_trace_extern.h"
#include "libdb_extern.h"
#include "libshm_extern.h"
#include "lib_set_procstat.h"

/* external variables */
extern int db_connected;
#if defined _WIN64 || defined _WIN32
extern int bShutdown;
#endif

/* Local variables for debugging */

static int l_debug = FALSE;     /* Debug logical copy */
static char l_name[7];          /* Name of process */
static int l_proc_idx = 0;      /* Local store for process index */
static struct shmprc *l_prcdata = NULL;

int lib_set_procstat (const char *prcnam,
                      int process_step,
                      int proc_idx,
                      struct shmprc *prcdata,
                      int *debug)
/*****************************************************************
*                                                                *
* Function Name : lib_set_procstat                               *
*                                                                *
* Function      : To set process status in the MSEG_PROCESS      *
*                 shared memory area and outputs the debug       *
*                 logical for this process ,return FAILURE if    *
*                 function fails, otherwise SUCCESS              *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prcnam   Calling program name              *
*            int process_step  Process step number               *
*             if step is PRC_START sets up pid and name also     *
*             if step is PRC_DEAD clears pid and detaches area   *
*            int proc_idx      Index into process                *
*            struct prc_seg_struct *prcdata  Pointer to data     *
*             area MSEG_PROCESS, If NULL process will attach and *
*             detach area its self.                              *
*                                                                *
* Outputs -  int *debug       Value of process debug status      *
*                                                                *
* Ext Requirements : Shared are MSEG_PROCESS must exist          *
*                                                                *
* Ext Effects : none                                             *
*                                                                *
*****************************************************************/   
{
  int attach_shm;
  static int last_db_connected = FALSE;
  static int last_step = -1;
  /* Pick up local prcdata pointer , if process has been terminated
   *  as lib_attach_shm call fails on HP-UX after a kill TERM */

  if (process_step == PRC_DEAD)
    prcdata = l_prcdata;

  attach_shm = db_connected;

  /* Attach shared memory if not done by caller */
  if (prcdata == NULL && attach_shm)
  {
    prcdata = (struct shmprc *)lib_attach_shm(prcnam,process_step,MSEG_PROCESS);
    if (prcdata == (void*)FAILURE )
    {
      lib_log_fprint(prcnam,process_step,"lib_set_procstat",
       "Failure in lib_attach_shm to MSEG_PROCESS errno = %d",errno); 
      return (FAILURE);
    }
  }
  l_prcdata = prcdata;

  l_debug = *debug;
  if (db_connected)
  {
    /* Get debug from procst if first time connected or after step 5 */
	if (last_db_connected == FALSE || last_step == 5)
	{
	  l_debug = libdb_get_debug(prcnam, process_step);
	}

    if (last_db_connected == FALSE)
    {
#if defined linux
      int pid = getpid();
#else
      int pid = _getpid();
#endif
 
      last_db_connected = TRUE;
      for (proc_idx = 0 ; proc_idx < MAX_PROCESS 
        && strcmp(prcnam, prcdata[proc_idx].prcnam) != 0 ; proc_idx++);
      if (proc_idx < MAX_PROCESS)
      {
        last_db_connected = TRUE;
        strcpy(l_name,prcnam);
        l_proc_idx=proc_idx;
        prcdata[proc_idx].prcpid = pid;
        prcdata[proc_idx].instnc = 1;
      }     
      libdb_set_prcpid(prcnam,process_step,pid);
    }



#if defined linux
    if (process_step == PRC_DEAD)
#else
    if (bShutdown)
#endif
    {
      libdb_set_prcpid(prcnam,process_step,0);
    }
  }
  last_step = process_step;

#if defined linux
  if (process_step == PRC_START)
  {
    signal(SIGHUP, SIG_IGN); /* Ignore terminal hang ups from parents */
    signal(SIGTERM,exit_process);
  }
#endif

  *debug = l_debug;
  return SUCCESS;
}

static void exit_process()
/* Handling for SIGTERM ,usually called by SYSMAN kill call */
{
  if (l_debug) lib_trace(l_name,"Termination requested");
  lib_set_procstat (l_name,PRC_DEAD,l_proc_idx,NULL,&l_debug);
  exit(0);
}


//						↓↓↓VULNERABLE LINES↓↓↓

// 141,8;141,14

