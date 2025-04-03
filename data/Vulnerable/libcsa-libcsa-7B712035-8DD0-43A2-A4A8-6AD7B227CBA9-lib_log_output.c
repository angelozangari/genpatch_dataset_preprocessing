/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_log_output.c $
$Archive: /V60/Development/AS_Instance/WMS/Dev/applib/lib_csa/lib_log_output.c $
$Revision: 15 $

********************************************************************************

Last changed:

$Author: Jrees $
$Modtime: 1/11/12 8:45 $
$Date: 1/11/12 10:07 $ (Of last check-in)

********************************************************************************

$History: lib_log_output.c $

*******************************************************************************/

char lib_log_output_sccsid[] = "@(#)$Workfile: lib_log_output.c $ $Revision: 15 $";

/******************************************************************************
$NoKeywords: $
********************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h"

#include <time.h>
#ifdef __alpha
#include <sys/mode.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#if defined _WIN64 || defined _WIN32
#include "astlib.h"
#else
#include <sys/time.h>
#endif

#include "hostvarsc.h"

int liblog_output_nofflush = 0;
int liblog_output_nocache = 0;

/* Linked list of cached FILE objects, in no particular order.  Since most processes
only open one trace file, the O(n) search time is negligible, and probably faster
than opening a file on every call.  New FILE objects are prepended to the list so they're
checked first.  Cache entries also store the day-of-month; when the current day-of-month
changes, the cache entry is discarded, so we start a new trace file. */
typedef struct fpcache
{
	char prcnam[PRCNAM_SIZE]; /* Name of the process this trace file is for */
	FILE *fp;        /* Open FILE object for writing to */
	int day;         /* Day of the month this FILE object was opened */
	long writes;     /* Number of writes into this file. */
	struct fpcache *next;
} fpcache;


#ifndef CSA_TRACE_NOTHREAD
#include "lib_thread_defs.h"

/* Mutex is initialised by the first call to lib_log_output.
There's technically a race condition before the first call
returns, but as long as a process returns from its first
lib_trace before spawning threads (which it will), it's fine. */
static CSAMutex trace_mtx;
static int is_initialised = FALSE;
#endif

#include "lib_date_extern.h"
#include "lib_log_output.h"

static fpcache *fpcache_housekeep(fpcache *cache, int day)
/*
Remove all expired file pointers from the cache,
returning the new location of the cache.
*/
{
	fpcache *p = cache;
	fpcache *prev = NULL;
	fpcache *next = NULL;

	while (p != NULL)
	{
		/* If this file wasn't opened today */
		if (day != p->day)
		{
			/* Close and remove this fp - it's expired */
			fclose(p->fp);
			if (prev)
			{
				prev->next = p->next;
			}
			else
			{
				cache = p->next;
			}
			next = p->next;
			free(p);
		}
		else
		{
			prev = p;
		}
		p = next;
	}

	return cache;
}


void lib_log_direct	(const char *progname, const char *messg, int trace)
/*****************************************************************
*                                                                *
* Function Name : lib_log_direct	                             *
*                                                                *
* Function      : Called to log messages to 'progname_mmmdd.txt' *
*                 and 'daily_mmmdd.txt' if not just tracing      *
*                                                                *
* Parameters                                                     *
* Inputs -   char *progname Process name                         *
*            char *messg    Message to be appended to file       *
*            int trace      If 0, also write to daily trace      *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Effects : Creates or appends to file 'daily.mmmdd'         *
*                and 'process.mmmdd'                             *
*                                                                *
*  DEPRECATED FUNCTION. Use ONLY if file-handle caching will     *
*  hinder trace output (eg crash-handler log files)              *
*                                                                *
*****************************************************************/
{
	static char envdir[FILENAME_MAX] = "";

	FILE *fp;
	struct timeval now;
	char ext[6];
	char pathname[FILENAME_MAX];
	char timestr[26];

	/* Get current date/time and format into 'ext' */
	/* lib_datclc(0,&now_t); */
	gettimeofday(&now, NULL);
	lib_timext(ext, now.tv_sec);

	/* Format the timestamp prefix for the trace message */
	lib_ctime(timestr, sizeof(timestr), &now.tv_sec);
	memmove(timestr, timestr + 4, 15);
	timestr[15] = '\0';

#ifndef CSA_TRACE_NOTHREAD
	if (!is_initialised)
	{
		if (csa_mutex_init(&trace_mtx) == 0)
		{
			is_initialised = TRUE;
		}
		else
		{
			fprintf(stderr, "lib_log_output: Failed to initialise mutex\n");
			return;
		}
	}
	csa_mutex_lock(&trace_mtx);
#endif

	/* Get environment directory for trace (just the once - cause it does not change) */
	if (envdir[0] == '\0')
	{
		char *env_env = getenv("ENVDIR");
		if (env_env == NULL)
		{
			fprintf(stderr, "lib_log_output: ENVDIR is not defined\n");
			goto lib_log_output_cleanup;
		}
		strcpy(envdir, env_env);
	}

	/* Create required file pathname from month and day in 'ext' */
	snprintf(pathname, sizeof(pathname), "%s/trace/%s_%s.txt",
		envdir, progname, ext);
	pathname[sizeof(pathname) - 1] = '\0';

	/* Open for append the required log file and append message */
	if ((fp = fopen(pathname, "a")) != NULL)
	{
		chmod(pathname, 0666);
		fprintf(fp, "%s.%03ld :%s\n",
			timestr, (long)now.tv_usec / 1000, messg);
		fclose(fp);
	}
	else
	{
		/* Redundancy is redundant
		* For server processes, try writing to stderr, only works on start up, else lose shell
		* For RDTs, don't bother, as the 'fix' is worse than the problem */
		/* Also exclude utlsub (build anomalies) */
		/* Also exclude logsem (persmissions) */
		if (strncmp(progname, "RDT", 3) != 0 &&
			strncmp(progname, "rdt", 3) != 0 &&
			strncmp(progname, "utlsub", 6) != 0 &&
			strncmp(progname, "logsem", 6) != 0)
		{
			fprintf(stderr, "Unable to log to file %s message:-\n %s\n", pathname, messg);
			goto lib_log_output_cleanup;
		}
	}

	/* Output also to daily file if not just tracing */

	if (!trace)
	{
		snprintf(pathname, sizeof(pathname), "%s/trace/daily_%s.txt", envdir, ext);
		pathname[sizeof(pathname) - 1] = '\0';

		/* Open for append the required log file and append message */
		if ((fp = fopen(pathname, "a")) != NULL)
		{
			chmod(pathname, 0666);
			fprintf(fp, "%s.%03ld :%s\n",
				timestr, (long)now.tv_usec / 1000, messg);
			fclose(fp);
		}
		else
		{
			/* Redundancy is redundant
			* For server processes, try writing to stderr, only works on start up, else lose shell
			* For RDTs, don't bother, as the 'fix' is worse than the problem */
			/* Also exclude utlsub (build anomalies) */
			/* Also exclude logsem (persmissions) */
			if (strncmp(progname, "RDT", 3) != 0 &&
				strncmp(progname, "rdt", 3) != 0 &&
				strncmp(progname, "utlsub", 6) != 0 &&
				strncmp(progname, "logsem", 6) != 0)
			{
				fprintf(stderr, "Unable to log to file %s message:-\n %s\n", pathname, messg);
				goto lib_log_output_cleanup;
			}
		}
	}

lib_log_output_cleanup:

#ifndef CSA_TRACE_NOTHREAD
	csa_mutex_unlock(&trace_mtx);
#endif

	return;
}

void lib_log_output(const char *prcnam, const char *messg, int trace)
/*****************************************************************
*                                                                *
* Function Name : lib_log_output                                 *
*                                                                *
* Function      : Called to log messages to 'progname_mmmdd.txt' *
*                 and 'daily_mmmdd.txt' if not just tracing      *
*                                                                *
* Parameters                                                     *
* Inputs -   char *prcnam   Process name                         *
*            char *messg    Message to be appended to file       *
*            int trace      If 0, also write to daily trace      *
*                                                                *
* Outputs -  none                                                *
*                                                                *
* Ext Effects : Creates or appends to file 'daily.mmmdd'         *
*                and 'process.mmmdd'                             *
*                                                                *
*****************************************************************/
{
	static fpcache *cache = NULL;
	static char envdir[FILENAME_MAX] = "";

	FILE *fp = NULL;
	FILE *daily_fp = NULL;
	struct timeval now;

	char ext[6];
	char pathname[FILENAME_MAX];
	char timestr[26];

	fpcache *fp_cursor;
	int is_cached = FALSE;
	int day = 0;

	/* Get current date/time and format into 'ext' */
	gettimeofday(&now, NULL);
	lib_timext(ext, now.tv_sec);
	day = atoi(&ext[3]);

#ifndef CSA_TRACE_NOTHREAD
	if (!is_initialised)
	{
		if (csa_mutex_init(&trace_mtx) == 0)
		{
			is_initialised = TRUE;
		}
		else
		{
			printf("lib_log_output: Failed to initialise mutex\n");
			goto lib_log_output_cleanup;
		}
	}
	csa_mutex_lock(&trace_mtx);
#endif


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

	/* Get environment directory for trace (just the once - cause it does not change) */
	if (envdir[0] == '\0')
	{
		char *env_env = getenv("ENVDIR");
		if (env_env == NULL)
		{
			printf("lib_log_output: ENVDIR is not defined\n");
			goto lib_log_output_cleanup;
		}
		strcpy(envdir, env_env);
	}

	/* Search the cache to see whether we have a suitable file descriptor already */
	if (!liblog_output_nocache)
	{
		for (fp_cursor = cache; fp_cursor; fp_cursor = fp_cursor->next)
		{
			if (strncmp(prcnam, fp_cursor->prcnam, sizeof(fp_cursor->prcnam)) == 0)
			{
				if (day == fp_cursor->day)
				{
					/* Use cached fp */
					is_cached = TRUE;
					fp = fp_cursor->fp;
					fp_cursor->writes++;
				}
				else
				{
					/* It's tomorrow, so tidy up any file pointers from yesterday */
					cache = fpcache_housekeep(cache, day);
				}
				break;
			}
		}
	}

	if (!is_cached)
	{
		/* Create required file pathname from month and day in 'ext' */
		snprintf(pathname, sizeof(pathname) - 1, "%s/trace/%s_%s.txt",
			envdir, prcnam, ext);
		pathname[sizeof(pathname) - 1] = '\0';

		/* Open for append the required log file and append message */
		if ((fp = fopen(pathname, "a")) == NULL)
			goto lib_log_output_cleanup;
	}

	fprintf(fp, "%s.%03ld :%s\n",
		timestr, (long)now.tv_usec / 1000, messg);

	if (!liblog_output_nofflush)
		fflush(fp);

	if (liblog_output_nocache)
	{
		fclose(fp);
		fp = NULL;
	}

	/* If the file pointer is not already cached, and it's not broken, cache it */
	if (!is_cached && fp)
	{
		fp_cursor = malloc(sizeof(fpcache));
		if (!fp_cursor) {
			fclose(fp);
			goto lib_log_output_cleanup;
		}

		snprintf(fp_cursor->prcnam, sizeof(fp_cursor->prcnam), "%s", prcnam);
		fp_cursor->fp = fp;
		fp_cursor->day = day;
		fp_cursor->next = cache;
		fp_cursor->writes = 1;
		cache = fp_cursor;
	}

	/* Output also to daily file if not just tracing */
	/* NOTE. Daily trace file handles are NOT cached, since we have to share the file */
	if (!trace)
	{
		snprintf(pathname, sizeof(pathname), "%s/trace/daily_%s.txt", envdir, ext);

		/* Open for append the required log file and append message */
		if ((daily_fp = fopen(pathname, "a")) == NULL)
			goto lib_log_output_cleanup;

		fprintf(daily_fp, "%s.%03ld :%s\n",
			timestr, (long)now.tv_usec / 1000, messg);
		fclose(daily_fp);
	}

lib_log_output_cleanup:

#ifndef CSA_TRACE_NOTHREAD
	csa_mutex_unlock(&trace_mtx);
#endif

	return;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 192,2;192,8

// 347,2;347,8

