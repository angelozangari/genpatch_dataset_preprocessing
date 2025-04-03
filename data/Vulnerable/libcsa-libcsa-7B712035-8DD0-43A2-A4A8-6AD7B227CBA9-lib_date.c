/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_date.c $
$Revision: 6 $

*******************************************************************************/

char lib_date_sccsid[] = "@(#)$Workfile: lib_date.c $ $Revision: 6 $";

/******************************************************************************

Description: The get_date_* functions assume that p_dattim is in the format
             "Www Mmm dd hh:mm:ss yyyy" (as returned by ctime())

*******************************************************************************/

#include "stdafx.h"

#include <time.h>
#include "lib_trace_extern.h"
#include "lib_date.h"

void lib_datclc(int numday,time_t *starttime)
/*****************************************************************
*                                                                *
* Function Name : lib_datclc                                     *
*                                                                *
* Function      : Calculate date 'numday' from current date or   *
*                 date entered                                   * 
*                                                                *
* Parameters                                                     *
* Inputs -   int numday    Number of days away to calculate to   *
*                          -ve is in the past, +ve in the future *
*                                                                *
* Modify -   time_t *starttime  Time is seconds from 1/1/70 if 0 *
*                         will calculate from current day at noon*
*                                                                *
*****************************************************************/
{
int cor;

/* If start time not defined get current date/time */

  if (*starttime == 0)
  {
    *starttime = time ((time_t *)0);
    if (numday != 0) /* If doing a calc normalise to noon (to avoid BST/GMT) */
    {
      cor = 12 - atoi(&ctime (starttime)[11]);
      *starttime += cor*60*60;
    }
  }

/* Calculate offset of time in days */

  *starttime += numday*24*60*60;
}

char *lib_timext(char *buf,time_t starttime)
/*****************************************************************
*                                                                *
* Function Name : lib_timext                                     *
*                                                                *
* Function      : Get date in form 'mmmdd' from entered time     *
*                 as used for log file extensions
*                                                                *
* Parameters                                                     *
*                                                                *
* Inputs -   char *buf  buffer ptr to store 'mmmdd' format       *
*            time_t starttime  Time is seconds from 1/1/70       *
*                                                                *
*****************************************************************/
{
extern int tolower(int);

char *b_ptr = ctime(&starttime);

  buf[0] = tolower(b_ptr[4]);
  buf[1] = b_ptr[5];
  buf[2] = b_ptr[6];
  sprintf(&buf[3],"%2.2d",atoi(&b_ptr[8]));
  return buf;
}


void get_date_YYYYMMDD(char *date_YYYYMMDD_str, const char *p_dattim)
/*================================================================
Function : get_date_YYYYMMDD
Purpose  : Get the current date in YYYYMMDD format
Notes    :
================================================================*/
{
	char l_mm[3] = "";
	char l_dd[3] = "";


	strcpy(l_mm, get_date_MM(p_dattim));
	strcpy(l_dd, get_date_DD(p_dattim));

	sprintf(date_YYYYMMDD_str, "%4.4s%2.2s%2.2s",
		p_dattim + 20, l_mm, l_dd);

	return;
}


void get_time_HHMMSS(char *time_HHMMSS_str, const char *p_dattim)
/*================================================================
Function : get_time_HHMMSS
Purpose  : Get the current time in HHMMSS format
Notes    :
================================================================*/
{
	char l_h2[3] = "";
	char l_mi[3] = "";
	char l_ss[3] = "";


	strcpy(l_h2, get_date_H2(p_dattim));
	strcpy(l_mi, get_date_MI(p_dattim));
	strcpy(l_ss, get_date_SS(p_dattim));

	sprintf(time_HHMMSS_str, "%2.2s%2.2s%2.2s", l_h2, l_mi, l_ss);

	return;
}


char *get_date_CC(const char *p_dattim)
/*================================================================
Function : get_date_CC
Purpose  : Get the century of the current date
Notes    :
================================================================*/
{
	static char l_cc[3] = "";


	strncpy(l_cc, p_dattim + 20, 2);
	l_cc[2] = '\0';

	return(l_cc);
}


char *get_date_YY(const char *p_dattim)
/*================================================================
Function : get_date_YY
Purpose  : Get the year of the century of the current date
Notes    :
================================================================*/
{
	static char l_yy[3] = "";


	strncpy(l_yy, p_dattim + 22, 2);
	l_yy[2] = '\0';

	return(l_yy);
}


char *get_date_MM(const char *p_dattim)
/*================================================================
Function : get_date_MM
Purpose  : Get the numeric month of the current date
Notes    :
================================================================*/
{
	static char l_mon[4] = "";
	static char l_mm[3] = "";


	strncpy(l_mon, p_dattim + 4, 3);
	l_mon[3] = '\0';

	if (0 == strcmp(l_mon, "Jan"))
	{
		strcpy(l_mm, "01");
	}
	else if (0 == strcmp(l_mon, "Feb"))
	{
		strcpy(l_mm, "02");
	}
	else if (0 == strcmp(l_mon, "Mar"))
	{
		strcpy(l_mm, "03");
	}
	else if (0 == strcmp(l_mon, "Apr"))
	{
		strcpy(l_mm, "04");
	}
	else if (0 == strcmp(l_mon, "May"))
	{
		strcpy(l_mm, "05");
	}
	else if (0 == strcmp(l_mon, "Jun"))
	{
		strcpy(l_mm, "06");
	}
	else if (0 == strcmp(l_mon, "Jul"))
	{
		strcpy(l_mm, "07");
	}
	else if (0 == strcmp(l_mon, "Aug"))
	{
		strcpy(l_mm, "08");
	}
	else if (0 == strcmp(l_mon, "Sep"))
	{
		strcpy(l_mm, "09");
	}
	else if (0 == strcmp(l_mon, "Oct"))
	{
		strcpy(l_mm, "10");
	}
	else if (0 == strcmp(l_mon, "Nov"))
	{
		strcpy(l_mm, "11");
	}
	else if (0 == strcmp(l_mon, "Dec"))
	{
		strcpy(l_mm, "12");
	}
	else
	{
		strcpy(l_mm, "00");
	}

	return(l_mm);
}


char *get_date_MON(const char *p_dattim)
/*================================================================
Function : get_date_MON
Purpose  : Get the char month of the current date
Notes    :
================================================================*/
{
	static char l_mon[4] = "";


	strncpy(l_mon, p_dattim + 4, 3);
	l_mon[3] = '\0';
	strup(l_mon);

	return(l_mon);
}


char *get_date_DD(const char *p_dattim)
/*================================================================
Function : get_date_DD
Purpose  : Get the day of month of the current date
Notes    :
================================================================*/
{
	char l_char_dd[3] = "";
	static char l_dd[3] = "";


	sprintf(l_char_dd, "%2.2s", p_dattim + 8);
	l_char_dd[2] = '\0';
	sprintf(l_dd, "%.2d", atoi(l_char_dd));

	return(l_dd);
}


char *get_date_H2(const char *p_dattim)
/*================================================================
Function : get_date_H2
Purpose  : Get the hours in 24 hour format of the current time
Notes    :
================================================================*/
{
	static char l_h2[3] = "";


	strncpy(l_h2, p_dattim + 11, 2);
	l_h2[2] = '\0';

	return(l_h2);
}


char *get_date_H1(const char *p_dattim)
/*================================================================
Function : get_date_H1
Purpose  : Get the hours in 12 hour format of the current time
Notes    :
================================================================*/
{
	static char l_h1[3] = "";
	char l_char_h1[3] = "";
	int l_int_h1 = 0;


	strncpy(l_char_h1, p_dattim + 11, 2);
	l_char_h1[2] = '\0';
	l_int_h1 = atoi(l_char_h1);

	if (0 == l_int_h1)
	{
		strcpy(l_h1, "12");
	}
	else if ((l_int_h1 > 0) && (l_int_h1 < 12))
	{
		sprintf(l_h1, "%2.2s", l_char_h1);
	}
	else if (12 == l_int_h1)
	{
		strcpy(l_h1, "12");
	}
	else
	{
		sprintf(l_h1, "%.2d", (l_int_h1 - 12));
	}
	l_h1[2] = '\0';

	return(l_h1);
}


char *get_date_MI(const char *p_dattim)
/*================================================================
Function : get_date_MI
Purpose  : Get the minutes of the current time
Notes    :
================================================================*/
{
	static char l_mi[3] = "";


	strncpy(l_mi, p_dattim + 14, 2);
	l_mi[2] = '\0';

	return(l_mi);
}


char *get_date_SS(const char *p_dattim)
/*================================================================
Function : get_date_SS
Purpose  : Get the seconds of the current time
Notes    :
================================================================*/
{
	static char l_ss[3] = "";


	strncpy(l_ss, p_dattim + 17, 2);
	l_ss[2] = '\0';

	return(l_ss);
}


char *get_date_MS(const char *p_dattim)
/*================================================================
Function : get_date_MS
Purpose  : Get the milliseconds of the current time
Notes    :
================================================================*/
{
	static char l_ms[3] = "";


	strncpy(l_ms, p_dattim, 2);
	l_ms[2] = '\0';

	return(l_ms);
}


char *get_date_AMPM(const char *p_dattim)
/*================================================================
Function : get_date_AMPM
Purpose  : Get the AM/PM indicator for the current time
Notes    :
================================================================*/
{
	static char l_ampm[3] = "";
	char l_char_h1[3] = "";
	int l_int_h1 = 0;


	strncpy(l_char_h1, p_dattim + 11, 2);
	l_char_h1[2] = '\0';
	l_int_h1 = atoi(l_char_h1);

	if (0 == l_int_h1)
	{
		strcpy(l_ampm, "AM");
	}
	else if ((l_int_h1 > 0) && (l_int_h1 < 12))
	{
		strcpy(l_ampm, "AM");
	}
	else if (12 == l_int_h1)
	{
		strcpy(l_ampm, "PM");
	}
	else
	{
		strcpy(l_ampm, "PM");
	}
	l_ampm[2] = '\0';

	return(l_ampm);
}


int lib_ctime(char *buffer, size_t numberOfElements, const time_t *time)
/*================================================================
Function : lib_ctime
Purpose  : Convert a time_t to human-readable ASCII.
Notes    : 

 buffer           - output is written here in the format:
                    Wed Jan 02 02:03:55 1980\n\0
 numberOfElements - the capacity of buffer in bytes (must be at
                    least 26)
 time             - the time to convert

           If ctime_s is available (Win32), this function is an
           alias for ctime_s.  If ctime_r is available (Posix),
           this function calls ctime_r.
           If neither ctime_s or ctime_r is available, we fall
           back on plain old ctime.
================================================================*/
{
#ifdef _WIN32
  return ctime_s(buffer, numberOfElements, time) ;

#elif _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE

  if(numberOfElements < 26)
    return 1;

  return ctime_r(time, buffer) == NULL;

#else

  /* Don't have ctime_s or ctime_r, so use non-safe standard C version */
  const char *static_time = ctime(time);
  if(static_time != NULL)
  {
    strncpy(buffer, static_time, numberOfElements);
    return 0;
  }
  else
  {
    return 1;
  }

#endif
}


//						↓↓↓VULNERABLE LINES↓↓↓

// 87,2;87,9

// 103,1;103,7

// 104,1;104,7

// 106,1;106,8

// 125,1;125,7

// 126,1;126,7

// 127,1;127,7

// 129,1;129,8

// 185,2;185,8

// 189,2;189,8

// 193,2;193,8

// 197,2;197,8

// 201,2;201,8

// 205,2;205,8

// 209,2;209,8

// 213,2;213,8

// 217,2;217,8

// 221,2;221,8

// 225,2;225,8

// 229,2;229,8

// 233,2;233,8

// 269,1;269,8

// 271,1;271,8

// 312,2;312,8

// 316,2;316,9

// 320,2;320,8

// 324,2;324,9

// 401,2;401,8

// 405,2;405,8

// 409,2;409,8

// 413,2;413,8

