/******************************************************************************/
/*                                                                            */
/* TBA Leicester.                                   Copyright (c) 1996 - 2018 */
/* Registered in England                                   Reg. No. : 2209595 */
/*                                                                            */
/******************************************************************************/
/*

$Workfile: lib_crypt.c $
$Revision: 20 $

*******************************************************************************/

char lib_crypt_sccsid[] = "@(#)$Workfile: lib_crypt.c $ $Revision: 20 $";

/******************************************************************************

Description: CSA In House Encryption utilities
             Use for where reversible / controllable encryption is necessary

*******************************************************************************/

#include "stdafx.h"

/* #include <sys/types.h>  */
#include <stdarg.h>
#include <errno.h>

#include "lib_log_fprint_extern.h"
#include "lib_trace_extern.h"
#if defined _WIN32 || defined _WIN64
#include "astlib.h"
#else
#define csa_getenv getenv
#endif

/* Third-party public-domain implementations of Base64 and SHA-256 */
#include "crypto-algorithms/base64.h"
/*#{CHEAD_CP_START(extern)}*/
#include "crypto-algorithms/sha256.h"
/*#{CHEAD_CP_END}*/

#include "lib_crypt.h"

#define DB_PROTOCOL_PLAIN 0
#define DB_PROTOCOL_OBFUSCATE 1
#define DB_PROTOCOL_HASH 2

#define MIN(x,y) ((x)<(y)?(x):(y))

/* external variables */
extern int debug;

void lib_crypt_db_hash(char *passwd, unsigned *returnLen, const char *hash_uname, BYTE *envSalt, size_t envSaltLen)
  /* In protocol 2, the database password is calculated as the first 20 characters of:

  base64( sha256(fixedSalt + hash_uname + envSalt) )

  where fixedSalt is a universal constant built into this function, hash_uname
  is the passed-in database username, and envSalt is the value of the
  DB_CONNECT environment variable (or Registry value on Windows).  
  DB_CONNECT should be a hexadecimal string, which is packed into raw bytes before use.

  + denotes concatenation.

  */
{
  /* Fixed, universal salt value (obfuscated) */
  BYTE fixedSalt[] = { 0xb3, 0x72, 0x2b, 0x44, 0xef, 0xfa, 0x41, 0xcf,
  0xb3, 0xe4, 0xd4, 0xf1, 0xa8, 0xda, 0x53, 0x3c };

SHA256_CTX ctx;
BYTE hash[SHA256_BLOCK_SIZE];
BYTE phrase[SHA256_BLOCK_SIZE * 2];
size_t phraseLen = 0;

obfuscate(fixedSalt, sizeof(fixedSalt));
/* Result is 211d644552a70edf3e5a98fd04cf53da */

sha256_init(&ctx);

/* Hash the fixed salt */
sha256_update(&ctx, fixedSalt, sizeof(fixedSalt));

/* Hash the username */
sha256_update(&ctx, (BYTE*)hash_uname, strlen(hash_uname));

/* hash the env salt */
sha256_update(&ctx, envSalt, envSaltLen);

/* Finalise the hash and reduce the result to 20 ASCII characters */
sha256_final(&ctx, hash);
phraseLen = base64_encode(hash, phrase, SHA256_BLOCK_SIZE, FALSE, base64_charset_oracle);
*returnLen = (unsigned)MIN(*returnLen, phraseLen);
memcpy(passwd, phrase, *returnLen);
/* Truncate our returned passphrase to 20 chars */
passwd[*returnLen] = '\0';

/*
lib_trace(prcnam, "sha256( 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x + \"%s\" + 0x%s ) = \n"
"0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n"
"pwd = %s",
fixedSalt[0], fixedSalt[1], fixedSalt[2], fixedSalt[3], fixedSalt[4], fixedSalt[5], fixedSalt[6], fixedSalt[7],
fixedSalt[8], fixedSalt[9], fixedSalt[10], fixedSalt[11], fixedSalt[12], fixedSalt[13], fixedSalt[14], fixedSalt[15],
db_uname, db_connect,
hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7],
hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15],
hash[16], hash[17], hash[18], hash[19], hash[20], hash[21], hash[22], hash[23],
hash[24], hash[25], hash[26], hash[27], hash[28], hash[29], hash[30], hash[31],
passwd
);
*/
return;
}

char *lib_crypt_db_passwd(const char *prcnam, const char *db_uname,char *passwd)
/*****************************************************************
*                                                                *
* Function Name : lib_crypt_db_passwd                            *
*                                                                *
* Function      : To return the database password derived from   *
*                 the username, either as encrypted or direct    *
*                 This function mirrors a version in the web     *
*                 service and MUST be kept matching and up to    *
*                 date (in Globals.cs).                          *
*                                                                *
*                 The password is returned via the passwd        *
*                 pointer.  The buffer must be able to fit 20    *
*                 chars plus a \0-terminator, or a copy of the   *
*                 db_uname - whichever is larger.                *
*                                                                *
*                 The return value is simply passwd, or NULL on  *
*                 failure.                                       *
*                                                                *
* Note          : This function must be replicable in the client.*
*                 For further documentation, see                 *
* http://frankie/wiki/index.php/Database_Password_Encryption     *
*                                                                *
*****************************************************************/
{

  const char *db_protocol_env = csa_getenv("DB_PROTOCOL");
  /* The standard password is stored obfuscated here */
  BYTE stdpw[] = { 0xf0, 0x0e, 0x29, 0x70, 0xc9, 0x73, 0x21, '\0' };
  int dbProtocol = 0;
  int tmp = 0;
  size_t plen = strlen(db_uname);
  size_t i;
  char *env_var = NULL;

  /* Determine whether this is a standard user name
  For Oracle standard users (system, sys), we want to return
  specific passwords, not based on current method.
  Hardcoded defaults for these password appear here; these are
  overridden by environment variables (if not set or blank, use
  the default) */
  if (strcmp(db_uname, "system") == 0)
  {
    env_var = getenv("DB_DBASYSTEM_NAME");
    if (env_var == NULL || strlen(lib_rtrim(env_var)) == 0)
    {
      /* Not using a PDB / separate DBA system password */
      env_var = getenv("DB_SYSTEM_CONF");
    }
    else
    {
      if (strcmp(env_var, "system") == 0)
      {
        /* Using a PDB / separate DBA system password */
        env_var = getenv("DB_DBASYSTEM_CONF");
      }
      else
      {
        /* Not using a PDB / separate DBA system password */
        env_var = getenv("DB_SYSTEM_CONF");
      }
    }
    if (env_var == NULL || strlen(lib_rtrim(env_var)) == 0)
    {
      /* Standard system password.  Does this need further control? */
      obfuscate(stdpw, sizeof(stdpw)-1);
      strcpy(passwd, (char*)stdpw);
    }
    else
    {
      strcpy(passwd, env_var);
    }
    return passwd;
  }
  if (strcmp(db_uname, "wmsadm") == 0)
  {
    env_var = getenv("DB_SYSTEM_CONF");
    if (env_var == NULL || strlen(lib_rtrim(env_var)) == 0)
    {
      /* Standard system password.  Does this need further control? */
      obfuscate(stdpw, sizeof(stdpw)-1);
      strcpy(passwd, (char*)stdpw);
    }
    else
    {
      strcpy(passwd, env_var);
    }
    return passwd;
  }
  if (strcmp(db_uname, "sys") == 0)
  {
    env_var = getenv("DB_SYSDBA_CONF");
    if (env_var == NULL || strlen(lib_rtrim(env_var)) == 0)
    {
      /* Standard sys password.  Does this need further control? */
      obfuscate(stdpw, sizeof(stdpw)-1);
      strcpy(passwd, (char*)stdpw);
    }
    else
    {
      strcpy(passwd, env_var);
    }
    return passwd;
  }

  /* End "special user" handling */

  if(db_protocol_env != NULL)
  {
    dbProtocol = atoi(db_protocol_env);
  }

  /*lib_trace(prcnam, "lib_crypt_db_passwd: db_uname[%s]", db_uname);*/


  switch(dbProtocol)
  {

  case DB_PROTOCOL_OBFUSCATE: /* Protocol 1 */
  {
    const char *strKey = "wuArehfCpiwufXhiewBfh3ifuh3ipfh2iopfhifbty5y45y54y54y4y4yy";
    int skey = toupper(db_uname[0]) - 65;

    /* Direct copy first two characters of dbname to password */
    for(i = 0; i < 2; i++)
    {
      tmp = toupper(*(db_uname+i));
      /* Make sure it's printable */
      if(tmp < 33) tmp += 65;
      if(tmp > 126) tmp = 126;
      passwd[i] = tmp;
    }

    /* Now encrypt and append */
    for(i = 0; i < plen; i++)
    {
      tmp = *(strKey+skey+i+1) ^ toupper(*(db_uname+i));
      /* Make sure it's printable */
      if(tmp < 33) tmp += 65;
      if(tmp > 126) tmp = 126;

      /* Special, if the character is a: comma,
      *                                 single quote,
      *                                 chevron >
      *                                 chevron <
      *                                 exclamation mark
      *                                 minus
      *                                 plus
      *                                 double quote
      *                                 dollar
      *                                 percent %
      *                                 asterisk *
      *                                 )
      *                                 (
      *                                 ;
      *                                 ampersand &
      *                                 colon :
      * ...use the unhashed character */
      if(tmp == 44) tmp = toupper(*(db_uname+i));
      if(tmp == 39) tmp = toupper(*(db_uname+i));
      if(tmp == 60) tmp = toupper(*(db_uname+i));
      if(tmp == 62) tmp = toupper(*(db_uname+i));
      if(tmp == 33) tmp = toupper(*(db_uname+i));
      if(tmp == 45) tmp = toupper(*(db_uname+i));
      if(tmp == 43) tmp = toupper(*(db_uname+i));
      if(tmp == 34) tmp = toupper(*(db_uname+i));
      if(tmp == 36) tmp = toupper(*(db_uname+i));
      if(tmp == 37) tmp = toupper(*(db_uname+i));
      if(tmp == 42) tmp = toupper(*(db_uname+i));
      if(tmp == 41) tmp = toupper(*(db_uname+i));
      if(tmp == 40) tmp = toupper(*(db_uname+i));
      if(tmp == 59) tmp = toupper(*(db_uname+i));
      if(tmp == 38) tmp = toupper(*(db_uname+i));
      if(tmp == 58) tmp = toupper(*(db_uname+i));
      if(tmp == 92) tmp = toupper(*(db_uname+i));

      passwd[i+2] = tmp;
    }
    passwd[i+2] = '\0';
    /*lib_trace(prcnam, "Protocol 1: %s", passwd);*/
    break;
  }



  case DB_PROTOCOL_HASH: /* Protocol 2 */
  /* In protocol 2, the database password is calculated as the first 20
     characters of:

     base64( sha256(fixedSalt + db_uname + envSalt) )

     where fixedSalt is a universal constant built into this function, db_uname
     is the passed-in database username, and envSalt is the value of the
     DB_CONNECT environment variable (or Registry value on Windows).  DB_CONNECT
     should be a hexadecimal string, which is packed into raw bytes before use.
     + denotes concatenation.
  */
  {
    const char *hexChars = "0123456789ABCDEF";
    const char *db_connect = csa_getenv("DB_CONNECT");
    BYTE envSalt[SHA256_BLOCK_SIZE*2];
    size_t envSaltLen = 0;
    unsigned i = 0;
    unsigned returnLen = 20;
    
    //Bug 2004 Dev: Database Username needs to be case insensitive
    char hash_uname[60] = "";
    char *sPtr = NULL;

    strcpy(hash_uname, db_uname);

    /* force db_uname to upper case. */
    sPtr = hash_uname;
    while (*sPtr != '\0')
    {
      *sPtr = toupper(*sPtr);
      sPtr++;
    }

    if(db_connect != NULL)
    {
      if( strlen(db_connect) % 2 != 0)
      {
        if(debug)
          lib_trace(prcnam, "Warning: invalid environment salt (odd number of nibbles)");
        db_connect = NULL;
      }
    }

    if(db_connect == NULL)
    {
      if(debug)
        lib_trace(prcnam, "Warning: no environment salt value found");
    }

    /* Grab the environment salt */
    for(i=0; db_connect!=NULL && i<strlen(db_connect) && envSaltLen < sizeof(envSalt); i+=2)
    {
      const char *hexCharLeft = strchr(hexChars, toupper(db_connect[i]));
      const char *hexCharRight = strchr(hexChars, toupper(db_connect[i+1]));
      if(hexCharLeft == NULL || hexCharRight == NULL)
      {
        if(debug)
          lib_trace(prcnam, "Warning: invalid environment salt (non-hexadecimal character)");
      }

      envSalt[envSaltLen] = (BYTE)( ((hexCharLeft-hexChars) << 4)  |  (hexCharRight-hexChars) );
      envSaltLen++;
    }

    /* run the salts+tokens through the Hash Function. */
    lib_crypt_db_hash(passwd, &returnLen, hash_uname, envSalt, envSaltLen);
    break;
  }


  case DB_PROTOCOL_PLAIN: /* Protocol 0 */
  default:
    /* Just copy the db username */
    for(i = 0; i < plen; i++)
    {
      tmp = db_uname[i];
      /* Make sure it's printable */
      if(tmp < 33) tmp += 65;
      if(tmp > 126) tmp = 126;
      passwd[i] = tmp;
    }
    passwd[i] = '\0';

    /*lib_trace(prcnam, "Protocol 0: %s", passwd);*/


  } /* End switch */


  return passwd;
}

void lib_crypt_sha256_tracehash(const char *tracename, size_t b, BYTE *d)
/*
 Trace out the hexadecimal representation of the 32 bytes starting at d.
 b is just an index, printed as a line prefix.
*/
{
  lib_trace(tracename, "MD[%lu] = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    b,
    d[0],  d[1],  d[2],  d[3],  d[4],  d[5],  d[6],  d[7], 
    d[8],  d[9],  d[10], d[11], d[12], d[13], d[14], d[15],
    d[16], d[17], d[18], d[19], d[20], d[21], d[22], d[23],
    d[24], d[25], d[26], d[27], d[28], d[29], d[30], d[31]);
}

int lib_crypt_sha256_validate(const char *tracename)
/* unit-test our implementation of SHA-256
   Trace out the NIST Test Sequence to tracename 
   and compare final digest to that expected.

   http://csrc.nist.gov/groups/STM/cavp/documents/shs/SHAVS.pdf

   SHAVs 6.4, Monte Carlo test, BYTE orientation.

   Seed and final digest taken from reference vectors "SHA256-Monte":

   seed  = 6d1e72ad03ddeb5de891e572e2396f8da015d899ef0e79503152d6010a3fe691
   md99  = 6a912ba4188391a78e6f13d88ed2d14e13afce9db6f7dcbf4a48c24f3db02778

   for (j=0; j<100; j++) {
   MD0 = MD1 = MD2 = Seed; for (i=3; i<1003; i++) {
   Mi = MDi-3 || MDi-2 || MDi-1; MDi = SHA(Mi); } MDj = Seed = MD1002;
   OUTPUT: MDj }

   */

{  
  BYTE seed[] = { 0x6d,0x1e,0x72,0xad,0x03,0xdd,0xeb,0x5d,0xe8,0x91,0xe5,0x72,0xe2,0x39,0x6f,0x8d,0xa0,0x15,0xd8,0x99,0xef,0x0e,0x79,0x50,0x31,0x52,0xd6,0x01,0x0a,0x3f,0xe6,0x91 };
  BYTE md99[] = { 0x6a,0x91,0x2b,0xa4,0x18,0x83,0x91,0xa7,0x8e,0x6f,0x13,0xd8,0x8e,0xd2,0xd1,0x4e,0x13,0xaf,0xce,0x9d,0xb6,0xf7,0xdc,0xbf,0x4a,0x48,0xc2,0x4f,0x3d,0xb0,0x27,0x78 };
  
  BYTE m[1003][96] = { { 0 } };
  BYTE md[1003][32] = { { 0 } };
  
  size_t l = 32;
    
  size_t i = 0;
  size_t j = 0;
  size_t b = 0;

  /* 100 blocks*/
  for (j = 0; j < 100; j++)
  {
    memcpy(md[0], seed, l);
    memcpy(md[1], seed, l);
    memcpy(md[2], seed, l);

    /* iterate 1000 times */
    for (i = 3; i < 1003; i++)
    {

      /* copy L bytes into the m array from the last 3 MD outputs */
      for (b = 0; b < l; b++)
      {
        m[i][b]      = md[i - 3][b];
        m[i][l + b]  = md[i - 2][b];
        m[i][(l * 2) + b] = md[i - 1][b];
      }

      /* hash the message m[i] into MD[i] */
      lib_crypt_sha256((const BYTE *)&m[i], l * 3, (BYTE *)&md[i]);
    }

    memcpy(md[j], md[1002], l);
    memcpy(seed, md[1002], l);

    lib_crypt_sha256_tracehash(tracename, j, (BYTE *)&md[j]);
  }  

  lib_trace(tracename, "Comparing final hash with expected...");
  
  for (b = 0; b < l; b++)
  {
    if (md99[b] != md[99][b])
    {
      lib_trace(tracename, "SHA-256 Validation failed at md[99][%ld]: [0x%02x] != expected [0x%02x]", b, md99[b], md[99][b]);
      return -1;
    }
  }

  lib_trace(tracename, "SHA-256 Validator: NIST Vectors MATCH.");  
  return 0;
}


void lib_crypt_sha256(const BYTE *data, size_t length, BYTE *retHash)
/*
 Calculate the SHA-256 digest of the sequence of bytes starting at
 "data", with length "length", and return it in "retHash".
 retHash must point to a buffer of length >= SHA256_BLOCK_SIZE.
 The output will not be \0-terminated.
*/
{
  SHA256_CTX ctx;

  sha256_init(&ctx);
  sha256_update(&ctx, data, length);
  sha256_final(&ctx, retHash);
}


static void obfuscate(BYTE *data, unsigned length)
/* Change the first "length" bytes of "data" in a predictable way */
{
  const BYTE mixer[] ={ 0x92, 0x6E, 0x49, 0x06, 0xA1, 0x58, 0x49, 0x17,
                        0x85, 0xB5, 0x5A, 0x03, 0x50, 0x18, 0x16, 0xF7 };
  unsigned i = 0;

  for(i=0; i<length; i++)
  {
    data[i] = (data[i] ^ mixer[i%sizeof(mixer)]) + i;
  }
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 182,6;182,12

// 186,6;186,12

// 197,6;197,12

// 201,6;201,12

// 212,6;212,12

// 216,6;216,12

// 325,4;325,10

