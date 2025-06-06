/*
 * Copyright (C) 2000-2016  Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*! \file */

#include <config.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include <isc/app.h>
#include <isc/netaddr.h>
#include <isc/parseint.h>
#include <isc/print.h>
#include <isc/string.h>
#include <isc/task.h>
#include <isc/util.h>

#include <pk11/site.h>

#include <dns/byaddr.h>
#include <dns/fixedname.h>
#include <dns/masterdump.h>
#include <dns/message.h>
#include <dns/name.h>
#include <dns/rcode.h>
#include <dns/rdata.h>
#include <dns/rdataset.h>
#include <dns/rdatatype.h>
#include <dns/rdataclass.h>
#include <dns/result.h>
#include <dns/tsig.h>

#include <dig/dig.h>

#define ADD_STRING(b, s) { 				\
	if (strlen(s) >= isc_buffer_availablelength(b)) \
		return (ISC_R_NOSPACE); 		\
	else 						\
		isc_buffer_putstr(b, s); 		\
}

#define DIG_MAX_ADDRESSES 20

dig_lookup_t *default_lookup = NULL;

static char *batchname = NULL;
static FILE *batchfp = NULL;
static char *argv0;
static int addresscount = 0;

static char domainopt[DNS_NAME_MAXTEXT];
static char hexcookie[81];

static isc_boolean_t short_form = ISC_FALSE, printcmd = ISC_TRUE,
	ip6_int = ISC_FALSE, plusquest = ISC_FALSE, pluscomm = ISC_FALSE,
	multiline = ISC_FALSE, nottl = ISC_FALSE, noclass = ISC_FALSE,
	onesoa = ISC_FALSE, use_usec = ISC_FALSE,
	nocrypto = ISC_FALSE, ttlunits = ISC_FALSE,
	ipv4only = ISC_FALSE, ipv6only = ISC_FALSE;
static isc_uint32_t splitwidth = 0xffffffff;

/*% rrcomments are neither explicitly enabled nor disabled by default */
static int rrcomments = 0;

/*% opcode text */
static const char * const opcodetext[] = {
	"QUERY",
	"IQUERY",
	"STATUS",
	"RESERVED3",
	"NOTIFY",
	"UPDATE",
	"RESERVED6",
	"RESERVED7",
	"RESERVED8",
	"RESERVED9",
	"RESERVED10",
	"RESERVED11",
	"RESERVED12",
	"RESERVED13",
	"RESERVED14",
	"RESERVED15"
};

static const char *
rcode_totext(dns_rcode_t rcode) {
	static char buf[64];
	isc_buffer_t b;
	isc_result_t result;

	memset(buf, 0, sizeof(buf));
	isc_buffer_init(&b, buf + 1, sizeof(buf) - 2);
	result = dns_rcode_totext(rcode, &b);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	if (strspn(buf + 1, "0123456789") == strlen(buf + 1)) {
		buf[0] = '?';
		return(buf);
	}
	return (buf + 1);
}

/*% print usage */
static void
print_usage(FILE *fp) {
	fputs(
"Usage:  dig [@global-server] [domain] [q-type] [q-class] {q-opt}\n"
"            {global-d-opt} host [@local-server] {local-d-opt}\n"
"            [ host [@local-server] {local-d-opt} [...]]\n", fp);
}

ISC_PLATFORM_NORETURN_PRE static void
usage(void) ISC_PLATFORM_NORETURN_POST;

static void
usage(void) {
	print_usage(stderr);
	fputs("\nUse \"dig -h\" (or \"dig -h | more\") "
	      "for complete list of options\n", stderr);
	exit(1);
}

/*% version */
static void
version(void) {
	fputs("DiG " VERSION "\n", stderr);
}

/*% help */
static void
help(void) {
	print_usage(stdout);
	fputs(
"Where:  domain	  is in the Domain Name System\n"
"        q-class  is one of (in,hs,ch,...) [default: in]\n"
"        q-type   is one of (a,any,mx,ns,soa,hinfo,axfr,txt,...) [default:a]\n"
"                 (Use ixfr=version for type ixfr)\n"
"        q-opt    is one of:\n"
"                 -4                  (use IPv4 query transport only)\n"
"                 -6                  (use IPv6 query transport only)\n"
"                 -b address[#port]   (bind to source address/port)\n"
"                 -c class            (specify query class)\n"
"                 -f filename         (batch mode)\n"
"                 -i                  (use IP6.INT for IPv6 reverse lookups)\n"
"                 -k keyfile          (specify tsig key file)\n"
"                 -m                  (enable memory usage debugging)\n"
"                 -p port             (specify port number)\n"
"                 -q name             (specify query name)\n"
"                 -t type             (specify query type)\n"
"                 -u                  (display times in usec instead of msec)\n"
"                 -x dot-notation     (shortcut for reverse lookups)\n"
"                 -y [hmac:]name:key  (specify named base64 tsig key)\n"
"        d-opt    is of the form +keyword[=value], where keyword is:\n"
"                 +[no]aaflag         (Set AA flag in query (+[no]aaflag))\n"
"                 +[no]aaonly         (Set AA flag in query (+[no]aaflag))\n"
"                 +[no]additional     (Control display of additional section)\n"
"                 +[no]adflag         (Set AD flag in query (default on))\n"
"                 +[no]all            (Set or clear all display flags)\n"
"                 +[no]answer         (Control display of answer section)\n"
"                 +[no]authority      (Control display of authority section)\n"
"                 +[no]badcookie      (Retry BADCOOKIE responses)\n"
"                 +[no]besteffort     (Try to parse even illegal messages)\n"
"                 +bufsize=###        (Set EDNS0 Max UDP packet size)\n"
"                 +[no]cdflag         (Set checking disabled flag in query)\n"
"                 +[no]class          (Control display of class in records)\n"
"                 +[no]cmd            (Control display of command line)\n"
"                 +[no]comments       (Control display of comment lines)\n"
"                 +[no]cookie         (Add a COOKIE option to the request)\n"
"                 +[no]crypto         (Control display of cryptographic "
				       "fields in records)\n"
"                 +[no]defname        (Use search list (+[no]search))\n"
"                 +[no]dnssec         (Request DNSSEC records)\n"
"                 +domain=###         (Set default domainname)\n"
"                 +[no]dscp[=###]     (Set the DSCP value to ### [0..63])\n"
"                 +[no]edns[=###]     (Set EDNS version) [0]\n"
"                 +ednsflags=###      (Set EDNS flag bits)\n"
"                 +[no]ednsnegotiation (Set EDNS version negotiation)\n"
"                 +ednsopt=###[:value] (Send specified EDNS option)\n"
"                 +noednsopt          (Clear list of +ednsopt options)\n"
"                 +[no]expire         (Request time to expire)\n"
"                 +[no]fail           (Don't try next server on SERVFAIL)\n"
"                 +[no]header-only    (Send query without a question section)\n"
"                 +[no]identify       (ID responders in short answers)\n"
"                 +[no]ignore         (Don't revert to TCP for TC responses.)\n"
"                 +[no]keepopen       (Keep the TCP socket open between queries)\n"
"                 +[no]mapped         (Allow mapped IPv4 over IPv6)\n"
"                 +[no]multiline      (Print records in an expanded format)\n"
"                 +ndots=###          (Set search NDOTS value)\n"
"                 +[no]nsid           (Request Name Server ID)\n"
"                 +[no]nssearch       (Search all authoritative nameservers)\n"
"                 +[no]onesoa         (AXFR prints only one soa record)\n"
"                 +[no]opcode=###     (Set the opcode of the request)\n"
"                 +[no]qr             (Print question before sending)\n"
"                 +[no]question       (Control display of question section)\n"
"                 +[no]rdflag         (Recursive mode (+[no]recurse))\n"
"                 +[no]recurse        (Recursive mode (+[no]rdflag))\n"
"                 +retry=###          (Set number of UDP retries) [2]\n"
"                 +[no]rrcomments     (Control display of per-record "
					"comments)\n"
"                 +[no]search         (Set whether to use searchlist)\n"
"                 +[no]short          (Display nothing except short\n"
"                                      form of answer)\n"
"                 +[no]showsearch     (Search with intermediate results)\n"
#ifdef DIG_SIGCHASE
"                 +[no]sigchase       (Chase DNSSEC signatures)\n"
#endif
"                 +[no]split=##       (Split hex/base64 fields into chunks)\n"
"                 +[no]stats          (Control display of statistics)\n"
"                 +subnet=addr        (Set edns-client-subnet option)\n"
"                 +[no]tcp            (TCP mode (+[no]vc))\n"
"                 +timeout=###        (Set query timeout) [5]\n"
#if defined(DIG_SIGCHASE) && DIG_SIGCHASE_TD
"                 +[no]topdown        (Do +sigchase in top-down mode)\n"
#endif
"                 +[no]trace          (Trace delegation down from root [+dnssec])\n"
#ifdef DIG_SIGCHASE
"                 +trusted-key=####   (Trusted Key to use with +sigchase)\n"
#endif
"                 +tries=###          (Set number of UDP attempts) [3]\n"
"                 +[no]ttlid          (Control display of ttls in records)\n"
"                 +[no]ttlunits       (Display TTLs in human-readable units)\n"
"                 +[no]unknownformat  (Print RDATA in RFC 3597 \"unknown\" format)\n"
"                 +[no]vc             (TCP mode (+[no]tcp))\n"
"                 +[no]zflag          (Set Z flag in query)\n"
"        global d-opts and servers (before host name) affect all queries.\n"
"        local d-opts and servers (after host name) affect only that lookup.\n"
"        -h                           (print help and exit)\n"
"        -v                           (print version and exit)\n",
	stdout);
}

/*%
 * Callback from dighost.c to print the received message.
 */
void
received(int bytes, isc_sockaddr_t *from, dig_query_t *query) {
	isc_uint64_t diff;
	time_t tnow;
	struct tm tmnow;
	char time_str[100];
	char fromtext[ISC_SOCKADDR_FORMATSIZE];

	isc_sockaddr_format(from, fromtext, sizeof(fromtext));

	if (query->lookup->stats && !short_form) {
		diff = isc_time_microdiff(&query->time_recv, &query->time_sent);
		if (use_usec)
			printf(";; Query time: %ld usec\n", (long) diff);
		else
			printf(";; Query time: %ld msec\n", (long) diff / 1000);
		printf(";; SERVER: %s(%s)\n", fromtext, query->servname);
		time(&tnow);
		tmnow  = *localtime(&tnow);
		if (strftime(time_str, sizeof(time_str),
			     "%a %b %d %H:%M:%S %Z %Y", &tmnow) > 0U)
			printf(";; WHEN: %s\n", time_str);
		if (query->lookup->doing_xfr) {
			printf(";; XFR size: %u records (messages %u, "
			       "bytes %" ISC_PRINT_QUADFORMAT "u)\n",
			       query->rr_count, query->msg_count,
			       query->byte_count);
		} else {
			printf(";; MSG SIZE  rcvd: %u\n", bytes);
		}
		if (key != NULL) {
			if (!validated)
				puts(";; WARNING -- Some TSIG could not "
				     "be validated");
		}
		if ((key == NULL) && (keysecret[0] != 0)) {
			puts(";; WARNING -- TSIG key was not used.");
		}
		puts("");
	} else if (query->lookup->identify && !short_form) {
		diff = isc_time_microdiff(&query->time_recv, &query->time_sent);
		if (use_usec)
			printf(";; Received %" ISC_PRINT_QUADFORMAT "u bytes "
			       "from %s(%s) in %ld us\n\n",
			       query->lookup->doing_xfr
				 ? query->byte_count
				 : (isc_uint64_t)bytes,
			       fromtext, query->userarg, (long) diff);
		else
			printf(";; Received %" ISC_PRINT_QUADFORMAT "u bytes "
			       "from %s(%s) in %ld ms\n\n",
			       query->lookup->doing_xfr
				 ?  query->byte_count
				 : (isc_uint64_t)bytes,
			       fromtext, query->userarg, (long) diff / 1000);
	}
}

/*
 * Callback from dighost.c to print that it is trying a server.
 * Not used in dig.
 * XXX print_trying
 */
void
trying(char *frm, dig_lookup_t *lookup) {
	UNUSED(frm);
	UNUSED(lookup);
}

/*%
 * Internal print routine used to print short form replies.
 */
static isc_result_t
say_message(dns_rdata_t *rdata, dig_query_t *query, isc_buffer_t *buf) {
	isc_result_t result;
	isc_uint64_t diff;
	char store[sizeof("12345678901234567890")];
	unsigned int styleflags = 0;

	if (query->lookup->trace || query->lookup->ns_search_only) {
		result = dns_rdatatype_totext(rdata->type, buf);
		if (result != ISC_R_SUCCESS)
			return (result);
		ADD_STRING(buf, " ");
	}

	/* Turn on rrcomments if explicitly enabled */
	if (rrcomments > 0)
		styleflags |= DNS_STYLEFLAG_RRCOMMENT;
	if (nocrypto)
		styleflags |= DNS_STYLEFLAG_NOCRYPTO;
	if (query->lookup->print_unknown_format)
		styleflags |= DNS_STYLEFLAG_UNKNOWNFORMAT;
	result = dns_rdata_tofmttext(rdata, NULL, styleflags, 0,
				     splitwidth, " ", buf);
	if (result == ISC_R_NOSPACE)
		return (result);
	check_result(result, "dns_rdata_totext");
	if (query->lookup->identify) {
		diff = isc_time_microdiff(&query->time_recv, &query->time_sent);
		ADD_STRING(buf, " from server ");
		ADD_STRING(buf, query->servname);
		if (use_usec)
			snprintf(store, 19, " in %ld us.", (long) diff);
		else
			snprintf(store, 19, " in %ld ms.", (long) diff / 1000);
		ADD_STRING(buf, store);
	}
	ADD_STRING(buf, "\n");
	return (ISC_R_SUCCESS);
}

/*%
 * short_form message print handler.  Calls above say_message()
 */
static isc_result_t
short_answer(dns_message_t *msg, dns_messagetextflag_t flags,
	     isc_buffer_t *buf, dig_query_t *query)
{
	dns_name_t *name;
	dns_rdataset_t *rdataset;
	isc_result_t result, loopresult;
	dns_name_t empty_name;
	dns_rdata_t rdata = DNS_RDATA_INIT;

	UNUSED(flags);

	dns_name_init(&empty_name, NULL);
	result = dns_message_firstname(msg, DNS_SECTION_ANSWER);
	if (result == ISC_R_NOMORE)
		return (ISC_R_SUCCESS);
	else if (result != ISC_R_SUCCESS)
		return (result);

	for (;;) {
		name = NULL;
		dns_message_currentname(msg, DNS_SECTION_ANSWER, &name);

		for (rdataset = ISC_LIST_HEAD(name->list);
		     rdataset != NULL;
		     rdataset = ISC_LIST_NEXT(rdataset, link)) {
			loopresult = dns_rdataset_first(rdataset);
			while (loopresult == ISC_R_SUCCESS) {
				dns_rdataset_current(rdataset, &rdata);
				result = say_message(&rdata, query,
						     buf);
				if (result == ISC_R_NOSPACE)
					return (result);
				check_result(result, "say_message");
				loopresult = dns_rdataset_next(rdataset);
				dns_rdata_reset(&rdata);
			}
		}
		result = dns_message_nextname(msg, DNS_SECTION_ANSWER);
		if (result == ISC_R_NOMORE)
			break;
		else if (result != ISC_R_SUCCESS)
			return (result);
	}

	return (ISC_R_SUCCESS);
}
#ifdef DIG_SIGCHASE
isc_result_t
printrdataset(dns_name_t *owner_name, dns_rdataset_t *rdataset,
	      isc_buffer_t *target)
{
	isc_result_t result;
	dns_master_style_t *style = NULL;
	unsigned int styleflags = 0;

	if (rdataset == NULL || owner_name == NULL || target == NULL)
		return(ISC_FALSE);

	styleflags |= DNS_STYLEFLAG_REL_OWNER;
	if (ttlunits)
		styleflags |= DNS_STYLEFLAG_TTL_UNITS;
	if (nottl)
		styleflags |= DNS_STYLEFLAG_NO_TTL;
	if (noclass)
		styleflags |= DNS_STYLEFLAG_NO_CLASS;
	if (nocrypto)
		styleflags |= DNS_STYLEFLAG_NOCRYPTO;
	/* Turn on rrcomments if explicitly enabled */
	if (rrcomments > 0)
		styleflags |= DNS_STYLEFLAG_RRCOMMENT;
	if (multiline) {
		styleflags |= DNS_STYLEFLAG_OMIT_OWNER;
		styleflags |= DNS_STYLEFLAG_OMIT_CLASS;
		styleflags |= DNS_STYLEFLAG_REL_DATA;
		styleflags |= DNS_STYLEFLAG_OMIT_TTL;
		styleflags |= DNS_STYLEFLAG_TTL;
		styleflags |= DNS_STYLEFLAG_MULTILINE;
		styleflags |= DNS_STYLEFLAG_COMMENT;
		/* Turn on rrcomments if not explicitly disabled */
		if (rrcomments >= 0)
			styleflags |= DNS_STYLEFLAG_RRCOMMENT;
	}

	if (multiline || (nottl && noclass))
		result = dns_master_stylecreate2(&style, styleflags,
						24, 24, 24, 32, 80, 8,
						splitwidth, mctx);
	else if (nottl || noclass)
		result = dns_master_stylecreate2(&style, styleflags,
						24, 24, 32, 40, 80, 8,
						splitwidth, mctx);
	else
		result = dns_master_stylecreate2(&style, styleflags,
						24, 32, 40, 48, 80, 8,
						splitwidth, mctx);
	check_result(result, "dns_master_stylecreate");

	result = dns_master_rdatasettotext(owner_name, rdataset, style, target);

	if (style != NULL)
		dns_master_styledestroy(&style, mctx);

	return(result);
}
#endif

/*
 * Callback from dighost.c to print the reply from a server
 */
isc_result_t
printmessage(dig_query_t *query, dns_message_t *msg, isc_boolean_t headers) {
	isc_result_t result;
	dns_messagetextflag_t flags;
	isc_buffer_t *buf = NULL;
	unsigned int len = OUTPUTBUF;
	dns_master_style_t *style = NULL;
	unsigned int styleflags = 0;

	styleflags |= DNS_STYLEFLAG_REL_OWNER;
	if (query->lookup->comments)
		styleflags |= DNS_STYLEFLAG_COMMENT;
	if (query->lookup->print_unknown_format)
		styleflags |= DNS_STYLEFLAG_UNKNOWNFORMAT;
	/* Turn on rrcomments if explicitly enabled */
	if (rrcomments > 0)
		styleflags |= DNS_STYLEFLAG_RRCOMMENT;
	if (ttlunits)
		styleflags |= DNS_STYLEFLAG_TTL_UNITS;
	if (nottl)
		styleflags |= DNS_STYLEFLAG_NO_TTL;
	if (noclass)
		styleflags |= DNS_STYLEFLAG_NO_CLASS;
	if (nocrypto)
		styleflags |= DNS_STYLEFLAG_NOCRYPTO;
	if (multiline) {
		styleflags |= DNS_STYLEFLAG_OMIT_OWNER;
		styleflags |= DNS_STYLEFLAG_OMIT_CLASS;
		styleflags |= DNS_STYLEFLAG_REL_DATA;
		styleflags |= DNS_STYLEFLAG_OMIT_TTL;
		styleflags |= DNS_STYLEFLAG_TTL;
		styleflags |= DNS_STYLEFLAG_MULTILINE;
		/* Turn on rrcomments unless explicitly disabled */
		if (rrcomments >= 0)
			styleflags |= DNS_STYLEFLAG_RRCOMMENT;
	}
	if (multiline || (nottl && noclass))
		result = dns_master_stylecreate2(&style, styleflags,
						 24, 24, 24, 32, 80, 8,
						 splitwidth, mctx);
	else if (nottl || noclass)
		result = dns_master_stylecreate2(&style, styleflags,
						 24, 24, 32, 40, 80, 8,
						 splitwidth, mctx);
	else
		result = dns_master_stylecreate2(&style, styleflags,
						 24, 32, 40, 48, 80, 8,
						 splitwidth, mctx);
	check_result(result, "dns_master_stylecreate");

	if (query->lookup->cmdline[0] != 0) {
		if (!short_form)
			fputs(query->lookup->cmdline, stdout);
		query->lookup->cmdline[0]=0;
	}
	debug("printmessage(%s %s %s)", headers ? "headers" : "noheaders",
	      query->lookup->comments ? "comments" : "nocomments",
	      short_form ? "short_form" : "long_form");

	flags = 0;
	if (!headers) {
		flags |= DNS_MESSAGETEXTFLAG_NOHEADERS;
		flags |= DNS_MESSAGETEXTFLAG_NOCOMMENTS;
	}
	if (onesoa && query->lookup->rdtype == dns_rdatatype_axfr)
		flags |= (query->msg_count == 0) ? DNS_MESSAGETEXTFLAG_ONESOA :
						   DNS_MESSAGETEXTFLAG_OMITSOA;
	if (!query->lookup->comments)
		flags |= DNS_MESSAGETEXTFLAG_NOCOMMENTS;

	result = isc_buffer_allocate(mctx, &buf, len);
	check_result(result, "isc_buffer_allocate");

	if (query->lookup->comments && !short_form) {
		if (query->lookup->cmdline[0] != 0)
			printf("; %s\n", query->lookup->cmdline);
		if (msg == query->lookup->sendmsg)
			printf(";; Sending:\n");
		else
			printf(";; Got answer:\n");

		if (headers) {
			printf(";; ->>HEADER<<- opcode: %s, status: %s, "
			       "id: %u\n",
			       opcodetext[msg->opcode],
			       rcode_totext(msg->rcode),
			       msg->id);
			printf(";; flags:");
			if ((msg->flags & DNS_MESSAGEFLAG_QR) != 0)
				printf(" qr");
			if ((msg->flags & DNS_MESSAGEFLAG_AA) != 0)
				printf(" aa");
			if ((msg->flags & DNS_MESSAGEFLAG_TC) != 0)
				printf(" tc");
			if ((msg->flags & DNS_MESSAGEFLAG_RD) != 0)
				printf(" rd");
			if ((msg->flags & DNS_MESSAGEFLAG_RA) != 0)
				printf(" ra");
			if ((msg->flags & DNS_MESSAGEFLAG_AD) != 0)
				printf(" ad");
			if ((msg->flags & DNS_MESSAGEFLAG_CD) != 0)
				printf(" cd");
			if ((msg->flags & 0x0040U) != 0)
				printf("; MBZ: 0x4");

			printf("; QUERY: %u, ANSWER: %u, "
			       "AUTHORITY: %u, ADDITIONAL: %u\n",
			       msg->counts[DNS_SECTION_QUESTION],
			       msg->counts[DNS_SECTION_ANSWER],
			       msg->counts[DNS_SECTION_AUTHORITY],
			       msg->counts[DNS_SECTION_ADDITIONAL]);

			if (msg != query->lookup->sendmsg &&
			    (msg->flags & DNS_MESSAGEFLAG_RD) != 0 &&
			    (msg->flags & DNS_MESSAGEFLAG_RA) == 0)
				printf(";; WARNING: recursion requested "
				       "but not available\n");
		}
		if (msg != query->lookup->sendmsg &&
		    query->lookup->edns != -1 && msg->opt == NULL &&
		    (msg->rcode == dns_rcode_formerr ||
		     msg->rcode == dns_rcode_notimp))
			printf("\n;; WARNING: EDNS query returned status "
			       "%s - retry with '%s+noedns'\n",
			       rcode_totext(msg->rcode),
			       query->lookup->dnssec ? "+nodnssec ": "");
		if (msg != query->lookup->sendmsg && extrabytes != 0U)
			printf(";; WARNING: Message has %u extra byte%s at "
			       "end\n", extrabytes, extrabytes != 0 ? "s" : "");
	}

repopulate_buffer:

	if (query->lookup->comments && headers && !short_form) {
		result = dns_message_pseudosectiontotext(msg,
			 DNS_PSEUDOSECTION_OPT,
			 style, flags, buf);
		if (result == ISC_R_NOSPACE) {
buftoosmall:
			len += OUTPUTBUF;
			isc_buffer_free(&buf);
			result = isc_buffer_allocate(mctx, &buf, len);
			if (result == ISC_R_SUCCESS)
				goto repopulate_buffer;
			else
				goto cleanup;
		}
		check_result(result,
		     "dns_message_pseudosectiontotext");
	}

	if (query->lookup->section_question && headers) {
		if (!short_form) {
			result = dns_message_sectiontotext(msg,
						       DNS_SECTION_QUESTION,
						       style, flags, buf);
			if (result == ISC_R_NOSPACE)
				goto buftoosmall;
			check_result(result, "dns_message_sectiontotext");
		}
	}
	if (query->lookup->section_answer) {
		if (!short_form) {
			result = dns_message_sectiontotext(msg,
						       DNS_SECTION_ANSWER,
						       style, flags, buf);
			if (result == ISC_R_NOSPACE)
				goto buftoosmall;
			check_result(result, "dns_message_sectiontotext");
		} else {
			result = short_answer(msg, flags, buf, query);
			if (result == ISC_R_NOSPACE)
				goto buftoosmall;
			check_result(result, "short_answer");
		}
	}
	if (query->lookup->section_authority) {
		if (!short_form) {
			result = dns_message_sectiontotext(msg,
						       DNS_SECTION_AUTHORITY,
						       style, flags, buf);
			if (result == ISC_R_NOSPACE)
				goto buftoosmall;
			check_result(result, "dns_message_sectiontotext");
		}
	}
	if (query->lookup->section_additional) {
		if (!short_form) {
			result = dns_message_sectiontotext(msg,
						      DNS_SECTION_ADDITIONAL,
						      style, flags, buf);
			if (result == ISC_R_NOSPACE)
				goto buftoosmall;
			check_result(result, "dns_message_sectiontotext");
			/*
			 * Only print the signature on the first record.
			 */
			if (headers) {
				result = dns_message_pseudosectiontotext(
						   msg,
						   DNS_PSEUDOSECTION_TSIG,
						   style, flags, buf);
				if (result == ISC_R_NOSPACE)
					goto buftoosmall;
				check_result(result,
					  "dns_message_pseudosectiontotext");
				result = dns_message_pseudosectiontotext(
						   msg,
						   DNS_PSEUDOSECTION_SIG0,
						   style, flags, buf);
				if (result == ISC_R_NOSPACE)
					goto buftoosmall;
				check_result(result,
					   "dns_message_pseudosectiontotext");
			}
		}
	}

	if (headers && query->lookup->comments && !short_form)
		printf("\n");

	printf("%.*s", (int)isc_buffer_usedlength(buf),
	       (char *)isc_buffer_base(buf));
	isc_buffer_free(&buf);

cleanup:
	if (style != NULL)
		dns_master_styledestroy(&style, mctx);
	return (result);
}

/*%
 * print the greeting message when the program first starts up.
 */
static void
printgreeting(int argc, char **argv, dig_lookup_t *lookup) {
	int i;
	size_t remaining;
	static isc_boolean_t first = ISC_TRUE;
	char append[MXNAME];

	if (printcmd) {
		lookup->cmdline[sizeof(lookup->cmdline) - 1] = 0;
		snprintf(lookup->cmdline, sizeof(lookup->cmdline),
			 "%s; <<>> DiG " VERSION " <<>>",
			 first?"\n":"");
		i = 1;
		while (i < argc) {
			snprintf(append, sizeof(append), " %s", argv[i++]);
			remaining = sizeof(lookup->cmdline) -
				    strlen(lookup->cmdline) - 1;
			strncat(lookup->cmdline, append, remaining);
		}
		remaining = sizeof(lookup->cmdline) -
			    strlen(lookup->cmdline) - 1;
		strncat(lookup->cmdline, "\n", remaining);
		if (first && addresscount != 0) {
			snprintf(append, sizeof(append),
				 "; (%d server%s found)\n",
				 addresscount,
				 addresscount > 1 ? "s" : "");
			remaining = sizeof(lookup->cmdline) -
				    strlen(lookup->cmdline) - 1;
			strncat(lookup->cmdline, append, remaining);
		}
		if (first) {
			snprintf(append, sizeof(append),
				 ";; global options:%s%s\n",
				 short_form ? " +short" : "",
				 printcmd ? " +cmd" : "");
			first = ISC_FALSE;
			remaining = sizeof(lookup->cmdline) -
				    strlen(lookup->cmdline) - 1;
			strncat(lookup->cmdline, append, remaining);
		}
	}
}

/*%
 * We're not using isc_commandline_parse() here since the command line
 * syntax of dig is quite a bit different from that which can be described
 * by that routine.
 * XXX doc options
 */

static void
plus_option(const char *option, isc_boolean_t is_batchfile,
	    dig_lookup_t *lookup)
{
	isc_result_t result;
	char option_store[256];
	char *cmd, *value, *ptr, *code;
	isc_uint32_t num;
	isc_boolean_t state = ISC_TRUE;
	size_t n;

	strncpy(option_store, option, sizeof(option_store));
	option_store[sizeof(option_store)-1]=0;
	ptr = option_store;
	cmd = next_token(&ptr, "=");
	if (cmd == NULL) {
		printf(";; Invalid option %s\n", option_store);
		return;
	}
	value = ptr;
	if (strncasecmp(cmd, "no", 2)==0) {
		cmd += 2;
		state = ISC_FALSE;
	}

#define FULLCHECK(A) \
	do { \
		size_t _l = strlen(cmd); \
		if (_l >= sizeof(A) || strncasecmp(cmd, A, _l) != 0) \
			goto invalid_option; \
	} while (0)
#define FULLCHECK2(A, B) \
	do { \
		size_t _l = strlen(cmd); \
		if ((_l >= sizeof(A) || strncasecmp(cmd, A, _l) != 0) && \
		    (_l >= sizeof(B) || strncasecmp(cmd, B, _l) != 0)) \
			goto invalid_option; \
	} while (0)

	switch (cmd[0]) {
	case 'a':
		switch (cmd[1]) {
		case 'a': /* aaonly / aaflag */
			FULLCHECK2("aaonly", "aaflag");
			lookup->aaonly = state;
			break;
		case 'd':
			switch (cmd[2]) {
			case 'd': /* additional */
				FULLCHECK("additional");
				lookup->section_additional = state;
				break;
			case 'f': /* adflag */
			case '\0': /* +ad is a synonym for +adflag */
				FULLCHECK("adflag");
				lookup->adflag = state;
				break;
			default:
				goto invalid_option;
			}
			break;
		case 'l': /* all */
			FULLCHECK("all");
			lookup->section_question = state;
			lookup->section_authority = state;
			lookup->section_answer = state;
			lookup->section_additional = state;
			lookup->comments = state;
			lookup->stats = state;
			printcmd = state;
			break;
		case 'n': /* answer */
			FULLCHECK("answer");
			lookup->section_answer = state;
			break;
		case 'u': /* authority */
			FULLCHECK("authority");
			lookup->section_authority = state;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'b':
		switch (cmd[1]) {
		case 'a':/* badcookie */
			FULLCHECK("badcookie");
			lookup->badcookie = state;
			break;
		case 'e':/* besteffort */
			FULLCHECK("besteffort");
			lookup->besteffort = state;
			break;
		case 'u':/* bufsize */
			FULLCHECK("bufsize");
			if (value == NULL)
				goto need_value;
			if (!state)
				goto invalid_option;
			result = parse_uint(&num, value, COMMSIZE,
					    "buffer size");
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse buffer size");
			lookup->udpsize = num;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'c':
		switch (cmd[1]) {
		case 'd':/* cdflag */
			switch (cmd[2]) {
			case 'f': /* cdflag */
			case '\0': /* +cd is a synonym for +cdflag */
				FULLCHECK("cdflag");
				lookup->cdflag = state;
				break;
			default:
				goto invalid_option;
			}
			break;
		case 'l': /* class */
			/* keep +cl for backwards compatibility */
			FULLCHECK2("cl", "class");
			noclass = ISC_TF(!state);
			break;
		case 'm': /* cmd */
			FULLCHECK("cmd");
			printcmd = state;
			break;
		case 'o': /* comments */
			switch (cmd[2]) {
			case 'm':
				FULLCHECK("comments");
				lookup->comments = state;
				if (lookup == default_lookup)
					pluscomm = state;
				break;
			case 'o': /* cookie */
				FULLCHECK("cookie");
				if (state && lookup->edns == -1)
					lookup->edns = 0;
				lookup->sendcookie = state;
				if (value != NULL) {
					n = strlcpy(hexcookie, value,
						    sizeof(hexcookie));
					if (n >= sizeof(hexcookie))
						fatal("COOKIE data too large");
					lookup->cookie = hexcookie;
				} else
					lookup->cookie = NULL;
				break;
			default:
				goto invalid_option;
			}
			break;
		case 'r':
			FULLCHECK("crypto");
			nocrypto = ISC_TF(!state);
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'd':
		switch (cmd[1]) {
		case 'e': /* defname */
			FULLCHECK("defname");
			if (!lookup->trace) {
				usesearch = state;
			}
			break;
		case 'n': /* dnssec */
			FULLCHECK("dnssec");
			if (state && lookup->edns == -1)
				lookup->edns = 0;
			lookup->dnssec = state;
			break;
		case 'o': /* domain */
			FULLCHECK("domain");
			if (value == NULL)
				goto need_value;
			if (!state)
				goto invalid_option;
			strncpy(domainopt, value, sizeof(domainopt));
			domainopt[sizeof(domainopt)-1] = '\0';
			break;
		case 's': /* dscp */
			FULLCHECK("dscp");
			if (!state) {
				lookup->dscp = -1;
				break;
			}
			if (value == NULL)
				goto need_value;
			result = parse_uint(&num, value, 0x3f, "DSCP");
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse DSCP value");
			lookup->dscp = num;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'e':
		switch (cmd[1]) {
		case 'd':
			switch(cmd[2]) {
			case 'n':
				switch (cmd[3]) {
				case 's':
					switch (cmd[4]) {
					case 0:
						FULLCHECK("edns");
						if (!state) {
							lookup->edns = -1;
							break;
						}
						if (value == NULL) {
							lookup->edns = 0;
							break;
						}
						result = parse_uint(&num,
								    value,
								    255,
								    "edns");
						if (result != ISC_R_SUCCESS)
							fatal("Couldn't parse "
							      "edns");
						lookup->edns = num;
						break;
					case 'f':
						FULLCHECK("ednsflags");
						if (!state) {
							lookup->ednsflags = 0;
							break;
						}
						if (value == NULL) {
							lookup->ednsflags = 0;
							break;
						}
						result = parse_xint(&num,
								    value,
								    0xffff,
								  "ednsflags");
						if (result != ISC_R_SUCCESS)
							fatal("Couldn't parse "
							      "ednsflags");
						lookup->ednsflags = num;
						break;
					case 'n':
						FULLCHECK("ednsnegotiation");
						lookup->ednsneg = state;
						break;
					case 'o':
						FULLCHECK("ednsopt");
						if (!state) {
							lookup->ednsoptscnt = 0;
							break;
						}
						if (value == NULL)
							fatal("ednsopt no "
							      "code point "
							      "specified");
						code = next_token(&value, ":");
						save_opt(lookup, code, value);
						break;
					default:
						goto invalid_option;
					}
					break;
				default:
					goto invalid_option;
				}
				break;
			default:
				goto invalid_option;
			}
			break;
		case 'x':
			FULLCHECK("expire");
			lookup->expire = state;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'f': /* fail */
		FULLCHECK("fail");
		lookup->servfail_stops = state;
		break;
	case 'h':
		FULLCHECK("header-only");
		lookup->header_only = state;
		break;
	case 'i':
		switch (cmd[1]) {
		case 'd': /* identify */
			FULLCHECK("identify");
			lookup->identify = state;
			break;
		case 'g': /* ignore */
		default: /*
			  * Inherits default for compatibility (+[no]i*).
			  */
			FULLCHECK("ignore");
			lookup->ignore = state;
		}
		break;
	case 'k':
		FULLCHECK("keepopen");
		keep_open = state;
		break;
	case 'm': /* multiline */
		switch (cmd[1]) {
		case 'a':
			FULLCHECK("mapped");
			lookup->mapped = state;
			break;
		case 'u':
			FULLCHECK("multiline");
			multiline = state;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'n':
		switch (cmd[1]) {
		case 'd': /* ndots */
			FULLCHECK("ndots");
			if (value == NULL)
				goto need_value;
			if (!state)
				goto invalid_option;
			result = parse_uint(&num, value, MAXNDOTS, "ndots");
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse ndots");
			ndots = num;
			break;
		case 's':
			switch (cmd[2]) {
			case 'i': /* nsid */
				FULLCHECK("nsid");
				if (state && lookup->edns == -1)
					lookup->edns = 0;
				lookup->nsid = state;
				break;
			case 's': /* nssearch */
				FULLCHECK("nssearch");
				lookup->ns_search_only = state;
				if (state) {
					lookup->trace_root = ISC_TRUE;
					lookup->recurse = ISC_TRUE;
					lookup->identify = ISC_TRUE;
					lookup->stats = ISC_FALSE;
					lookup->comments = ISC_FALSE;
					lookup->section_additional = ISC_FALSE;
					lookup->section_authority = ISC_FALSE;
					lookup->section_question = ISC_FALSE;
					lookup->rdtype = dns_rdatatype_ns;
					lookup->rdtypeset = ISC_TRUE;
					short_form = ISC_TRUE;
					rrcomments = 0;
				}
				break;
			default:
				goto invalid_option;
			}
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'o':
		switch (cmd[1]) {
		case 'n':
			FULLCHECK("onesoa");
			onesoa = state;
			break;
		case 'p':
			FULLCHECK("opcode");
			if (!state) {
				lookup->opcode = 0;	/* default - query */
				break;
			}
			if (value == NULL)
				goto need_value;
			for (num = 0;
			     num < sizeof(opcodetext)/sizeof(opcodetext[0]);
			     num++) {
				if (strcasecmp(opcodetext[num], value) == 0)
					break;
			}
			if (num < 16) {
				lookup->opcode = (dns_opcode_t)num;
				break;
			}
			result = parse_uint(&num, value, 15, "opcode");
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse opcode");
			lookup->opcode = (dns_opcode_t)num;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'q':
		switch (cmd[1]) {
		case 'r': /* qr */
			FULLCHECK("qr");
			qr = state;
			break;
		case 'u': /* question */
			FULLCHECK("question");
			lookup->section_question = state;
			if (lookup == default_lookup)
				plusquest = state;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'r':
		switch (cmd[1]) {
		case 'd': /* rdflag */
			FULLCHECK("rdflag");
			lookup->recurse = state;
			break;
		case 'e':
			switch (cmd[2]) {
			case 'c': /* recurse */
				FULLCHECK("recurse");
				lookup->recurse = state;
				break;
			case 't': /* retry / retries */
				FULLCHECK2("retry", "retries");
				if (value == NULL)
					goto need_value;
				if (!state)
					goto invalid_option;
				result = parse_uint(&lookup->retries, value,
						    MAXTRIES - 1, "retries");
				if (result != ISC_R_SUCCESS)
					fatal("Couldn't parse retries");
				lookup->retries++;
				break;
			default:
				goto invalid_option;
			}
			break;
		case 'r': /* rrcomments */
			FULLCHECK("rrcomments");
			rrcomments = state ? 1 : -1;
			break;
		default:
			goto invalid_option;
		}
		break;
	case 's':
		switch (cmd[1]) {
		case 'e': /* search */
			FULLCHECK("search");
			if (!lookup->trace) {
				usesearch = state;
			}
			break;
		case 'h':
			if (cmd[2] != 'o')
				goto invalid_option;
			switch (cmd[3]) {
			case 'r': /* short */
				FULLCHECK("short");
				short_form = state;
				if (state) {
					printcmd = ISC_FALSE;
					lookup->section_additional = ISC_FALSE;
					lookup->section_answer = ISC_TRUE;
					lookup->section_authority = ISC_FALSE;
					lookup->section_question = ISC_FALSE;
					lookup->comments = ISC_FALSE;
					lookup->stats = ISC_FALSE;
					rrcomments = -1;
				}
				break;
			case 'w': /* showsearch */
				FULLCHECK("showsearch");
				if (!lookup->trace) {
					showsearch = state;
					usesearch = state;
				}
				break;
			default:
				goto invalid_option;
			}
			break;
#ifdef DIG_SIGCHASE
		case 'i': /* sigchase */
			FULLCHECK("sigchase");
			lookup->sigchase = state;
			if (lookup->sigchase)
				lookup->dnssec = ISC_TRUE;
			break;
#endif
		case 'p': /* split */
			FULLCHECK("split");
			if (value != NULL && !state)
				goto invalid_option;
			if (!state) {
				splitwidth = 0;
				break;
			} else if (value == NULL)
				break;

			result = parse_uint(&splitwidth, value,
					    1023, "split");
			if (splitwidth % 4 != 0) {
				splitwidth = ((splitwidth + 3) / 4) * 4;
				fprintf(stderr, ";; Warning, split must be "
						"a multiple of 4; adjusting "
						"to %d\n", splitwidth);
			}
			/*
			 * There is an adjustment done in the
			 * totext_<rrtype>() functions which causes
			 * splitwidth to shrink.  This is okay when we're
			 * using the default width but incorrect in this
			 * case, so we correct for it
			 */
			if (splitwidth)
				splitwidth += 3;
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse split");
			break;
		case 't': /* stats */
			FULLCHECK("stats");
			lookup->stats = state;
			break;
		case 'u': /* subnet */
			FULLCHECK("subnet");
			if (state && value == NULL)
				goto need_value;
			if (!state) {
				if (lookup->ecs_addr != NULL) {
					isc_mem_free(mctx, lookup->ecs_addr);
					lookup->ecs_addr = NULL;
				}
				break;
			}
			if (lookup->edns == -1)
				lookup->edns = 0;

			result = parse_netprefix(&lookup->ecs_addr, value);
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse client");
			break;
		default:
			goto invalid_option;
		}
		break;
	case 't':
		switch (cmd[1]) {
		case 'c': /* tcp */
			FULLCHECK("tcp");
			if (!is_batchfile) {
				lookup->tcp_mode = state;
				lookup->tcp_mode_set = ISC_TRUE;
			}
			break;
		case 'i': /* timeout */
			FULLCHECK("timeout");
			if (value == NULL)
				goto need_value;
			if (!state)
				goto invalid_option;
			result = parse_uint(&timeout, value, MAXTIMEOUT,
					    "timeout");
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse timeout");
			if (timeout == 0)
				timeout = 1;
			break;
#if DIG_SIGCHASE_TD
		case 'o': /* topdown */
			FULLCHECK("topdown");
			lookup->do_topdown = state;
			break;
#endif
		case 'r':
			switch (cmd[2]) {
			case 'a': /* trace */
				FULLCHECK("trace");
				lookup->trace = state;
				lookup->trace_root = state;
				if (state) {
					lookup->recurse = ISC_FALSE;
					lookup->identify = ISC_TRUE;
					lookup->comments = ISC_FALSE;
					rrcomments = 0;
					lookup->stats = ISC_FALSE;
					lookup->section_additional = ISC_FALSE;
					lookup->section_authority = ISC_TRUE;
					lookup->section_question = ISC_FALSE;
					lookup->dnssec = ISC_TRUE;
					lookup->sendcookie = ISC_TRUE;
					usesearch = ISC_FALSE;
				}
				break;
			case 'i': /* tries */
				FULLCHECK("tries");
				if (value == NULL)
					goto need_value;
				if (!state)
					goto invalid_option;
				result = parse_uint(&lookup->retries, value,
						    MAXTRIES, "tries");
				if (result != ISC_R_SUCCESS)
					fatal("Couldn't parse tries");
				if (lookup->retries == 0)
					lookup->retries = 1;
				break;
#ifdef DIG_SIGCHASE
			case 'u': /* trusted-key */
				FULLCHECK("trusted-key");
				if (value == NULL)
					goto need_value;
				if (!state)
					goto invalid_option;
				n = strlcpy(trustedkey, ptr,
					    sizeof(trustedkey));
				if (n >= sizeof(trustedkey))
					fatal("trusted key too large");
				break;
#endif
			default:
				goto invalid_option;
			}
			break;
		case 't':
			switch (cmd[2]) {
			case 'l':
				switch (cmd[3]) {
				case 0:
				case 'i': /* ttlid */
					FULLCHECK2("ttl", "ttlid");
					nottl = ISC_TF(!state);
					break;
				case 'u': /* ttlunits */
					FULLCHECK("ttlunits");
					nottl = ISC_FALSE;
					ttlunits = ISC_TF(state);
					break;
				default:
					goto invalid_option;
				}
				break;
			default:
				goto invalid_option;
			}
			break;
		default:
			goto invalid_option;
		}
		break;
	case 'u':
		FULLCHECK("unknownformat");
		lookup->print_unknown_format = state;
		break;
	case 'v':
		FULLCHECK("vc");
		if (!is_batchfile) {
			lookup->tcp_mode = state;
			lookup->tcp_mode_set = ISC_TRUE;
		}
		break;
	case 'z': /* zflag */
		FULLCHECK("zflag");
		lookup->zflag = state;
		break;
	default:
	invalid_option:
	need_value:
		fprintf(stderr, "Invalid option: +%s\n",
			option);
		usage();
	}
	return;
}

/*%
 * #ISC_TRUE returned if value was used
 */
static const char *single_dash_opts = "46dhimnuv";
static const char *dash_opts = "46bcdfhikmnptvyx";
static isc_boolean_t
dash_option(char *option, char *next, dig_lookup_t **lookup,
	    isc_boolean_t *open_type_class, isc_boolean_t *need_clone,
	    isc_boolean_t config_only, int argc, char **argv,
	    isc_boolean_t *firstarg)
{
	char opt, *value, *ptr, *ptr2, *ptr3;
	isc_result_t result;
	isc_boolean_t value_from_next;
	isc_textregion_t tr;
	dns_rdatatype_t rdtype;
	dns_rdataclass_t rdclass;
	char textname[MXNAME];
	struct in_addr in4;
	struct in6_addr in6;
	in_port_t srcport;
	char *hash, *cmd;
	isc_uint32_t num;

	while (strpbrk(option, single_dash_opts) == &option[0]) {
		/*
		 * Since the -[46dhimnuv] options do not take an argument,
		 * account for them (in any number and/or combination)
		 * if they appear as the first character(s) of a q-opt.
		 */
		opt = option[0];
		switch (opt) {
		case '4':
			if (have_ipv4) {
				isc_net_disableipv6();
				have_ipv6 = ISC_FALSE;
			} else {
				fatal("can't find IPv4 networking");
				/* NOTREACHED */
				return (ISC_FALSE);
			}
			break;
		case '6':
			if (have_ipv6) {
				isc_net_disableipv4();
				have_ipv4 = ISC_FALSE;
			} else {
				fatal("can't find IPv6 networking");
				/* NOTREACHED */
				return (ISC_FALSE);
			}
			break;
		case 'd':
			ptr = strpbrk(&option[1], dash_opts);
			if (ptr != &option[1]) {
				cmd = option;
				FULLCHECK("debug");
				debugging = ISC_TRUE;
				return (ISC_FALSE);
			} else
				debugging = ISC_TRUE;
			break;
		case 'h':
			help();
			exit(0);
			break;
		case 'i':
			ip6_int = ISC_TRUE;
			break;
		case 'm': /* memdebug */
			/* memdebug is handled in preparse_args() */
			break;
		case 'n':
			/* deprecated */
			break;
		case 'u':
			use_usec = ISC_TRUE;
			break;
		case 'v':
			version();
			exit(0);
			break;
		}
		if (strlen(option) > 1U)
			option = &option[1];
		else
			return (ISC_FALSE);
	}
	opt = option[0];
	if (strlen(option) > 1U) {
		value_from_next = ISC_FALSE;
		value = &option[1];
	} else {
		value_from_next = ISC_TRUE;
		value = next;
	}
	if (value == NULL)
		goto invalid_option;
	switch (opt) {
	case 'b':
		hash = strchr(value, '#');
		if (hash != NULL) {
			result = parse_uint(&num, hash + 1, MAXPORT,
					    "port number");
			if (result != ISC_R_SUCCESS)
				fatal("Couldn't parse port number");
			srcport = num;
			*hash = '\0';
		} else
			srcport = 0;
		if (have_ipv6 && inet_pton(AF_INET6, value, &in6) == 1) {
			isc_sockaddr_fromin6(&bind_address, &in6, srcport);
			isc_net_disableipv4();
		} else if (have_ipv4 && inet_pton(AF_INET, value, &in4) == 1) {
			isc_sockaddr_fromin(&bind_address, &in4, srcport);
			isc_net_disableipv6();
		} else {
			if (hash != NULL)
				*hash = '#';
			fatal("invalid address %s", value);
		}
		if (hash != NULL)
			*hash = '#';
		specified_source = ISC_TRUE;
		return (value_from_next);
	case 'c':
		if ((*lookup)->rdclassset) {
			fprintf(stderr, ";; Warning, extra class option\n");
		}
		*open_type_class = ISC_FALSE;
		tr.base = value;
		tr.length = (unsigned int) strlen(value);
		result = dns_rdataclass_fromtext(&rdclass,
						 (isc_textregion_t *)&tr);
		if (result == ISC_R_SUCCESS) {
			(*lookup)->rdclass = rdclass;
			(*lookup)->rdclassset = ISC_TRUE;
		} else
			fprintf(stderr, ";; Warning, ignoring "
				"invalid class %s\n",
				value);
		return (value_from_next);
	case 'f':
		batchname = value;
		return (value_from_next);
	case 'k':
		strncpy(keyfile, value, sizeof(keyfile));
		keyfile[sizeof(keyfile)-1]=0;
		return (value_from_next);
	case 'p':
		result = parse_uint(&num, value, MAXPORT, "port number");
		if (result != ISC_R_SUCCESS)
			fatal("Couldn't parse port number");
		port = num;
		return (value_from_next);
	case 'q':
		if (!config_only) {
			if (*need_clone)
				(*lookup) = clone_lookup(default_lookup,
							 ISC_TRUE);
			*need_clone = ISC_TRUE;
			strncpy((*lookup)->textname, value,
				sizeof((*lookup)->textname));
			(*lookup)->textname[sizeof((*lookup)->textname)-1]=0;
			(*lookup)->trace_root = ISC_TF((*lookup)->trace  ||
						     (*lookup)->ns_search_only);
			(*lookup)->new_search = ISC_TRUE;
			if (*firstarg) {
				printgreeting(argc, argv, *lookup);
				*firstarg = ISC_FALSE;
			}
			ISC_LIST_APPEND(lookup_list, (*lookup), link);
			debug("looking up %s", (*lookup)->textname);
		}
		return (value_from_next);
	case 't':
		*open_type_class = ISC_FALSE;
		if (strncasecmp(value, "ixfr=", 5) == 0) {
			rdtype = dns_rdatatype_ixfr;
			result = ISC_R_SUCCESS;
		} else {
			tr.base = value;
			tr.length = (unsigned int) strlen(value);
			result = dns_rdatatype_fromtext(&rdtype,
						(isc_textregion_t *)&tr);
			if (result == ISC_R_SUCCESS &&
			    rdtype == dns_rdatatype_ixfr) {
				result = DNS_R_UNKNOWN;
			}
		}
		if (result == ISC_R_SUCCESS) {
			if ((*lookup)->rdtypeset) {
				fprintf(stderr, ";; Warning, "
						"extra type option\n");
			}
			if (rdtype == dns_rdatatype_ixfr) {
				isc_uint32_t serial;
				(*lookup)->rdtype = dns_rdatatype_ixfr;
				(*lookup)->rdtypeset = ISC_TRUE;
				result = parse_uint(&serial, &value[5],
					   MAXSERIAL, "serial number");
				if (result != ISC_R_SUCCESS)
					fatal("Couldn't parse serial number");
				(*lookup)->ixfr_serial = serial;
				(*lookup)->section_question = plusquest;
				(*lookup)->comments = pluscomm;
				if (!(*lookup)->tcp_mode_set)
					(*lookup)->tcp_mode = ISC_TRUE;
			} else {
				(*lookup)->rdtype = rdtype;
				if (!config_only)
					(*lookup)->rdtypeset = ISC_TRUE;
				if (rdtype == dns_rdatatype_axfr) {
					(*lookup)->section_question = plusquest;
					(*lookup)->comments = pluscomm;
				}
				(*lookup)->ixfr_serial = ISC_FALSE;
			}
		} else
			fprintf(stderr, ";; Warning, ignoring "
				 "invalid type %s\n",
				 value);
		return (value_from_next);
	case 'y':
		ptr = next_token(&value, ":");	/* hmac type or name */
		if (ptr == NULL) {
			usage();
		}
		ptr2 = next_token(&value, ":");	/* name or secret */
		if (ptr2 == NULL)
			usage();
		ptr3 = next_token(&value, ":"); /* secret or NULL */
		if (ptr3 != NULL) {
			parse_hmac(ptr);
			ptr = ptr2;
			ptr2 = ptr3;
		} else  {
#ifndef PK11_MD5_DISABLE
			hmacname = DNS_TSIG_HMACMD5_NAME;
#else
			hmacname = DNS_TSIG_HMACSHA256_NAME;
#endif
			digestbits = 0;
		}
		strncpy(keynametext, ptr, sizeof(keynametext));
		keynametext[sizeof(keynametext)-1]=0;
		strncpy(keysecret, ptr2, sizeof(keysecret));
		keysecret[sizeof(keysecret)-1]=0;
		return (value_from_next);
	case 'x':
		if (*need_clone)
			*lookup = clone_lookup(default_lookup, ISC_TRUE);
		*need_clone = ISC_TRUE;
		if (get_reverse(textname, sizeof(textname), value,
				ip6_int, ISC_FALSE) == ISC_R_SUCCESS) {
			strncpy((*lookup)->textname, textname,
				sizeof((*lookup)->textname));
			(*lookup)->textname[sizeof((*lookup)->textname)-1] = 0;
			debug("looking up %s", (*lookup)->textname);
			(*lookup)->trace_root = ISC_TF((*lookup)->trace  ||
						(*lookup)->ns_search_only);
			(*lookup)->ip6_int = ip6_int;
			if (!(*lookup)->rdtypeset)
				(*lookup)->rdtype = dns_rdatatype_ptr;
			if (!(*lookup)->rdclassset)
				(*lookup)->rdclass = dns_rdataclass_in;
			(*lookup)->new_search = ISC_TRUE;
			if (*firstarg) {
				printgreeting(argc, argv, *lookup);
				*firstarg = ISC_FALSE;
			}
			ISC_LIST_APPEND(lookup_list, *lookup, link);
		} else {
			fprintf(stderr, "Invalid IP address %s\n", value);
			exit(1);
		}
		return (value_from_next);
	invalid_option:
	default:
		fprintf(stderr, "Invalid option: -%s\n", option);
		usage();
	}
	/* NOTREACHED */
	return (ISC_FALSE);
}

/*%
 * Because we may be trying to do memory allocation recording, we're going
 * to need to parse the arguments for the -m *before* we start the main
 * argument parsing routine.
 *
 * I'd prefer not to have to do this, but I am not quite sure how else to
 * fix the problem.  Argument parsing in dig involves memory allocation
 * by its nature, so it can't be done in the main argument parser.
 */
static void
preparse_args(int argc, char **argv) {
	int rc;
	char **rv;
	char *option;

	rc = argc;
	rv = argv;
	for (rc--, rv++; rc > 0; rc--, rv++) {
		if (rv[0][0] != '-')
			continue;
		option = &rv[0][1];
		while (strpbrk(option, single_dash_opts) == &option[0]) {
			switch (option[0]) {
			case 'm':
				memdebugging = ISC_TRUE;
				isc_mem_debugging = ISC_MEM_DEBUGTRACE |
					ISC_MEM_DEBUGRECORD;
				break;
			case '4':
				if (ipv6only)
					fatal("only one of -4 and -6 allowed");
				ipv4only = ISC_TRUE;
				break;
			case '6':
				if (ipv4only)
					fatal("only one of -4 and -6 allowed");
				ipv6only = ISC_TRUE;
				break;
			}
			option = &option[1];
		}
	}
}

static void
parse_args(isc_boolean_t is_batchfile, isc_boolean_t config_only,
	   int argc, char **argv)
{
	isc_result_t result;
	isc_textregion_t tr;
	isc_boolean_t firstarg = ISC_TRUE;
	dig_lookup_t *lookup = NULL;
	dns_rdatatype_t rdtype;
	dns_rdataclass_t rdclass;
	isc_boolean_t open_type_class = ISC_TRUE;
	char batchline[MXNAME];
	int bargc;
	char *bargv[64];
	int rc;
	char **rv;
#ifndef NOPOSIX
	char *homedir;
	char rcfile[256];
#endif
	char *input;
	int i;
	isc_boolean_t need_clone = ISC_TRUE;

	/*
	 * The semantics for parsing the args is a bit complex; if
	 * we don't have a host yet, make the arg apply globally,
	 * otherwise make it apply to the latest host.  This is
	 * a bit different than the previous versions, but should
	 * form a consistent user interface.
	 *
	 * First, create a "default lookup" which won't actually be used
	 * anywhere, except for cloning into new lookups
	 */

	debug("parse_args()");
	if (!is_batchfile) {
		debug("making new lookup");
		default_lookup = make_empty_lookup();
		default_lookup->adflag = ISC_TRUE;
		default_lookup->edns = 0;
		default_lookup->sendcookie = ISC_TRUE;

#ifndef NOPOSIX
		/*
		 * Treat ${HOME}/.digrc as a special batchfile
		 */
		INSIST(batchfp == NULL);
		homedir = getenv("HOME");
		if (homedir != NULL) {
			unsigned int n;
			n = snprintf(rcfile, sizeof(rcfile), "%s/.digrc",
				     homedir);
			if (n < sizeof(rcfile))
				batchfp = fopen(rcfile, "r");
		}
		if (batchfp != NULL) {
			while (fgets(batchline, sizeof(batchline),
				     batchfp) != 0) {
				debug("config line %s", batchline);
				bargc = 1;
				input = batchline;
				bargv[bargc] = next_token(&input, " \t\r\n");
				while ((bargv[bargc] != NULL) &&
				       (bargc < 62)) {
					bargc++;
					bargv[bargc] =
						next_token(&input, " \t\r\n");
				}

				bargv[0] = argv[0];
				argv0 = argv[0];

				for(i = 0; i < bargc; i++)
					debug(".digrc argv %d: %s",
					      i, bargv[i]);
				parse_args(ISC_TRUE, ISC_TRUE, bargc,
					   (char **)bargv);
			}
			fclose(batchfp);
		}
#endif
	}

	if (is_batchfile && !config_only) {
		/* Processing '-f batchfile'. */
		lookup = clone_lookup(default_lookup, ISC_TRUE);
		need_clone = ISC_FALSE;
	} else
		lookup = default_lookup;

	rc = argc;
	rv = argv;
	for (rc--, rv++; rc > 0; rc--, rv++) {
		debug("main parsing %s", rv[0]);
		if (strncmp(rv[0], "%", 1) == 0)
			break;
		if (rv[0][0] == '@') {

			if (is_batchfile && !config_only) {
				addresscount = getaddresses(lookup, &rv[0][1],
							     &result);
				if (addresscount == 0) {
					fprintf(stderr, "couldn't get address "
						"for '%s': %s: skipping "
						"lookup\n", &rv[0][1],
						isc_result_totext(result));
					if (ISC_LINK_LINKED(lookup, link))
						ISC_LIST_DEQUEUE(lookup_list,
								 lookup, link);
					destroy_lookup(lookup);
					return;
				}
			} else {
				addresscount = getaddresses(lookup, &rv[0][1],
							    NULL);
				if (addresscount == 0)
					fatal("no valid addresses for '%s'\n",
					      &rv[0][1]);
			}
		} else if (rv[0][0] == '+') {
			plus_option(&rv[0][1], is_batchfile,
				    lookup);
		} else if (rv[0][0] == '-') {
			if (rc <= 1) {
				if (dash_option(&rv[0][1], NULL,
						&lookup, &open_type_class,
						&need_clone, config_only,
						argc, argv, &firstarg)) {
					rc--;
					rv++;
				}
			} else {
				if (dash_option(&rv[0][1], rv[1],
						&lookup, &open_type_class,
						&need_clone, config_only,
						argc, argv, &firstarg)) {
					rc--;
					rv++;
				}
			}
		} else {
			/*
			 * Anything which isn't an option
			 */
			if (open_type_class) {
				if (strncasecmp(rv[0], "ixfr=", 5) == 0) {
					rdtype = dns_rdatatype_ixfr;
					result = ISC_R_SUCCESS;
				} else {
					tr.base = rv[0];
					tr.length =
						(unsigned int) strlen(rv[0]);
					result = dns_rdatatype_fromtext(&rdtype,
						(isc_textregion_t *)&tr);
					if (result == ISC_R_SUCCESS &&
					    rdtype == dns_rdatatype_ixfr) {
						fprintf(stderr, ";; Warning, "
							"ixfr requires a "
							"serial number\n");
						continue;
					}
				}
				if (result == ISC_R_SUCCESS) {
					if (lookup->rdtypeset) {
						fprintf(stderr, ";; Warning, "
							"extra type option\n");
					}
					if (rdtype == dns_rdatatype_ixfr) {
						isc_uint32_t serial;
						lookup->rdtype =
							dns_rdatatype_ixfr;
						lookup->rdtypeset = ISC_TRUE;
						result = parse_uint(&serial,
								    &rv[0][5],
								    MAXSERIAL,
							      "serial number");
						if (result != ISC_R_SUCCESS)
							fatal("Couldn't parse "
							      "serial number");
						lookup->ixfr_serial = serial;
						lookup->section_question =
							plusquest;
						lookup->comments = pluscomm;
						if (!lookup->tcp_mode_set)
							lookup->tcp_mode = ISC_TRUE;
					} else {
						lookup->rdtype = rdtype;
						lookup->rdtypeset = ISC_TRUE;
						if (rdtype ==
						    dns_rdatatype_axfr) {
						    lookup->section_question =
								plusquest;
						    lookup->comments = pluscomm;
						}
						lookup->ixfr_serial = ISC_FALSE;
					}
					continue;
				}
				result = dns_rdataclass_fromtext(&rdclass,
						     (isc_textregion_t *)&tr);
				if (result == ISC_R_SUCCESS) {
					if (lookup->rdclassset) {
						fprintf(stderr, ";; Warning, "
							"extra class option\n");
					}
					lookup->rdclass = rdclass;
					lookup->rdclassset = ISC_TRUE;
					continue;
				}
			}

			if (!config_only) {
				if (need_clone)
					lookup = clone_lookup(default_lookup,
								      ISC_TRUE);
				need_clone = ISC_TRUE;
				strncpy(lookup->textname, rv[0],
					sizeof(lookup->textname));
				lookup->textname[sizeof(lookup->textname)-1]=0;
				lookup->trace_root = ISC_TF(lookup->trace  ||
						     lookup->ns_search_only);
				lookup->new_search = ISC_TRUE;
				if (firstarg) {
					printgreeting(argc, argv, lookup);
					firstarg = ISC_FALSE;
				}
				ISC_LIST_APPEND(lookup_list, lookup, link);
				debug("looking up %s", lookup->textname);
			}
			/* XXX Error message */
		}
	}

	/*
	 * If we have a batchfile, seed the lookup list with the
	 * first entry, then trust the callback in dighost_shutdown
	 * to get the rest
	 */
	if ((batchname != NULL) && !(is_batchfile)) {
		if (strcmp(batchname, "-") == 0)
			batchfp = stdin;
		else
			batchfp = fopen(batchname, "r");
		if (batchfp == NULL) {
			perror(batchname);
			if (exitcode < 8)
				exitcode = 8;
			fatal("couldn't open specified batch file");
		}
		/* XXX Remove code dup from shutdown code */
	next_line:
		if (fgets(batchline, sizeof(batchline), batchfp) != 0) {
			bargc = 1;
			debug("batch line %s", batchline);
			if (batchline[0] == '\r' || batchline[0] == '\n'
			    || batchline[0] == '#' || batchline[0] == ';')
				goto next_line;
			input = batchline;
			bargv[bargc] = next_token(&input, " \t\r\n");
			while ((bargv[bargc] != NULL) && (bargc < 14)) {
				bargc++;
				bargv[bargc] = next_token(&input, " \t\r\n");
			}

			bargv[0] = argv[0];
			argv0 = argv[0];

			for(i = 0; i < bargc; i++)
				debug("batch argv %d: %s", i, bargv[i]);
			parse_args(ISC_TRUE, ISC_FALSE, bargc, (char **)bargv);
			return;
		}
		return;
	}
	/*
	 * If no lookup specified, search for root
	 */
	if ((lookup_list.head == NULL) && !config_only) {
		if (need_clone)
			lookup = clone_lookup(default_lookup, ISC_TRUE);
		need_clone = ISC_TRUE;
		lookup->trace_root = ISC_TF(lookup->trace ||
					    lookup->ns_search_only);
		lookup->new_search = ISC_TRUE;
		strcpy(lookup->textname, ".");
		lookup->rdtype = dns_rdatatype_ns;
		lookup->rdtypeset = ISC_TRUE;
		if (firstarg) {
			printgreeting(argc, argv, lookup);
			firstarg = ISC_FALSE;
		}
		ISC_LIST_APPEND(lookup_list, lookup, link);
	}
	if (!need_clone)
		destroy_lookup(lookup);
}

/*
 * Callback from dighost.c to allow program-specific shutdown code.
 * Here, we're possibly reading from a batch file, then shutting down
 * for real if there's nothing in the batch file to read.
 */
void
dighost_shutdown(void) {
	char batchline[MXNAME];
	int bargc;
	char *bargv[16];
	char *input;
	int i;

	if (batchname == NULL) {
		isc_app_shutdown();
		return;
	}

	fflush(stdout);
	if (feof(batchfp)) {
		batchname = NULL;
		isc_app_shutdown();
		if (batchfp != stdin)
			fclose(batchfp);
		return;
	}

	if (fgets(batchline, sizeof(batchline), batchfp) != 0) {
		debug("batch line %s", batchline);
		bargc = 1;
		input = batchline;
		bargv[bargc] = next_token(&input, " \t\r\n");
		while ((bargv[bargc] != NULL) && (bargc < 14)) {
			bargc++;
			bargv[bargc] = next_token(&input, " \t\r\n");
		}

		bargv[0] = argv0;

		for(i = 0; i < bargc; i++)
			debug("batch argv %d: %s", i, bargv[i]);
		parse_args(ISC_TRUE, ISC_FALSE, bargc, (char **)bargv);
		start_lookup();
	} else {
		batchname = NULL;
		if (batchfp != stdin)
			fclose(batchfp);
		isc_app_shutdown();
		return;
	}
}

/*% Main processing routine for dig */
int
main(int argc, char **argv) {
	isc_result_t result;

	ISC_LIST_INIT(lookup_list);
	ISC_LIST_INIT(server_list);
	ISC_LIST_INIT(search_list);

	debug("main()");
	progname = argv[0];
	preparse_args(argc, argv);
	result = isc_app_start();
	check_result(result, "isc_app_start");
	setup_libs();
	setup_system(ipv4only, ipv6only);
	parse_args(ISC_FALSE, ISC_FALSE, argc, argv);
	if (keyfile[0] != 0)
		setup_file_key();
	else if (keysecret[0] != 0)
		setup_text_key();
	if (domainopt[0] != '\0') {
		set_search_domain(domainopt);
		usesearch = ISC_TRUE;
	}
	result = isc_app_onrun(mctx, global_task, onrun_callback, NULL);
	check_result(result, "isc_app_onrun");
	isc_app_run();
	destroy_lookup(default_lookup);
	if (batchname != NULL) {
		if (batchfp != stdin)
			fclose(batchfp);
		batchname = NULL;
	}
#ifdef DIG_SIGCHASE
	clean_trustedkey();
#endif
	cancel_all();
	destroy_libs();
	isc_app_finish();
	return (exitcode);
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 2055,2;2055,8

