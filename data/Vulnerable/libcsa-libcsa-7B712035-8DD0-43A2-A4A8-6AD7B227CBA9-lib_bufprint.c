/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_bufprint.c $
$Archive: /V60/Development/AS_Instance/WMS/Dev/applib/lib_csa/lib_bufprint.c $
$Revision: 8 $

********************************************************************************

Last changed:

$Author: Phil $
$Modtime: 19/05/11 10:12 $
$Date: 1/06/11 13:59 $ (Of last check-in)

********************************************************************************

$History: lib_bufprint.c $

*******************************************************************************/

char lib_bufprint_sccsid[] = "@(#)$Workfile: lib_bufprint.c $ $Revision: 8 $";

/******************************************************************************
$NoKeywords: $
********************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h" 

#include "lib_trace_extern.h"

extern int debug;

void lib_bufprint (const char *process, const char *desc,
                   const unsigned char *buf, int len)
/* Trace communication data routine */
{
  char buffer[512];
  char *bptr=buffer;
  int  olen = len;

  buffer[0]=0;

  while (len && (bptr - buffer) < 500) 
  {
    switch (*buf) 
    {
    case 0: sprintf (bptr, "<NUL>"); break;
    case 1: sprintf (bptr, "<SOH>"); break;
    case 2: sprintf (bptr, "<STX>"); break;
    case 3: sprintf (bptr, "<ETX>"); break;
    case 4: sprintf (bptr, "<EOT>"); break;
    case 5: sprintf (bptr, "<ENQ>"); break;
    case 6: sprintf (bptr, "<ACK>"); break;
    case 7: sprintf (bptr, "<BEL>"); break;
    case 9: sprintf (bptr, "<HT >"); break;
    case 10: sprintf (bptr, "<NL >"); break;
    case 11: sprintf (bptr, "<VT >"); break;
    case 13: sprintf (bptr, "<CR >"); break;
    case 16: sprintf (bptr, "<DLE>"); break;
    case 21: sprintf (bptr, "<NAK>"); break;
    case 27: sprintf (bptr, "<ESC>"); break;
    case 30: sprintf (bptr, "<RS >"); break;
    case 127: sprintf (bptr, "<DEL>"); break;
    default:
      if (*buf < ' ' || *buf >= 128)
        sprintf (bptr, "<x%02X>", *buf);
      else
      {
        sprintf (bptr, "%c", *buf);
        bptr = bptr-4;
      }
      break;
    }
    bptr = bptr+5;
    ++buf;
    --len;
  } 
  lib_trace(process,"%s :[%s] length [%d]",desc,buffer, olen);

  if (len)
    lib_trace(process,"%s :[%d] characters not printed "
      "input string too long!", desc, len);

  return;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 57,12;57,19

// 58,12;58,19

// 59,12;59,19

// 60,12;60,19

// 61,12;61,19

// 62,12;62,19

// 63,12;63,19

// 64,12;64,19

// 65,12;65,19

// 66,13;66,20

// 67,13;67,20

// 68,13;68,20

// 69,13;69,20

// 70,13;70,20

// 71,13;71,20

// 72,13;72,20

// 73,14;73,21

// 76,8;76,15

// 79,8;79,15

