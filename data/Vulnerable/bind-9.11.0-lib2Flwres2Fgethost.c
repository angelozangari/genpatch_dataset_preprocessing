/*
 * Copyright (C) 2000, 2001, 2004, 2005, 2007, 2013-2016  Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* $Id: gethost.c,v 1.34 2007/06/19 23:47:22 tbox Exp $ */

/*! \file */

/**
 *    These functions provide hostname-to-address and address-to-hostname
 *    lookups by means of the lightweight resolver. They are similar to the
 *    standard gethostent(3) functions provided by most operating systems.
 *    They use a struct hostent which is usually defined in <namedb.h>.
 *
 * \code
 * struct  hostent {
 *         char    *h_name;        // official name of host
 * 	   char    **h_aliases;    // alias list
 *         int     h_addrtype;     // host address type
 *         int     h_length;       // length of address
 *         char    **h_addr_list;  // list of addresses from name server
 * };
 * #define h_addr  h_addr_list[0]  // address, for backward compatibility
 * \endcode
 *
 *    The members of this structure are:
 *
 * \li   h_name:
 *           The official (canonical) name of the host.
 *
 * \li   h_aliases:
 *           A NULL-terminated array of alternate names (nicknames) for the
 *           host.
 *
 * \li   h_addrtype:
 *           The type of address being returned -- PF_INET or PF_INET6.
 *
 * \li   h_length:
 *           The length of the address in bytes.
 *
 * \li   h_addr_list:
 *           A NULL terminated array of network addresses for the host. Host
 *           addresses are returned in network byte order.
 *
 *    For backward compatibility with very old software, h_addr is the first
 *    address in h_addr_list.
 *
 *    lwres_gethostent(), lwres_sethostent(), lwres_endhostent(),
 *    lwres_gethostent_r(), lwres_sethostent_r() and lwres_endhostent_r()
 *    provide iteration over the known host entries on systems that provide
 *    such functionality through facilities like /etc/hosts or NIS. The
 *    lightweight resolver does not currently implement these functions; it
 *    only provides them as stub functions that always return failure.
 *
 *    lwres_gethostbyname() and lwres_gethostbyname2() look up the hostname
 *    name. lwres_gethostbyname() always looks for an IPv4 address while
 *    lwres_gethostbyname2() looks for an address of protocol family af:
 *    either PF_INET or PF_INET6 -- IPv4 or IPV6 addresses respectively.
 *    Successful calls of the functions return a struct hostent for the name
 *    that was looked up. NULL is returned if the lookups by
 *    lwres_gethostbyname() or lwres_gethostbyname2() fail.
 *
 *    Reverse lookups of addresses are performed by lwres_gethostbyaddr().
 *    addr is an address of length len bytes and protocol family type --
 *    PF_INET or PF_INET6. lwres_gethostbyname_r() is a thread-safe function
 *    for forward lookups. If an error occurs, an error code is returned in
 *    *error. resbuf is a pointer to a struct hostent which is initialised
 *    by a successful call to lwres_gethostbyname_r() . buf is a buffer of
 *    length len bytes which is used to store the h_name, h_aliases, and
 *    h_addr_list elements of the struct hostent returned in resbuf.
 *    Successful calls to lwres_gethostbyname_r() return resbuf, which is a
 *    pointer to the struct hostent it created.
 *
 *    lwres_gethostbyaddr_r() is a thread-safe function that performs a
 *    reverse lookup of address addr which is len bytes long and is of
 *    protocol family type -- PF_INET or PF_INET6. If an error occurs, the
 *    error code is returned in *error. The other function parameters are
 *    identical to those in lwres_gethostbyname_r(). resbuf is a pointer to
 *    a struct hostent which is initialised by a successful call to
 *    lwres_gethostbyaddr_r(). buf is a buffer of length len bytes which is
 *    used to store the h_name, h_aliases, and h_addr_list elements of the
 *    struct hostent returned in resbuf. Successful calls to
 *    lwres_gethostbyaddr_r() return resbuf, which is a pointer to the
 *    struct hostent it created.
 *
 * \section gethost_return Return Values
 *
 *    The functions lwres_gethostbyname(), lwres_gethostbyname2(),
 *    lwres_gethostbyaddr(), and lwres_gethostent() return NULL to indicate
 *    an error. In this case the global variable lwres_h_errno will contain
 *    one of the following error codes defined in \link netdb.h <lwres/netdb.h>:\endlink
 *
 * \li #HOST_NOT_FOUND:
 *           The host or address was not found.
 *
 * \li #TRY_AGAIN:
 *           A recoverable error occurred, e.g., a timeout. Retrying the
 *           lookup may succeed.
 *
 * \li #NO_RECOVERY:
 *           A non-recoverable error occurred.
 *
 * \li #NO_DATA:
 *           The name exists, but has no address information associated with
 *           it (or vice versa in the case of a reverse lookup). The code
 *           NO_ADDRESS is accepted as a synonym for NO_DATA for backwards
 *           compatibility.
 *
 *    lwres_hstrerror() translates these error codes to suitable error
 *    messages.
 *
 *    lwres_gethostent() and lwres_gethostent_r() always return NULL.
 *
 *    Successful calls to lwres_gethostbyname_r() and
 *    lwres_gethostbyaddr_r() return resbuf, a pointer to the struct hostent
 *    that was initialised by these functions. They return NULL if the
 *    lookups fail or if buf was too small to hold the list of addresses and
 *    names referenced by the h_name, h_aliases, and h_addr_list elements of
 *    the struct hostent. If buf was too small, both lwres_gethostbyname_r()
 *    and lwres_gethostbyaddr_r() set the global variable errno to ERANGE.
 *
 * \section gethost_see See Also
 *
 *    gethostent(), \link getipnode.c getipnode\endlink, lwres_hstrerror()
 *
 * \section gethost_bugs Bugs
 *
 *    lwres_gethostbyname(), lwres_gethostbyname2(), lwres_gethostbyaddr()
 *    and lwres_endhostent() are not thread safe; they return pointers to
 *    static data and provide error codes through a global variable.
 *    Thread-safe versions for name and address lookup are provided by
 *    lwres_gethostbyname_r(), and lwres_gethostbyaddr_r() respectively.
 *
 *    The resolver daemon does not currently support any non-DNS name
 *    services such as /etc/hosts or NIS, consequently the above functions
 *    don't, either.
 */

#include <config.h>

#include <errno.h>
#include <string.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h> /* uintptr_t */
#endif

#include <lwres/net.h>
#include <lwres/netdb.h>

#include "assert_p.h"

#define LWRES_ALIGNBYTES (sizeof(char *) - 1)
#define LWRES_ALIGN(p) \
	(((uintptr_t)(p) + LWRES_ALIGNBYTES) &~ LWRES_ALIGNBYTES)

static struct hostent *he = NULL;
static int copytobuf(struct hostent *, struct hostent *, char *, int);

/*% Always looks for an IPv4 address. */
struct hostent *
lwres_gethostbyname(const char *name) {

	if (he != NULL)
		lwres_freehostent(he);

	he = lwres_getipnodebyname(name, AF_INET, 0, &lwres_h_errno);
	return (he);
}

/*% Looks for either an IPv4 or IPv6 address. */
struct hostent *
lwres_gethostbyname2(const char *name, int af) {
	if (he != NULL)
		lwres_freehostent(he);

	he = lwres_getipnodebyname(name, af, 0, &lwres_h_errno);
	return (he);
}

/*% Reverse lookup of addresses. */
struct hostent *
lwres_gethostbyaddr(const char *addr, int len, int type) {

	if (he != NULL)
		lwres_freehostent(he);

	he = lwres_getipnodebyaddr(addr, len, type, &lwres_h_errno);
	return (he);
}

/*% Stub function.  Always returns failure. */
struct hostent *
lwres_gethostent(void) {
	if (he != NULL)
		lwres_freehostent(he);

	return (NULL);
}

/*% Stub function.  Always returns failure. */
void
lwres_sethostent(int stayopen) {
	/*
	 * Empty.
	 */
	UNUSED(stayopen);
}

/*% Stub function.  Always returns failure. */
void
lwres_endhostent(void) {
	/*
	 * Empty.
	 */
}

/*% Thread-safe function for forward lookups. */
struct hostent *
lwres_gethostbyname_r(const char *name, struct hostent *resbuf,
		char *buf, int buflen, int *error)
{
	struct hostent *myhe;
	int res;

	myhe = lwres_getipnodebyname(name, AF_INET, 0, error);
	if (myhe == NULL)
		return (NULL);
	res = copytobuf(myhe, resbuf, buf, buflen);
	lwres_freehostent(myhe);
	if (res != 0) {
		errno = ERANGE;
		return (NULL);
	}
	return (resbuf);
}

/*% Thread-safe reverse lookup. */
struct hostent  *
lwres_gethostbyaddr_r(const char *addr, int len, int type,
		      struct hostent *resbuf, char *buf, int buflen,
		      int *error)
{
	struct hostent *myhe;
	int res;

	myhe = lwres_getipnodebyaddr(addr, len, type, error);
	if (myhe == NULL)
		return (NULL);
	res = copytobuf(myhe, resbuf, buf, buflen);
	lwres_freehostent(myhe);
	if (res != 0) {
		errno = ERANGE;
		return (NULL);
	}
	return (resbuf);
}

/*% Stub function.  Always returns failure. */
struct hostent  *
lwres_gethostent_r(struct hostent *resbuf, char *buf, int buflen, int *error) {
	UNUSED(resbuf);
	UNUSED(buf);
	UNUSED(buflen);
	*error = 0;
	return (NULL);
}

/*% Stub function.  Always returns failure. */
void
lwres_sethostent_r(int stayopen) {
	/*
	 * Empty.
	 */
	UNUSED(stayopen);
}

/*% Stub function.  Always returns failure. */
void
lwres_endhostent_r(void) {
	/*
	 * Empty.
	 */
}

static int
copytobuf(struct hostent *src, struct hostent *hptr, char *buf, int buflen) {
	char *cp;
	char **ptr;
	int i, n;
	int nptr, len;

	/*
	 * Find out the amount of space required to store the answer.
	 */
	nptr = 2; /* NULL ptrs */
	len = (int)((char *)LWRES_ALIGN(buf) - buf);
	for (i = 0; src->h_addr_list[i]; i++, nptr++) {
		len += src->h_length;
	}
	for (i = 0; src->h_aliases[i]; i++, nptr++) {
		len += strlen(src->h_aliases[i]) + 1;
	}
	len += strlen(src->h_name) + 1;
	len += nptr * sizeof(char*);

	if (len > buflen) {
		return (-1);
	}

	/*
	 * Copy address size and type.
	 */
	hptr->h_addrtype = src->h_addrtype;
	n = hptr->h_length = src->h_length;

	ptr = (char **)LWRES_ALIGN(buf);
	cp = (char *)LWRES_ALIGN(buf) + nptr * sizeof(char *);

	/*
	 * Copy address list.
	 */
	hptr->h_addr_list = ptr;
	for (i = 0; src->h_addr_list[i]; i++, ptr++) {
		memmove(cp, src->h_addr_list[i], n);
		hptr->h_addr_list[i] = cp;
		cp += n;
	}
	hptr->h_addr_list[i] = NULL;
	ptr++;

	/*
	 * Copy official name.
	 */
	n = strlen(src->h_name) + 1;
	strcpy(cp, src->h_name);
	hptr->h_name = cp;
	cp += n;

	/*
	 * Copy aliases.
	 */
	hptr->h_aliases = ptr;
	for (i = 0; src->h_aliases[i]; i++) {
		n = strlen(src->h_aliases[i]) + 1;
		strcpy(cp, src->h_aliases[i]);
		hptr->h_aliases[i] = cp;
		cp += n;
	}
	hptr->h_aliases[i] = NULL;

	return (0);
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 339,1;339,7

// 349,2;349,8

