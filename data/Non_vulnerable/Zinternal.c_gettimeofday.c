#ifndef ZEPHYR_USES_KERBEROS
   int gettimeofday(struct timeval* p, struct timezone* tz ){
     union {
       long long ns100; /*time since 1 Jan 1601 in 100ns units */
       FILETIME ft;
     } _now;
     GetSystemTimeAsFileTime( &(_now.ft) );
     p->tv_usec=(long)((_now.ns100 / 10LL) % 1000000LL );
     p->tv_sec= (long)((_now.ns100-(116444736000000000LL))/10000000LL);
     return 0;
   }
#ifndef ZEPHYR_USES_KERBEROS
   int gettimeofday(struct timeval* p, struct timezone* tz ){
     union {
       long long ns100; /*time since 1 Jan 1601 in 100ns units */
       FILETIME ft;
     } _now;
     GetSystemTimeAsFileTime( &(_now.ft) );
     p->tv_usec=(long)((_now.ns100 / 10LL) % 1000000LL );
     p->tv_sec= (long)((_now.ns100-(116444736000000000LL))/10000000LL);
     return 0;
   }
