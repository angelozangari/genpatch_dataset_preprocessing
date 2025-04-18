/*
 * sig.c - signal name, and number, conversions
 * Copyright 1998-2003 by Albert Cahalan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sig.h"
#include "c.h"

/* Linux signals:
 *
 * SIGSYS is required by Unix98.
 * SIGEMT is part of SysV, BSD, and ancient UNIX tradition.
 *
 * They are provided by these Linux ports: alpha, mips, sparc, and sparc64.
 * You get SIGSTKFLT and SIGUNUSED instead on i386, m68k, ppc, and arm.
 * (this is a Linux & libc bug -- both must be fixed)
 *
 * Total garbage: SIGIO SIGINFO SIGIOT SIGLOST SIGCLD
 *                 (popular ones are handled as aliases)
 * Nearly garbage: SIGSTKFLT SIGUNUSED (nothing else to fill slots)
 */

/* Linux 2.3.29 replaces SIGUNUSED with the standard SIGSYS signal */
#ifndef SIGSYS
#  warning Standards require that <signal.h> define SIGSYS
#  define SIGSYS SIGUNUSED
#endif

/* If we see both, it is likely SIGSTKFLT (junk) was replaced. */
#ifdef SIGEMT
#  undef SIGSTKFLT
#endif

#ifndef SIGRTMIN
#  warning Standards require that <signal.h> define SIGRTMIN; assuming 32
#  define SIGRTMIN 32
#endif

/* It seems the SPARC libc does not know the kernel supports SIGPWR. */
#ifndef SIGPWR
#  warning Your header files lack SIGPWR. (assuming it is number 29)
#  define SIGPWR 29
#endif

typedef struct mapstruct {
  const char *name;
  int num;
} mapstruct;


static const mapstruct sigtable[] = {
  {"ABRT",   SIGABRT},  /* IOT */
  {"ALRM",   SIGALRM},
  {"BUS",    SIGBUS},
  {"CHLD",   SIGCHLD},  /* CLD */
  {"CONT",   SIGCONT},
#ifdef SIGEMT
  {"EMT",    SIGEMT},
#endif
  {"FPE",    SIGFPE},
  {"HUP",    SIGHUP},
  {"ILL",    SIGILL},
  {"INT",    SIGINT},
  {"KILL",   SIGKILL},
  {"PIPE",   SIGPIPE},
  {"POLL",   SIGPOLL},  /* IO */
  {"PROF",   SIGPROF},
  {"PWR",    SIGPWR},
  {"QUIT",   SIGQUIT},
  {"SEGV",   SIGSEGV},
#ifdef SIGSTKFLT
  {"STKFLT", SIGSTKFLT},
#endif
  {"STOP",   SIGSTOP},
  {"SYS",    SIGSYS},   /* UNUSED */
  {"TERM",   SIGTERM},
  {"TRAP",   SIGTRAP},
  {"TSTP",   SIGTSTP},
  {"TTIN",   SIGTTIN},
  {"TTOU",   SIGTTOU},
  {"URG",    SIGURG},
  {"USR1",   SIGUSR1},
  {"USR2",   SIGUSR2},
  {"VTALRM", SIGVTALRM},
  {"WINCH",  SIGWINCH},
  {"XCPU",   SIGXCPU},
  {"XFSZ",   SIGXFSZ}
};

static const int number_of_signals = sizeof(sigtable)/sizeof(mapstruct);

static int compare_signal_names(const void *a, const void *b){
  return strcasecmp( ((const mapstruct*)a)->name, ((const mapstruct*)b)->name );
}

/* return -1 on failure */
int signal_name_to_number(const char *restrict name){
  long val;
  int offset;

  /* clean up name */
  if(!strncasecmp(name,"SIG",3)) name += 3;

  if(!strcasecmp(name,"CLD")) return SIGCHLD;
  if(!strcasecmp(name,"IO"))  return SIGPOLL;
  if(!strcasecmp(name,"IOT")) return SIGABRT;

  /* search the table */
  {
    const mapstruct ms = {name,0};
    const mapstruct *restrict const ptr = bsearch(
      &ms,
      sigtable,
      number_of_signals,
      sizeof(mapstruct),
      compare_signal_names
    );
    if(ptr) return ptr->num;
  }

  if(!strcasecmp(name,"RTMIN")) return SIGRTMIN;
  if(!strcasecmp(name,"EXIT"))  return 0;
  if(!strcasecmp(name,"NULL"))  return 0;

  offset = 0;
  if(!strncasecmp(name,"RTMIN+",6)){
    name += 6;
    offset = SIGRTMIN;
  }

  /* not found, so try as a number */
  {
    char *endp;
    val = strtol(name,&endp,10);
    if(*endp || endp==name) return -1; /* not valid */
  }
  if(val+SIGRTMIN>127) return -1; /* not valid */
  return val+offset;
}

const char *signal_number_to_name(int signo){
  static char buf[32];
  int n = number_of_signals;
  signo &= 0x7f; /* need to process exit values too */
  while(n--){
    if(sigtable[n].num==signo) return sigtable[n].name;
  }
  if(signo == SIGRTMIN) return "RTMIN";
  if(signo) sprintf(buf, "RTMIN+%d", signo-SIGRTMIN);
  else      strcpy(buf,"0");  /* AIX has NULL; Solaris has EXIT */
  return buf;
}

int print_given_signals(int argc, const char *restrict const *restrict argv, int max_line){
  char buf[1280]; /* 128 signals, "RTMIN+xx" is largest */
  int ret = 0;  /* to be used as exit code by caller */
  int place = 0; /* position on this line */
  int amt;
  if(argc > 128) return 1;
  while(argc--){
    char tmpbuf[16];
    const char *restrict const txt = *argv;
    if(*txt >= '0' && *txt <= '9'){
      long val;
      char *endp;
      val = strtol(txt,&endp,10);
      if(*endp){
        fprintf(stderr, "Signal \"%s\" not known.\n", txt);
        ret = 1;
        goto end;
      }
      amt = sprintf(tmpbuf, "%s", signal_number_to_name(val));
    }else{
      int sno;
      sno = signal_name_to_number(txt);
      if(sno == -1){
        fprintf(stderr, "Signal \"%s\" not known.\n", txt);
        ret = 1;
        goto end;
      }
      amt = sprintf(tmpbuf, "%d", sno);
    }

    if(!place){
      strcpy(buf,tmpbuf);
      place = amt;
      goto end;
    }
    if(amt+place+1 > max_line){
      printf("%s\n", buf);
      strcpy(buf,tmpbuf);
      place = amt;
      goto end;
    }
    sprintf(buf+place, " %s", tmpbuf);
    place += amt+1;
end:
    argv++;
  }
  if(place) printf("%s\n", buf);
  return ret;
}

/* strtosig is similar to print_given_signals() with exception, that
 * this function takes a string, and converts it to a signal name or
 * a number string depending on which way a round conversion is
 * queried.  Non-existing signals return NULL.  Notice that the
 * returned string should be freed after use.
 */
char *strtosig(const char *restrict s){
  char *converted = NULL, *copy, *p, *endp;
  int i, numsignal = 0;

  copy = strdup(s);
  if (!copy)
    xerrx(EXIT_FAILURE, "cannot duplicate string");
  for (p = copy; *p != '\0'; p++)
    *p = toupper(*p);
  p = copy;
  if (p[0] == 'S' && p[1] == 'I' && p[2] == 'G')
    p += 3;
  if (isdigit(*p)){
    numsignal = strtol(s,&endp,10);
    if(*endp || endp==s) return NULL; /* not valid */
  }
  if (numsignal){
    for (i = 0; i < number_of_signals; i++){
      if (numsignal == sigtable[i].num){
	converted = strdup(sigtable[i].name);
	break;
      }
    }
  } else {
    for (i = 0; i < number_of_signals; i++){
      if (strcmp(p, sigtable[i].name) == 0){
	converted = malloc(sizeof(char) * 8);
	if (converted)
	  snprintf(converted, sizeof(converted) - 1, "%d", sigtable[i].num);
	break;
      }
    }
  }
  free(p);
  return converted;
}

void pretty_print_signals(void){
  int i = 0;
  while(++i <= number_of_signals){
    int n;
    n = printf("%2d %s", i, signal_number_to_name(i));
    if(n>0 && i%7) printf("%s", "           \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" + n);
    else printf("\n");
  }
  if((i-1)%7) printf("\n");
}

void unix_print_signals(void){
  int pos = 0;
  int i = 0;
  while(++i <= number_of_signals){
    if(i-1) printf("%c", (pos>73)?(pos=0,'\n'):(pos++,' ') );
    pos += printf("%s", signal_number_to_name(i));
  }
  printf("\n");
}

/* sanity check */
static int init_signal_list(void) __attribute__((constructor));
static int init_signal_list(void){
  if(number_of_signals != 31){
    fprintf(stderr, "WARNING: %d signals -- adjust and recompile.\n", number_of_signals);
  }
  return 0;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 168,12;168,19

// 169,12;169,18

// 191,12;191,19

// 200,12;200,19

// 204,6;204,12

// 210,6;210,12

// 214,4;214,11

