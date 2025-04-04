/*
 * Copyright (C) 2013-2016  Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef RDATA_GENERIC_L64_106_C
#define RDATA_GENERIC_L64_106_C

#include <string.h>

#include <isc/net.h>

#define RRTYPE_L64_ATTRIBUTES (0)

static inline isc_result_t
fromtext_l64(ARGS_FROMTEXT) {
	isc_token_t token;
	unsigned char locator[NS_LOCATORSZ];

	REQUIRE(type == dns_rdatatype_l64);

	UNUSED(type);
	UNUSED(rdclass);
	UNUSED(origin);
	UNUSED(options);
	UNUSED(callbacks);

	RETERR(isc_lex_getmastertoken(lexer, &token, isc_tokentype_number,
				      ISC_FALSE));
	if (token.value.as_ulong > 0xffffU)
		RETTOK(ISC_R_RANGE);
	RETERR(uint16_tobuffer(token.value.as_ulong, target));

	RETERR(isc_lex_getmastertoken(lexer, &token, isc_tokentype_string,
				      ISC_FALSE));

	if (locator_pton(DNS_AS_STR(token), locator) != 1)
		RETTOK(DNS_R_SYNTAX);
	return (mem_tobuffer(target, locator, NS_LOCATORSZ));
}

static inline isc_result_t
totext_l64(ARGS_TOTEXT) {
	isc_region_t region;
	char buf[sizeof("xxxx:xxxx:xxxx:xxxx")];
	unsigned short num;

	REQUIRE(rdata->type == dns_rdatatype_l64);
	REQUIRE(rdata->length == 10);

	UNUSED(tctx);

	dns_rdata_toregion(rdata, &region);
	num = uint16_fromregion(&region);
	isc_region_consume(&region, 2);
	sprintf(buf, "%u", num);
	RETERR(str_totext(buf, target));

	RETERR(str_totext(" ", target));

	sprintf(buf, "%x:%x:%x:%x",
		region.base[0]<<8 | region.base[1],
		region.base[2]<<8 | region.base[3],
		region.base[4]<<8 | region.base[5],
		region.base[6]<<8 | region.base[7]);
	return (str_totext(buf, target));
}

static inline isc_result_t
fromwire_l64(ARGS_FROMWIRE) {
	isc_region_t sregion;

	REQUIRE(type == dns_rdatatype_l64);

	UNUSED(type);
	UNUSED(options);
	UNUSED(rdclass);
	UNUSED(dctx);

	isc_buffer_activeregion(source, &sregion);
	if (sregion.length != 10)
		return (DNS_R_FORMERR);
	isc_buffer_forward(source, sregion.length);
	return (mem_tobuffer(target, sregion.base, sregion.length));
}

static inline isc_result_t
towire_l64(ARGS_TOWIRE) {

	REQUIRE(rdata->type == dns_rdatatype_l64);
	REQUIRE(rdata->length == 10);

	UNUSED(cctx);

	return (mem_tobuffer(target, rdata->data, rdata->length));
}

static inline int
compare_l64(ARGS_COMPARE) {
	isc_region_t region1;
	isc_region_t region2;

	REQUIRE(rdata1->type == rdata2->type);
	REQUIRE(rdata1->rdclass == rdata2->rdclass);
	REQUIRE(rdata1->type == dns_rdatatype_l64);
	REQUIRE(rdata1->length == 10);
	REQUIRE(rdata2->length == 10);

	dns_rdata_toregion(rdata1, &region1);
	dns_rdata_toregion(rdata2, &region2);
	return (isc_region_compare(&region1, &region2));
}

static inline isc_result_t
fromstruct_l64(ARGS_FROMSTRUCT) {
	dns_rdata_l64_t *l64 = source;

	REQUIRE(type == dns_rdatatype_l64);
	REQUIRE(source != NULL);
	REQUIRE(l64->common.rdtype == type);
	REQUIRE(l64->common.rdclass == rdclass);

	UNUSED(type);
	UNUSED(rdclass);

	RETERR(uint16_tobuffer(l64->pref, target));
	return (mem_tobuffer(target, l64->l64, sizeof(l64->l64)));
}

static inline isc_result_t
tostruct_l64(ARGS_TOSTRUCT) {
	isc_region_t region;
	dns_rdata_l64_t *l64 = target;

	REQUIRE(rdata->type == dns_rdatatype_l64);
	REQUIRE(target != NULL);
	REQUIRE(rdata->length == 10);

	UNUSED(mctx);

	l64->common.rdclass = rdata->rdclass;
	l64->common.rdtype = rdata->type;
	ISC_LINK_INIT(&l64->common, link);

	dns_rdata_toregion(rdata, &region);
	l64->pref = uint16_fromregion(&region);
	memmove(l64->l64, region.base, region.length);
	return (ISC_R_SUCCESS);
}

static inline void
freestruct_l64(ARGS_FREESTRUCT) {
	dns_rdata_l64_t *l64 = source;

	REQUIRE(source != NULL);
	REQUIRE(l64->common.rdtype == dns_rdatatype_l64);

	return;
}

static inline isc_result_t
additionaldata_l64(ARGS_ADDLDATA) {

	REQUIRE(rdata->type == dns_rdatatype_l64);
	REQUIRE(rdata->length == 10);

	UNUSED(rdata);
	UNUSED(add);
	UNUSED(arg);

	return (ISC_R_SUCCESS);
}

static inline isc_result_t
digest_l64(ARGS_DIGEST) {
	isc_region_t r;

	REQUIRE(rdata->type == dns_rdatatype_l64);
	REQUIRE(rdata->length == 10);

	dns_rdata_toregion(rdata, &r);

	return ((digest)(arg, &r));
}

static inline isc_boolean_t
checkowner_l64(ARGS_CHECKOWNER) {

	REQUIRE(type == dns_rdatatype_l64);

	UNUSED(name);
	UNUSED(type);
	UNUSED(rdclass);
	UNUSED(wildcard);

	return (ISC_TRUE);
}

static inline isc_boolean_t
checknames_l64(ARGS_CHECKNAMES) {

	REQUIRE(rdata->type == dns_rdatatype_l64);
	REQUIRE(rdata->length == 10);

	UNUSED(rdata);
	UNUSED(owner);
	UNUSED(bad);

	return (ISC_TRUE);
}

static inline int
casecompare_l64(ARGS_COMPARE) {
	return (compare_l64(rdata1, rdata2));
}

#endif	/* RDATA_GENERIC_L64_106_C */

//						↓↓↓VULNERABLE LINES↓↓↓

// 59,1;59,8

// 64,1;64,8

