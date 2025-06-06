/*
 * utils.c
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/pkt_sched.h>
#include <linux/param.h>
#include <linux/if_arp.h>
#include <linux/mpls.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include "rt_names.h"
#include "utils.h"
#include "namespace.h"

int timestamp_short = 0;

int get_hex(char c)
{
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= '0' && c <= '9')
		return c - '0';

	return -1;
}

int get_integer(int *val, const char *arg, int base)
{
	long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;

	res = strtol(arg, &ptr, base);

	/* If there were no digits at all, strtol()  stores
         * the original value of nptr in *endptr (and returns 0).
	 * In particular, if *nptr is not '\0' but **endptr is '\0' on return,
	 * the entire string is valid.
	 */
	if (!ptr || ptr == arg || *ptr)
		return -1;

	/* If an underflow occurs, strtol() returns LONG_MIN.
	 * If an overflow occurs,  strtol() returns LONG_MAX.
	 * In both cases, errno is set to ERANGE.
	 */
	if ((res == LONG_MAX || res == LONG_MIN) && errno == ERANGE)
		return -1;

	/* Outside range of int */
	if (res < INT_MIN || res > INT_MAX)
		return -1;

	*val = res;
	return 0;
}

int mask2bits(__u32 netmask)
{
	unsigned bits = 0;
	__u32 mask = ntohl(netmask);
	__u32 host = ~mask;

	/* a valid netmask must be 2^n - 1 */
	if ((host & (host + 1)) != 0)
		return -1;

	for (; mask; mask <<= 1)
		++bits;
	return bits;
}

static int get_netmask(unsigned *val, const char *arg, int base)
{
	inet_prefix addr;

	if (!get_unsigned(val, arg, base))
		return 0;

	/* try coverting dotted quad to CIDR */
	if (!get_addr_1(&addr, arg, AF_INET) && addr.family == AF_INET) {
		int b = mask2bits(addr.data[0]);

		if (b >= 0) {
			*val = b;
			return 0;
		}
	}

	return -1;
}

int get_unsigned(unsigned *val, const char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;

	res = strtoul(arg, &ptr, base);

	/* empty string or trailing non-digits */
	if (!ptr || ptr == arg || *ptr)
		return -1;

	/* overflow */
	if (res == ULONG_MAX && errno == ERANGE)
		return -1;

	/* out side range of unsigned */
	if (res > UINT_MAX)
		return -1;

	*val = res;
	return 0;
}

/*
 * get_time_rtt is "translated" from a similar routine "get_time" in
 * tc_util.c.  We don't use the exact same routine because tc passes
 * microseconds to the kernel and the callers of get_time_rtt want to
 * pass milliseconds (standard unit for rtt values since 2.6.27), and
 * have a different assumption for the units of a "raw" number.
 */
int get_time_rtt(unsigned *val, const char *arg, int *raw)
{
	double t;
	unsigned long res;
	char *p;

	if (strchr(arg, '.') != NULL) {
		t = strtod(arg, &p);
		if (t < 0.0)
			return -1;

		/* no digits? */
		if (!p || p == arg)
			return -1;

		/* over/underflow */
		if ((t == HUGE_VALF || t == HUGE_VALL) && errno == ERANGE)
			return -1;
	} else {
		res = strtoul(arg, &p, 0);

		/* empty string? */
		if (!p || p == arg)
			return -1;

		/* overflow */
		if (res == ULONG_MAX && errno == ERANGE)
			return -1;

		t = (double)res;
	}

	if (p == arg)
		return -1;
	*raw = 1;

	if (*p) {
		*raw = 0;
                if (strcasecmp(p, "s") == 0 || strcasecmp(p, "sec")==0 ||
                    strcasecmp(p, "secs")==0)
                        t *= 1000;
                else if (strcasecmp(p, "ms") == 0 || strcasecmp(p, "msec")==0 ||
                         strcasecmp(p, "msecs") == 0)
			t *= 1.0; /* allow suffix, do nothing */
                else
                        return -1;
        }

	/* emulate ceil() without having to bring-in -lm and always be >= 1 */

	*val = t;
	if (*val < t)
		*val += 1;

        return 0;

}

int get_u64(__u64 *val, const char *arg, int base)
{
	unsigned long long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;

	res = strtoull(arg, &ptr, base);

	/* empty string or trailing non-digits */
	if (!ptr || ptr == arg || *ptr)
		return -1;

	/* overflow */
	if (res == ULLONG_MAX && errno == ERANGE)
		return -1;

	/* in case ULL is 128 bits */
	if (res > 0xFFFFFFFFFFFFFFFFULL)
		return -1;

 	*val = res;
 	return 0;
}

int get_u32(__u32 *val, const char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtoul(arg, &ptr, base);

	/* empty string or trailing non-digits */
	if (!ptr || ptr == arg || *ptr)
		return -1;

	/* overflow */
	if (res == ULONG_MAX && errno == ERANGE)
		return -1;

	/* in case UL > 32 bits */
	if (res > 0xFFFFFFFFUL)
		return -1;

	*val = res;
	return 0;
}

int get_u16(__u16 *val, const char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtoul(arg, &ptr, base);

	/* empty string or trailing non-digits */
	if (!ptr || ptr == arg || *ptr)
		return -1;

	/* overflow */
	if (res == ULONG_MAX && errno == ERANGE)
		return -1;

	if (res > 0xFFFFUL)
		return -1;

	*val = res;
	return 0;
}

int get_u8(__u8 *val, const char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;

	res = strtoul(arg, &ptr, base);
	/* empty string or trailing non-digits */
	if (!ptr || ptr == arg || *ptr)
		return -1;

	/* overflow */
	if (res == ULONG_MAX && errno == ERANGE)
		return -1;

	if (res > 0xFFUL)
		return -1;

	*val = res;
	return 0;
}

int get_s32(__s32 *val, const char *arg, int base)
{
	long res;
	char *ptr;

	errno = 0;

	if (!arg || !*arg)
		return -1;
	res = strtol(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr)
		return -1;
	if ((res == LONG_MIN || res == LONG_MAX) && errno == ERANGE)
		return -1;
	if (res > INT32_MAX || res < INT32_MIN)
		return -1;

	*val = res;
	return 0;
}

int get_s16(__s16 *val, const char *arg, int base)
{
	long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtol(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr)
		return -1;
	if ((res == LONG_MIN || res == LONG_MAX) && errno == ERANGE)
		return -1;
	if (res > 0x7FFF || res < -0x8000)
		return -1;

	*val = res;
	return 0;
}

int get_s8(__s8 *val, const char *arg, int base)
{
	long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtol(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr)
		return -1;
	if ((res == LONG_MIN || res == LONG_MAX) && errno == ERANGE)
		return -1;
	if (res > 0x7F || res < -0x80)
		return -1;
	*val = res;
	return 0;
}

int get_be64(__be64 *val, const char *arg, int base)
{
	__u64 v;
	int ret = get_u64(&v, arg, base);

	if (!ret)
		*val = htonll(v);

	return ret;
}

int get_be32(__be32 *val, const char *arg, int base)
{
	__u32 v;
	int ret = get_u32(&v, arg, base);

	if (!ret)
		*val = htonl(v);

	return ret;
}

int get_be16(__be16 *val, const char *arg, int base)
{
	__u16 v;
	int ret = get_u16(&v, arg, base);

	if (!ret)
		*val = htons(v);

	return ret;
}

/* This uses a non-standard parsing (ie not inet_aton, or inet_pton)
 * because of legacy choice to parse 10.8 as 10.8.0.0 not 10.0.0.8
 */
static int get_addr_ipv4(__u8 *ap, const char *cp)
{
	int i;

	for (i = 0; i < 4; i++) {
		unsigned long n;
		char *endp;

		n = strtoul(cp, &endp, 0);
		if (n > 255)
			return -1;	/* bogus network value */

		if (endp == cp) /* no digits */
			return -1;

		ap[i] = n;

		if (*endp == '\0')
			break;

		if (i == 3 || *endp != '.')
			return -1; 	/* extra characters */
		cp = endp + 1;
	}

	return 1;
}

int get_addr64(__u64 *ap, const char *cp)
{
	int i;

	union {
		__u16 v16[4];
		__u64 v64;
	} val;

	for (i = 0; i < 4; i++) {
		unsigned long n;
		char *endp;

		n = strtoul(cp, &endp, 16);
		if (n > 0xffff)
			return -1;	/* bogus network value */

		if (endp == cp) /* no digits */
			return -1;

		val.v16[i] = htons(n);

		if (*endp == '\0')
			break;

		if (i == 3 || *endp != ':')
			return -1;	/* extra characters */
		cp = endp + 1;
	}

	*ap = val.v64;

	return 1;
}

int get_addr_1(inet_prefix *addr, const char *name, int family)
{
	memset(addr, 0, sizeof(*addr));

	if (strcmp(name, "default") == 0 ||
	    strcmp(name, "all") == 0 ||
	    strcmp(name, "any") == 0) {
		if ((family == AF_DECnet) || (family == AF_MPLS))
			return -1;
		addr->family = family;
		addr->bytelen = (family == AF_INET6 ? 16 : 4);
		addr->bitlen = -1;
		return 0;
	}

	if (family == AF_PACKET) {
		int len;
		len = ll_addr_a2n((char *)&addr->data, sizeof(addr->data), name);
		if (len < 0)
			return -1;

		addr->family = AF_PACKET;
		addr->bytelen = len;
		addr->bitlen = len * 8;
		return 0;
	}

	if (strchr(name, ':')) {
		addr->family = AF_INET6;
		if (family != AF_UNSPEC && family != AF_INET6)
			return -1;
		if (inet_pton(AF_INET6, name, addr->data) <= 0)
			return -1;
		addr->bytelen = 16;
		addr->bitlen = -1;
		return 0;
	}

	if (family == AF_DECnet) {
		struct dn_naddr dna;
		addr->family = AF_DECnet;
		if (dnet_pton(AF_DECnet, name, &dna) <= 0)
			return -1;
		memcpy(addr->data, dna.a_addr, 2);
		addr->bytelen = 2;
		addr->bitlen = -1;
		return 0;
	}

	if (family == AF_MPLS) {
		int i;
		addr->family = AF_MPLS;
		if (mpls_pton(AF_MPLS, name, addr->data) <= 0)
			return -1;
		addr->bytelen = 4;
		addr->bitlen = 20;
		/* How many bytes do I need? */
		for (i = 0; i < 8; i++) {
			if (ntohl(addr->data[i]) & MPLS_LS_S_MASK) {
				addr->bytelen = (i + 1)*4;
				break;
			}
		}
		return 0;
	}

	addr->family = AF_INET;
	if (family != AF_UNSPEC && family != AF_INET)
		return -1;

	if (get_addr_ipv4((__u8 *)addr->data, name) <= 0)
		return -1;

	addr->bytelen = 4;
	addr->bitlen = -1;
	return 0;
}

int af_bit_len(int af)
{
	switch (af) {
	case AF_INET6:
		return 128;
	case AF_INET:
		return 32;
	case AF_DECnet:
		return 16;
	case AF_IPX:
		return 80;
	case AF_MPLS:
		return 20;
	}

	return 0;
}

int af_byte_len(int af)
{
	return af_bit_len(af) / 8;
}

int get_prefix_1(inet_prefix *dst, char *arg, int family)
{
	int err;
	unsigned plen;
	char *slash;

	memset(dst, 0, sizeof(*dst));

	if (strcmp(arg, "default") == 0 ||
	    strcmp(arg, "any") == 0 ||
	    strcmp(arg, "all") == 0) {
		if ((family == AF_DECnet) || (family == AF_MPLS))
			return -1;
		dst->family = family;
		dst->bytelen = 0;
		dst->bitlen = 0;
		return 0;
	}

	slash = strchr(arg, '/');
	if (slash)
		*slash = 0;

	err = get_addr_1(dst, arg, family);
	if (err == 0) {
		dst->bitlen = af_bit_len(dst->family);

		if (slash) {
			if (get_netmask(&plen, slash+1, 0)
			    || plen > dst->bitlen) {
				err = -1;
				goto done;
			}
			dst->flags |= PREFIXLEN_SPECIFIED;
			dst->bitlen = plen;
		}
	}
done:
	if (slash)
		*slash = '/';
	return err;
}

int get_addr(inet_prefix *dst, const char *arg, int family)
{
	if (get_addr_1(dst, arg, family)) {
		fprintf(stderr, "Error: %s address is expected rather than \"%s\".\n",
				family_name(dst->family) ,arg);
		exit(1);
	}
	return 0;
}

int get_prefix(inet_prefix *dst, char *arg, int family)
{
	if (family == AF_PACKET) {
		fprintf(stderr, "Error: \"%s\" may be inet prefix, but it is not allowed in this context.\n", arg);
		exit(1);
	}

	if (get_prefix_1(dst, arg, family)) {
		fprintf(stderr, "Error: %s prefix is expected rather than \"%s\".\n",
				family_name(dst->family) ,arg);
		exit(1);
	}
	return 0;
}

__u32 get_addr32(const char *name)
{
	inet_prefix addr;
	if (get_addr_1(&addr, name, AF_INET)) {
		fprintf(stderr, "Error: an IP address is expected rather than \"%s\"\n", name);
		exit(1);
	}
	return addr.data[0];
}

void incomplete_command(void)
{
	fprintf(stderr, "Command line is not complete. Try option \"help\"\n");
	exit(-1);
}

void missarg(const char *key)
{
	fprintf(stderr, "Error: argument \"%s\" is required\n", key);
	exit(-1);
}

void invarg(const char *msg, const char *arg)
{
	fprintf(stderr, "Error: argument \"%s\" is wrong: %s\n", arg, msg);
	exit(-1);
}

void duparg(const char *key, const char *arg)
{
	fprintf(stderr, "Error: duplicate \"%s\": \"%s\" is the second value.\n", key, arg);
	exit(-1);
}

void duparg2(const char *key, const char *arg)
{
	fprintf(stderr, "Error: either \"%s\" is duplicate, or \"%s\" is a garbage.\n", key, arg);
	exit(-1);
}

int matches(const char *cmd, const char *pattern)
{
	int len = strlen(cmd);
	if (len > strlen(pattern))
		return -1;
	return memcmp(pattern, cmd, len);
}

int inet_addr_match(const inet_prefix *a, const inet_prefix *b, int bits)
{
	const __u32 *a1 = a->data;
	const __u32 *a2 = b->data;
	int words = bits >> 0x05;

	bits &= 0x1f;

	if (words)
		if (memcmp(a1, a2, words << 2))
			return -1;

	if (bits) {
		__u32 w1, w2;
		__u32 mask;

		w1 = a1[words];
		w2 = a2[words];

		mask = htonl((0xffffffff) << (0x20 - bits));

		if ((w1 ^ w2) & mask)
			return 1;
	}

	return 0;
}

int __iproute2_hz_internal;

int __get_hz(void)
{
	char name[1024];
	int hz = 0;
	FILE *fp;

	if (getenv("HZ"))
		return atoi(getenv("HZ")) ? : HZ;

	if (getenv("PROC_NET_PSCHED")) {
		snprintf(name, sizeof(name)-1, "%s", getenv("PROC_NET_PSCHED"));
	} else if (getenv("PROC_ROOT")) {
		snprintf(name, sizeof(name)-1, "%s/net/psched", getenv("PROC_ROOT"));
	} else {
		strcpy(name, "/proc/net/psched");
	}
	fp = fopen(name, "r");

	if (fp) {
		unsigned nom, denom;
		if (fscanf(fp, "%*08x%*08x%08x%08x", &nom, &denom) == 2)
			if (nom == 1000000)
				hz = denom;
		fclose(fp);
	}
	if (hz)
		return hz;
	return HZ;
}

int __iproute2_user_hz_internal;

int __get_user_hz(void)
{
	return sysconf(_SC_CLK_TCK);
}

const char *rt_addr_n2a_r(int af, int len, const void *addr, char *buf, int buflen)
{
	switch (af) {
	case AF_INET:
	case AF_INET6:
		return inet_ntop(af, addr, buf, buflen);
	case AF_MPLS:
		return mpls_ntop(af, addr, buf, buflen);
	case AF_IPX:
		return ipx_ntop(af, addr, buf, buflen);
	case AF_DECnet:
	{
		struct dn_naddr dna = { 2, { 0, 0, }};
		memcpy(dna.a_addr, addr, 2);
		return dnet_ntop(af, &dna, buf, buflen);
	}
	case AF_PACKET:
		return ll_addr_n2a(addr, len, ARPHRD_VOID, buf, buflen);
	default:
		return "???";
	}
}

const char *rt_addr_n2a(int af, int len, const void *addr)
{
	static char buf[256];

	return rt_addr_n2a_r(af, len, addr, buf, 256);
}

int read_family(const char *name)
{
	int family = AF_UNSPEC;
	if (strcmp(name, "inet") == 0)
		family = AF_INET;
	else if (strcmp(name, "inet6") == 0)
		family = AF_INET6;
	else if (strcmp(name, "dnet") == 0)
		family = AF_DECnet;
	else if (strcmp(name, "link") == 0)
		family = AF_PACKET;
	else if (strcmp(name, "ipx") == 0)
		family = AF_IPX;
	else if (strcmp(name, "mpls") == 0)
		family = AF_MPLS;
	else if (strcmp(name, "bridge") == 0)
		family = AF_BRIDGE;
	return family;
}

const char *family_name(int family)
{
	if (family == AF_INET)
		return "inet";
	if (family == AF_INET6)
		return "inet6";
	if (family == AF_DECnet)
		return "dnet";
	if (family == AF_PACKET)
		return "link";
	if (family == AF_IPX)
		return "ipx";
	if (family == AF_MPLS)
		return "mpls";
	if (family == AF_BRIDGE)
		return "bridge";
	return "???";
}

#ifdef RESOLVE_HOSTNAMES
struct namerec
{
	struct namerec *next;
	const char *name;
	inet_prefix addr;
};

#define NHASH 257
static struct namerec *nht[NHASH];

static const char *resolve_address(const void *addr, int len, int af)
{
	struct namerec *n;
	struct hostent *h_ent;
	unsigned hash;
	static int notfirst;


	if (af == AF_INET6 && ((__u32*)addr)[0] == 0 &&
	    ((__u32*)addr)[1] == 0 && ((__u32*)addr)[2] == htonl(0xffff)) {
		af = AF_INET;
		addr += 12;
		len = 4;
	}

	hash = *(__u32 *)(addr + len - 4) % NHASH;

	for (n = nht[hash]; n; n = n->next) {
		if (n->addr.family == af &&
		    n->addr.bytelen == len &&
		    memcmp(n->addr.data, addr, len) == 0)
			return n->name;
	}
	if ((n = malloc(sizeof(*n))) == NULL)
		return NULL;
	n->addr.family = af;
	n->addr.bytelen = len;
	n->name = NULL;
	memcpy(n->addr.data, addr, len);
	n->next = nht[hash];
	nht[hash] = n;
	if (++notfirst == 1)
		sethostent(1);
	fflush(stdout);

	if ((h_ent = gethostbyaddr(addr, len, af)) != NULL)
		n->name = strdup(h_ent->h_name);

	/* Even if we fail, "negative" entry is remembered. */
	return n->name;
}
#endif

const char *format_host_r(int af, int len, const void *addr,
			char *buf, int buflen)
{
#ifdef RESOLVE_HOSTNAMES
	if (resolve_hosts) {
		const char *n;

		len = len <= 0 ? af_byte_len(af) : len;

		if (len > 0 &&
		    (n = resolve_address(addr, len, af)) != NULL)
			return n;
	}
#endif
	return rt_addr_n2a_r(af, len, addr, buf, buflen);
}

const char *format_host(int af, int len, const void *addr)
{
	static char buf[256];

	return format_host_r(af, len, addr, buf, 256);
}


char *hexstring_n2a(const __u8 *str, int len, char *buf, int blen)
{
	char *ptr = buf;
	int i;

	for (i=0; i<len; i++) {
		if (blen < 3)
			break;
		sprintf(ptr, "%02x", str[i]);
		ptr += 2;
		blen -= 2;
	}
	return buf;
}

__u8 *hexstring_a2n(const char *str, __u8 *buf, int blen, unsigned int *len)
{
	unsigned int cnt = 0;
	char *endptr;

	if (strlen(str) % 2)
		return NULL;
	while (cnt < blen && strlen(str) > 1) {
		unsigned int tmp;
		char tmpstr[3];

		strncpy(tmpstr, str, 2);
		tmpstr[2] = '\0';
		errno = 0;
		tmp = strtoul(tmpstr, &endptr, 16);
		if (errno != 0 || tmp > 0xFF || *endptr != '\0')
			return NULL;
		buf[cnt++] = tmp;
		str += 2;
	}

	if (len)
		*len = cnt;

	return buf;
}

int addr64_n2a(__u64 addr, char *buff, size_t len)
{
	__u16 *words = (__u16 *)&addr;
	__u16 v;
	int i, ret;
	size_t written = 0;
	char *sep = ":";

	for (i = 0; i < 4; i++) {
		v = ntohs(words[i]);

		if (i == 3)
			sep = "";

		ret = snprintf(&buff[written], len - written, "%x%s", v, sep);
		if (ret < 0)
			return ret;

		written += ret;
	}

	return written;
}

int print_timestamp(FILE *fp)
{
	struct timeval tv;
	struct tm *tm;

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);

	if (timestamp_short) {
		char tshort[40];

		strftime(tshort, sizeof(tshort), "%Y-%m-%dT%H:%M:%S", tm);
		fprintf(fp, "[%s.%06ld] ", tshort, tv.tv_usec);
	} else {
		char *tstr = asctime(tm);

		tstr[strlen(tstr)-1] = 0;
		fprintf(fp, "Timestamp: %s %ld usec\n",
			tstr, tv.tv_usec);
	}

	return 0;
}

int cmdlineno;

/* Like glibc getline but handle continuation lines and comments */
ssize_t getcmdline(char **linep, size_t *lenp, FILE *in)
{
	ssize_t cc;
	char *cp;

	if ((cc = getline(linep, lenp, in)) < 0)
		return cc;	/* eof or error */
	++cmdlineno;

	cp = strchr(*linep, '#');
	if (cp)
		*cp = '\0';

	while ((cp = strstr(*linep, "\\\n")) != NULL) {
		char *line1 = NULL;
		size_t len1 = 0;
		ssize_t cc1;

		if ((cc1 = getline(&line1, &len1, in)) < 0) {
			fprintf(stderr, "Missing continuation line\n");
			return cc1;
		}

		++cmdlineno;
		*cp = 0;

		cp = strchr(line1, '#');
		if (cp)
			*cp = '\0';

		*lenp = strlen(*linep) + strlen(line1) + 1;
		*linep = realloc(*linep, *lenp);
		if (!*linep) {
			fprintf(stderr, "Out of memory\n");
			*lenp = 0;
			return -1;
		}
		cc += cc1 - 2;
		strcat(*linep, line1);
		free(line1);
	}
	return cc;
}

/* split command line into argument vector */
int makeargs(char *line, char *argv[], int maxargs)
{
	static const char ws[] = " \t\r\n";
	char *cp;
	int argc = 0;

	for (cp = line + strspn(line, ws); *cp; cp += strspn(cp, ws)) {
		if (argc >= (maxargs - 1)) {
			fprintf(stderr, "Too many arguments to command\n");
			exit(1);
		}

		/* word begins with quote */
		if (*cp == '\'' || *cp == '"') {
			char quote = *cp++;

			argv[argc++] = cp;
			/* find ending quote */
			cp = strchr(cp, quote);
			if (cp == NULL) {
				fprintf(stderr, "Unterminated quoted string\n");
				exit(1);
			}
			*cp++ = 0;
			continue;
		}

		argv[argc++] = cp;
		/* find end of word */
		cp += strcspn(cp, ws);
		*cp++ = 0;
	}
	argv[argc] = NULL;

	return argc;
}

int inet_get_addr(const char *src, __u32 *dst, struct in6_addr *dst6)
{
	if (strchr(src, ':'))
		return inet_pton(AF_INET6, src, dst6);
	else
		return inet_pton(AF_INET, src, dst);
}

void print_nlmsg_timestamp(FILE *fp, const struct nlmsghdr *n)
{
	char *tstr;
	time_t secs = ((__u32*)NLMSG_DATA(n))[0];
	long usecs = ((__u32*)NLMSG_DATA(n))[1];
	tstr = asctime(localtime(&secs));
	tstr[strlen(tstr)-1] = 0;
	fprintf(fp, "Timestamp: %s %lu us\n", tstr, usecs);
}

static int on_netns(char *nsname, void *arg)
{
	struct netns_func *f = arg;

	if (netns_switch(nsname))
		return -1;

	return f->func(nsname, f->arg);
}

static int on_netns_label(char *nsname, void *arg)
{
	printf("\nnetns: %s\n", nsname);
	return on_netns(nsname, arg);
}

int do_each_netns(int (*func)(char *nsname, void *arg), void *arg,
		bool show_label)
{
	struct netns_func nsf = { .func = func, .arg = arg };

	if (show_label)
		return netns_foreach(on_netns_label, &nsf);

	return netns_foreach(on_netns, &nsf);
}

char *int_to_str(int val, char *buf)
{
	sprintf(buf, "%d", val);
	return buf;
}

int get_guid(__u64 *guid, const char *arg)
{
	unsigned long int tmp;
	char *endptr;
	int i;

#define GUID_STR_LEN 23
	/* Verify strict format: format string must be
	 * xx:xx:xx:xx:xx:xx:xx:xx where xx can be an arbitrary
	 * hex digit
	 */

	if (strlen(arg) != GUID_STR_LEN)
		return -1;

	/* make sure columns are in place */
	for (i = 0; i < 7; i++)
		if (arg[2 + i * 3] != ':')
			return -1;

	*guid = 0;
	for (i = 0; i < 8; i++) {
		tmp = strtoul(arg + i * 3, &endptr, 16);
		if (endptr != arg + i * 3 + 2)
			return -1;

		if (tmp > 255)
			return -1;

		 *guid |= tmp << (56 - 8 * i);
	}

	return 0;
}

/* This is a necessary workaround for multicast route dumps */
int get_real_family(int rtm_type, int rtm_family)
{
	if (rtm_type != RTN_MULTICAST)
		return rtm_family;

	return rtm_family == RTNL_FAMILY_IPMR ? AF_INET : AF_INET6;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 728,2;728,8

// 907,2;907,9

// 1030,2;1030,8

// 1121,1;1121,8

