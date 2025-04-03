/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_decode_ean.c $
$Revision: 46 $

*******************************************************************************/

char lib_decode_ean_sccsid[] = "@(#)$Workfile: lib_decode_ean.c $ $Revision: 46 $";

/*******************************************************************************

Description:

*******************************************************************************/

#include "stdafx.h"

#include <ctype.h>

#include "autostore.h"
#include "hostvarsc.h"

#include "lib_log_fprint_extern.h"
#include "lib_trace_extern.h"
#include "lib_decode_ean.h"
#include "lib_decode_ean_extra_extern.h"

extern int debug;

/* gl_plnscn: default to false, modules that include lib_decode_ean
              and care about enabling pallet number scanning
              (palnum => sscc) should declare it extern  */
int gl_plnscn = FALSE;

int lib_decode_ean(const char *prcnam, int step, int ean_coding, 
                   int num_bcode,char *barcode,
                   char *sscc,char item[17],int *cases,
                   char *bbdate, char *copdat,
                   char *prddat, char *usedat,
                   char *batref, char *csn, int *aitype,
                   char *serial, char* int_90, int *netwgt)
/*================================================================
Function Name :  lib_decode_ean

      Purpose :  To extract sscc,item codes cases count and best before date
                 from non-ean standard barcodes

      Parameters Input:
              prcnam,step     Normal error handling parameters
              int num_bcode   1 indicates first barcode read from label
              char *barcode   Barcode data (null terminated)

      Parameters Output:
              char sscc[19]   18 character SSCC from label or 9 digit palnum
              char item[17]   14 character EAN item code
              int  cases      Number of cases on pallet
              char bbdate[11] Barcode Date in format 'yyyy-mm-dd'
              char copdat[11] Copack date
              char prddat[11] Production date
              char usedat[11] Use by date
              char batref[21] Upto 20 character lot code (or batch reference)
              char csn[21]    Cadbury CSN,
                              used as itmcde (Not strictly EAN standard)
              int  aitype     application identifier type 1 (01) or 2 (02)
              char serial[21] Upto 20 character serial number
              char int_90[31] Upto 30 character internal value (reservered for internal customer use)
              int  netwgt     Net read from label (in KG) converted to grammes.

      Returns : EAN_BARCODE if barcode is EAN type
                PALNUM_BARCODE if barcode just contained palnum
                FAILURE if barcode invalid (error message generated)

 Note: This module is coded using information from the following document:-

             THE EAN LABEL
               Using the
        UCC/EAN Application Identifier Standard

        Edition May 1995, Section 2

  Updated from QIN spec 2006 - To include AI 240, 241, 250, 251
                                          AI 90-99
================================================================*/
{
int art_numb = 0;
int var_len = 0;
int itm_len = 0;
char *b_ptr = barcode;
int i = 0;
int len = 0;
char numb_buf[20] = "";
int retval = SUCCESS;
int decplace = 0;
int bigwgt = 0;
int padding = FALSE;
int got_map_cntrl = FALSE;
char l_batref[21] = "";
int size_item = 0; 

  /* First try project-specific barcode format.
   * If fail then try the generic style */

  retval = lib_decode_ean_extra(prcnam, step, ean_coding, num_bcode, barcode,
                               sscc, item, cases, bbdate, l_batref, csn);
  if ( retval == EAN_BARCODE ||
       retval == PALNUM_BARCODE )
  {	   
    if (debug)
       lib_trace(prcnam, "BARCODE decoded by project-specific format");
	strncpy(batref, l_batref, BATREF_SIZE - 1);
	batref[BATREF_SIZE - 1] = '\0';
    return retval;
  }

  if (retval == EAN_PAD_BARCODE)
    padding = TRUE;

  if (debug)
     lib_trace(prcnam, "BARCODE didn't decode as project-specific format; try generic format");

  /* if barcode length is between 7 and PALNUM_SIZE-1 chars,
   * it could be a pallet number, so assume it is */

  len = (int)strlen(lib_rtrim(barcode));

  /* If location entered that has 7 letters or more in it, the palnum barcode
   * code pads it out with zeros corrupting the data */
/*
  for (i = 0; i < len; i++)
  {
    if (!isdigit (barcode[i]) )
      is_num = FALSE;
  }

  lib_trace(prcnam,"is_num [%d]", is_num);
*/

  lib_trace(prcnam,"lib_decode_ean: enter with [gl_plnscn %d] num_bcode[%d] len [%d]",
            gl_plnscn, num_bcode, len);


/* Set not found values on first barcode */

  if (num_bcode == 1)
  {
    strcpy(sscc,"");
    size_item = sizeof(item);
    memset(item, 0, size_item);
    *cases = 0;
    strcpy(bbdate,"");
    strcpy(copdat,"");
    strcpy(prddat,"");
    strcpy(usedat,"");
    strcpy(batref,"");
	strcpy(l_batref, "");
    strcpy(csn,"");
    if (strlen(serial) > 0)
    {
      strcpy(serial,"");
    }
    if (strlen(int_90) > 0)
    {
      strcpy(int_90,"");
    }
    *aitype=999;
    *netwgt = 0;
  }


  /* gl_plnscn code formally removed, as this has now been correctly 
     recoded in the RDT calling code.  This is NOT a valid part of 
     lib_decode_ean functionality, as it bypasses EAN coding */



/* Get next article number in barcode */
  if (debug)
  {
    lib_trace(prcnam, "Unpack barcode string...");
  }
  while (b_ptr[0] != '\0')
  {

    if (b_ptr[1] == '\0')
    {
      lib_trace(prcnam,"Invalid odd character at end of barcode [%c]",b_ptr[0]);
      return FAILURE;
    }

    /* Check AI is valid */
    if (!isdigit (b_ptr[0]) || !isdigit (b_ptr[1]))
    {
      lib_trace(prcnam, "Invalid Artical number [%2.2s]", b_ptr);
      return FAILURE;
    }

    art_numb = (b_ptr[0] - '0')*10 + b_ptr[1] - '0';
    b_ptr = b_ptr + 2;
    if ((art_numb >= 24 && art_numb <= 25)
     || (art_numb >= 40 && art_numb <= 42)
     || art_numb == 31 )
    {
      art_numb = art_numb*10 + b_ptr[0] - '0';
      b_ptr++;
    }

    /* Get variable length */
    got_map_cntrl = FALSE; 
    for (var_len = 0 ; ! iscntrl(b_ptr[var_len]) && ! got_map_cntrl ; var_len++)
    {
      if (! iscntrl(b_ptr[var_len]))
      {
        if (b_ptr[var_len] == '[' &&
            b_ptr[var_len+1] == '@' &&
            b_ptr[var_len+2] == 'G' &&
            b_ptr[var_len+3] == 'S' &&
            b_ptr[var_len+4] == '@' &&
            b_ptr[var_len+5] == ']')
        {
          got_map_cntrl = TRUE; 
          var_len--;
        }
      }
    }

    if ( var_len == 0 && art_numb == 10 )
    {
      lib_trace(prcnam, "Length %d for AI %d allowed ", art_numb, var_len);
    }	    
    else if (var_len < 1)
    {
      lib_trace(prcnam, "No enough characters after AI %d", art_numb);	  
      return FAILURE;
    }

    if (debug)
      lib_trace(prcnam,"on barcode %d %s decoding art_num %d length %d",
        num_bcode,b_ptr, art_numb,var_len);

/* Use article number to select item length and extract any data required */

/* NOTE: DO NOT CHANGE FIXED VALUES THAT SET itm_len TO DATABASE FIELD SIZES,
    IF FIELD SIZE ON DATABASE IS DIFFERENT FROM VALUES BELOW DATABASE DESIGN
    IS WRONG NOT THIS CODE */

    switch (art_numb)
    {
      case 00: /* Serial Shipping Container Code */
        itm_len = 18;
        strncpy(sscc,b_ptr,itm_len);
        sscc[itm_len] = '\0';
        for (i = 0; i < itm_len; i++)
        {
          if (!isdigit (sscc[i]) )
          {
            lib_trace(prcnam,"Invalid SSCC [%s]", sscc);
            return FAILURE;
          }
        }
        break;

      case 01: /* EAN article Number note can get two scenarios here */
        itm_len = 14;
        strncpy(item,b_ptr,itm_len); /* First 14 charaters of full EAN number */
        item[itm_len] = '\0';
        for (i = 0; i < itm_len; i++)
        {
          if (!isdigit (item[i]) )
          {
            lib_trace(prcnam,"Invalid EAN 01  [%s]", item);
            return FAILURE;
          }
        }

        if (ean_coding == ATTCDE_EAN_CODING_BAC)
          *cases = 0; /* default to 0 will overwrite if different */
        else
          *cases = 1; /* default to 1 will overwrite if different */

	*aitype =1;
        break;

      case 02: /* Item code */
        itm_len = 14;
        strncpy(item,b_ptr,itm_len); /* First 14 charaters of full EAN number */
        item[itm_len] = '\0'; /* anything after itm_len could be random */

        for (i = 0; i < itm_len; i++)
        {
          if (!isdigit (item[i]) )
          {
            lib_trace(prcnam,"Invalid EAN 02  [%s]", item);
            return FAILURE;
          }
        }
        *aitype=2;
        break;                       /* which this + variant (16 chars) */

      case 10: /* Batch or lot number */
        itm_len = var_len;
        if (var_len > 20)
          itm_len++;
        else if ( var_len > 0 )
        {
          strncpy(l_batref, b_ptr, var_len);
          l_batref[var_len] = '\0';
        }
        break;

      case 11: /* Production date */
      case 13: /* Packaging date (copack) */
      case 15: /* Minimum Durability date (best before) */
      case 17: /* Maximum Durability date (use by) */
        itm_len = 6;
        for (i = 0; i < 6; i++)
        {
          if (!isdigit (b_ptr[i]) )
          {
            lib_trace(prcnam,"Invalid AI %d date [%6.6s]", art_numb, b_ptr);
            return FAILURE;
          }
        }

        {
          char *date_ptr;

          switch (art_numb)
          {
            case 11: date_ptr = prddat; break;
            case 13: date_ptr = copdat; break;
            case 15: date_ptr = bbdate; break;
            case 17: date_ptr = usedat; break;
            default: date_ptr = prddat; break;
          }

          /* Format to ANSI standard 'yyyy-mm-dd' from 'yymmdd' */
          if (b_ptr[0] < '9')
            strcpy(date_ptr,"20"); /* allow for 21st century */
          else
            strcpy(date_ptr,"19");
          date_ptr += 2;
          strncpy(date_ptr,b_ptr,2); date_ptr += 2;
          strncpy(date_ptr,"-",1); date_ptr += 1;
          strncpy(date_ptr,b_ptr+2,2); date_ptr += 2;
          strncpy(date_ptr,"-",1); date_ptr += 1;
          strncpy(date_ptr,b_ptr+4,2); date_ptr += 2;
          date_ptr[0] = '\0';
          /* Need to move ptr back to start of string for convert */
          date_ptr -= 10;
          lib_convert_French_bbdate(prcnam, date_ptr);
        }
        break;

      case 20: /* Product Variant */
        itm_len = 2;
        strncpy(&item[14],b_ptr,itm_len); /* Append variant to EAN number */
        item[16] = '\0';
        if (!isdigit (item[14]) || !isdigit (item[14+1]))
        {
          lib_trace(prcnam,"Invalid product variance [%2.2s]", b_ptr);
          return FAILURE;
        }
        break;

      case 21: /* Serial Number */
        itm_len = var_len;
        if (var_len > 20)
          itm_len++;
        else
        {
          strncpy(serial, b_ptr, var_len);
          serial[var_len] = '\0';
        }

        break;

      case 30: /* Measurements */
        itm_len = var_len;
        if (var_len > 8) itm_len++;
        break;

      case 31: /* Measurements */
        if (debug)
          lib_trace(prcnam, "ignoring measurement");
        break;

      case 310: /* Net weight */
        itm_len = 6;
        decplace = (*b_ptr) - 48;  /* Next digit is no. decimal places */
        b_ptr++;
        strcpy(numb_buf, b_ptr);
        numb_buf[itm_len] = '\0';
        bigwgt = atoi(numb_buf);

        /* Convert weight to grammes (assuming KG on label) & return as integer */
        *netwgt = (bigwgt * 1000) / (my_pow (10, decplace));

        if (debug)
          lib_trace (prcnam, "extracted netwgt [%d] bigwgt [%d] decplace [%d]",
                     *netwgt, bigwgt, decplace);
        break;

      case 32: /* Measurements */
      case 33: /* Logistic Measurements */
      case 34: /* Logistic Measurements */
        itm_len = 8; /* MAY BE 6 , spec is not very clear */
        break;

      case 37: /* Case Quantity */
        if (var_len > 8) /* They have put it the middle of the bar code */
        {
          int ix;

          /* we have made a nice big hole to fall into here.
             variable length fields are meant to end with the GS character.
             we did not use GS at the end of the case qty field, instead
             trying to second guess the number of zeros preceeding the no
             of cases (we may have been sent labels like this anyway).
             The original code falls over if the labels contains a GS and
             has leading 0's eg 000026<GS>. it is ok for someting like
             26<GS> though.
             The change that I have made is to keep going to the end of the
             string looking for a <GS>. If one is found, use this as the no
             of cases terminator. If no <GS> is found, use the old code (so
             hopefully it is not broken for non <GS> labels
          */
          ix = 0;
          while (b_ptr[ix] != '\0' && b_ptr[ix] != 29)
          {
            ix++;
          }

          if (b_ptr[ix] == 29)
          {
            var_len = ix + 1;
            *cases = atoi(b_ptr);
          }
          else
          {
            /* Check if non digit terminates, input */
            for (ix = 0 ; ix < 6 && isdigit(b_ptr[ix]) ; ix++);

            if (ix < 6 ) /* Yes there is a termination , go use it */
            {
              var_len = ix + 1;
              *cases = atoi(b_ptr);
            }
            /* Work out if fixed length is 4 or 8, by assuming cases<=9999 */
            else
            {
              if (strncmp(b_ptr,"0000",4) == 0)
              {
                var_len = 8;
              }
              else
              {
                var_len = 4;
              }
              strncpy(numb_buf, b_ptr, var_len);
              numb_buf[var_len] = 0;
              *cases = atoi(numb_buf);
            }
          }
          if (debug)
            lib_trace(prcnam, "Cases: %d from var_len %d terminator %d",
                      *cases, var_len, b_ptr[ix]);
        }
        else /* Case count at end, just use length remaining */
        {
          *cases = atoi(b_ptr);
        }

        if (*cases <= 0)
        {
          lib_trace(prcnam,"Invalid Case Qty [%*.*s]", var_len, var_len, b_ptr);
          return FAILURE;
        }
        itm_len = var_len;
        break;

      case 90: /* Mutually Agreed Between Trading Partners */
        itm_len = var_len;
        if (var_len > 30)
          itm_len++;
        else
        {
          strncpy(int_90, b_ptr, var_len);
          int_90[var_len] = '\0';
        }
        break;

      case 91: /* Cadbury CSN (itmcde) */

/*********************** CADBURY SPECIFIC *************************/

#ifdef CSN_BARCODESIZE

        if (var_len >= CSN_BARCODESIZE)
          itm_len = CSN_BARCODESIZE;
        else
          itm_len = var_len;
        strncpy(csn, b_ptr, itm_len);
        csn[itm_len] = '\0';

        /* Special for cadbury (clip first 3 zeros) */
        if (csn[0] == '0' && csn[1] == '0' && csn[2] == '0'
          && itm_len == CSN_BARCODESIZE)
        {
          int ix;
          for (ix = 3; ix < CSN_BARCODESIZE ; ix++)
            csn[ix-3] = csn[ix];
          csn[ix-3] = '\0';
        }
        break;
#endif

      case 92: /* Internal */
      case 93: /* Internal */
      case 94: /* Internal */
      case 95: /* Internal */
      case 96: /* Internal */
      case 97: /* Internal */
      case 98: /* Internal */
      case 99: /* Internal */
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        break;

      case 240: /* Additional Product Identification  */
        itm_len = var_len;
        if (var_len > 30) 
        {
          itm_len++;
        }
        else
        {
          if (lib_decode_item_position(prcnam, step) == 240)
          {
            lib_decode_return_itmcde(ean_coding, csn, b_ptr, itm_len);
          }
        }
        break;

      case 241: /* Customer Part Number */
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        break;
      case 250: /* Secondary Serial Number */
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        break;
      case 251: /* Reference to Source Entity */
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        break;
      case 400: /* Customer Information */
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        break;
      case 401: /* Shipment Identification */
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        break;
      case 410: /* Ship to location code */
        itm_len = 13;
        break;
      case 420: /* Ship to postal code */
        itm_len = var_len;
        if (var_len > 9) itm_len++;
        break;
      case 421: /* Ship to postal code preceded by 3-digit ISO country code */
        itm_len = var_len;
        if (var_len <= 3 || var_len > 12) itm_len++;
        break;

/* Handle any unknown AI's - accept if length less than 31 characters */

      default:
        itm_len = var_len;
        if (var_len > 30) itm_len++;
        lib_trace(prcnam,
           "UNKNOWN AI %d FROM BARCODE [%s] VARIABLE LENGTH %d",
             art_numb, barcode, var_len);
    }

/* Handle any barcode decode problems */

    if (itm_len > var_len || itm_len == 0)
    {
      size_t xlen = strlen(barcode); 
      if ( itm_len == 0 && art_numb == 10 )
      {

        lib_trace(prcnam,
                  "Zero length allowed for article no. %d from barcode [%s] position %lu",
                  art_numb,barcode,(unsigned long)xlen-var_len);
      } 
      else
      {
        lib_trace(prcnam,
                  "Invalid value for article no. %d from barcode [%s] position %lu",
                   art_numb,barcode,(unsigned long)xlen-var_len);
         return FAILURE;
      }
    }

/* Loop for next article number until all done */

    b_ptr = b_ptr + itm_len;
    /* Ignore previous terminators */
    if (b_ptr[0] != '\0' && iscntrl(b_ptr[0]))
    {
      b_ptr++;
    }
    if (b_ptr[0] == '[' && got_map_cntrl)
    {
      b_ptr+=6;
    }
  }

  /* some sites need to make ean code 16 chars by padding it with zeros */
  if (padding == TRUE)
  {
    if (strlen(item) < 16 && strlen(item) > 10)
    {
      int i;

      for (i = (int)strlen(item); i < 16; i++)
        item[i] = '0';

      item[16] = '\0';
    }
  }

  if (strlen(l_batref) > 0)
  {
	  strncpy(batref, l_batref, BATREF_SIZE - 1);
	  batref[BATREF_SIZE - 1] = '\0';
  }

  return EAN_BARCODE;
}


int lib_validate_sscc(const char *prcnam, int step, const char *sscode)
/*---------------------------------------------------------------------
  Function : lib_validate_sscc

  Description: Validate 18 character is correct and that check digit
               is OK

  Parameters:
              char *prcnam, int step  Details for error messages
              char *sscode   SSCC to check

  Returns:
              SUCCESS if SSCC is all OK
              FAILURE if SSCC is invalid

---------------------------------------------------------------------*/
{
int check_digit = 0;
int ix;

  for (ix = 0 ; ix < 17 ; ix++)
  {
    if (! isdigit(sscode[ix]))
    {
      lib_trace(prcnam,
        "posn %d of %s in not a digit", ix+1, sscode);
      return FAILURE;
    }
    if ( ix % 2 == 0)
      check_digit += (sscode[ix] - '0') * 3;
    else
      check_digit += (sscode[ix] - '0');
  }
  check_digit = check_digit % 10;
  if (check_digit != 0) check_digit = 10 - check_digit;
  check_digit += '0';
  if (sscode[17] != check_digit)
  {
    lib_trace(prcnam,
      "SSCC %s check %c calc %c",sscode,sscode[17],check_digit);
    return FAILURE;
  }
  return SUCCESS;
}


int lib_validate_date_string(const char *prcnam, int step, const char *datestr)
/*---------------------------------------------------------------------
  Function : lib_validate_date_string

  Description: Validate 10 character date string is a valid date
               in format 'YYYY-MM-DD'

  Parameters:
              char *prcnam, int step  Details for error messages
              char *datestr   Date string

  Returns:
              SUCCESS if date is all OK
              FAILURE if date is invalid

---------------------------------------------------------------------*/
{
int year;
int month;
int day;
int months[12] = { 31,28,31, 30,31,30, 31,31,30, 31,30,31 };

  year = atoi(datestr);
  month = atoi(&datestr[5]);
  day = atoi(&datestr[8]);

  if (year % 4 == 0) months[1] = 29; /* Leap February */
  if (year  < 1980 || year  > 2079
   || month < 1    || month > 12
   || day   < 1    || day   > months[month-1])
  {
    lib_trace ( prcnam,
      "Date string %s is invalid year %d month %d day %d",
       datestr, year, month, day);
    return FAILURE;
  }
  return SUCCESS;
}


void lib_convert_French_bbdate(const char *prcnam, char *bbdate)
/*---------------------------------------------------------------------
  Function : lib_convert_French_bbdate

  Description: If DD of bbdate (in format 'YYYY-MM-DD') is '00'
               then convert to last day of month, to handle
               French barcodes.

  Parameters:
              char *bbdate   Date string

---------------------------------------------------------------------*/
{
int year;
int month;
int day;
int months[12] = { 31,28,31, 30,31,30, 31,31,30, 31,30,31 };
char daytxt[3] = "99\0";

  year = atoi(bbdate);
  month = atoi(&bbdate[5]);
  day = atoi(&bbdate[8]);

  if(debug)
    lib_trace ( prcnam,
      "Date string [%s] before French conv %d mon %d day %d",
       bbdate, year, month, day);

  if (year % 4 == 0) months[1] = 29; /* Leap February */
  if (day == 0)
  {
    day = months[month-1];
    sprintf(daytxt, "%02d", day);
    bbdate[8] = daytxt[0];
    bbdate[9] = daytxt[1];
  }
}


int lib_decode_verify_delivery_label(const char *prcnam, int step, char *pal_sscode,
				                     char *barcode, char *scan_palnum)
/*================================================================
Function Name :  lib_decode_verify_delivery_label

      Purpose :  To extract sscc,item codes cases count and best before date
                 from non-ean standard barcodes

      Returns : SUCCESS if barcode is valid
                FAILURE if barcode invalid (error message generated) 

================================================================*/
{
  const char fname[] = "lib_decode_verify_delivery_label";
  char *b_ptr = barcode;
  int art_numb;
  int var_len;
  int itm_len;
  int endpt;
  char palnum[PALNUM_SIZE+1];
  int len;
  int retval;

  if ( debug > 2 )
  {
    lib_trace(prcnam, "in %s: for pallet [%s] scanned label [%s]",
              fname, pal_sscode, barcode);
  }
  /* First try project-specific barcode format.
   * If fail then try the generic style */

  retval = lib_decode_verify_delivery_label_extra(prcnam, step, pal_sscode, 
						 barcode, scan_palnum);
  if ( retval == SUCCESS)
  {	   
    if (debug)
       lib_trace(prcnam, "BARCODE decoded by project-specific format");
    return retval;
  }

  if (debug)
     lib_trace(prcnam, "BARCODE didn't decode as project-specific format; try generic format");
  
  palnum[0] = '\0';

  len = (int)strlen(lib_rtrim(barcode));

  if ( len < 11 )
  {
    lib_trace(prcnam,
              "Input string length %d is too short delivery label verify ",
              len);
    return FAILURE; 
  }

  /* Get next article number in barcode */
  while ( b_ptr[0] != '\0' && b_ptr[1] != '\0' )
  {
    art_numb = (b_ptr[0] - '0')*10 + b_ptr[1] - '0';
    b_ptr = b_ptr + 2;

    if ( art_numb >= 40 && art_numb <= 42 )
    {
      art_numb = art_numb*10 + b_ptr[0] - '0';
      b_ptr++;
    }

    var_len = (int)strlen(b_ptr); /* Get variable length */

    if ( debug > 2 )
    {
      lib_trace(prcnam, "decoding art_num [%d] length [%d]", art_numb,
                var_len);
    }

    /* Use article number to select item length and extract any data required */
    switch (art_numb)
    {
      case 92: /* Delivery label validation code */
        itm_len = var_len;
        /* First search for end marker within string */
        endpt = 0;
        while ( b_ptr[endpt] != '\0' && b_ptr[endpt] != 29 )
        {
          endpt++;
        }

        if ( debug > 2 )
        {
          lib_trace(prcnam, "Decode delivery label validation, endpt [%d]",
                    endpt);
        }

        if ( b_ptr[endpt] == 29 )
        {
          strncpy(scan_palnum, b_ptr, endpt);
          scan_palnum[endpt] = '\0';
          itm_len = endpt + 1;
        }
        else if ( var_len > 30 )
        {
          itm_len++;
        }
        else
        {
          strncpy(scan_palnum, b_ptr, itm_len);
          scan_palnum[itm_len] = '\0';
        }

        if ( debug > 2 )
        {
          lib_trace(prcnam, "App id 92 got palnum [%s]", scan_palnum);
        }
        break;

      /* Anything else means we've not read a valid delivery label */

	  default:
        lib_trace(prcnam,
                  "Invalid article number [%d] from barcode [%s] position [%lu]",
                  art_numb, barcode, ((unsigned long)strlen(barcode) - var_len - 2));
        return FAILURE;
    }

    if ( itm_len > var_len || itm_len == 0 )
    {
	  size_t xlen = strlen(barcode);
      lib_trace(prcnam,
                "Invalid value for article no. [%d] from barcode [%s] position [%lu]",
                art_numb, barcode, ((unsigned long)xlen - var_len));
      return FAILURE;
    }

    /* Loop for next article number until all done */
    b_ptr = b_ptr + itm_len;
  }

  strncpy(palnum, &pal_sscode[8], PALNUM_SIZE-1);
  /* barcode has a "@" char tacked on the end to make it harder for the users
     to key it in manually                                                   */
  strcat(palnum, "@");
  palnum[PALNUM_SIZE] = '\0';

  /* and also check that check digit is valid */
  if ( strcmp(scan_palnum, palnum) != 0 )
  {
    if ( debug > 1 )
    {
      lib_trace(prcnam, "palnum found in barcode [%s] doesn't match palnum [%s]",
                barcode, palnum);
    }
    return FAILURE;
  }

  if ( debug > 2 )
  {
    lib_trace(prcnam, "palnum found in barcode [%s] matches sscode [%s]",
              barcode, palnum);
  }

  return SUCCESS;
}


static int my_pow(int base, int power)
/* Returns the number to multiple another number by to raise it by the power passed */
{
  int result= 1;
  while (power > 0)
  {
    result = result * base;
    power--;
  }
  return result;
}


//						↓↓↓VULNERABLE LINES↓↓↓

// 152,4;152,10

// 156,4;156,10

// 157,4;157,10

// 158,4;158,10

// 159,4;159,10

// 160,4;160,10

// 161,1;161,7

// 162,4;162,10

// 165,6;165,12

// 169,6;169,12

// 344,12;344,18

// 346,12;346,18

// 397,8;397,14

// 769,4;769,11

// 916,2;916,8

