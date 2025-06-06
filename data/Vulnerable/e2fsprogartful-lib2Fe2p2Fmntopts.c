/*
 * mountopts.c --- convert between default mount options and strings
 *
 * Copyright (C) 2002  Theodore Ts'o <tytso@mit.edu>
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Library
 * General Public License, version 2.
 * %End-Header%
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>

#include "e2p.h"

struct mntopt {
	unsigned int	mask;
	const char	*string;
};

static struct mntopt mntopt_list[] = {
	{ EXT2_DEFM_DEBUG,	"debug" },
	{ EXT2_DEFM_BSDGROUPS,	"bsdgroups" },
	{ EXT2_DEFM_XATTR_USER,	"user_xattr" },
	{ EXT2_DEFM_ACL,	"acl" },
	{ EXT2_DEFM_UID16,	"uid16" },
	{ EXT3_DEFM_JMODE_DATA, "journal_data" },
	{ EXT3_DEFM_JMODE_ORDERED, "journal_data_ordered" },
	{ EXT3_DEFM_JMODE_WBACK, "journal_data_writeback" },
	{ EXT4_DEFM_NOBARRIER,	"nobarrier" },
	{ EXT4_DEFM_BLOCK_VALIDITY, "block_validity" },
	{ EXT4_DEFM_DISCARD,	"discard"},
	{ EXT4_DEFM_NODELALLOC,	"nodelalloc"},
	{ 0, 0 },
};

const char *e2p_mntopt2string(unsigned int mask)
{
	struct mntopt  *f;
	static char buf[20];
	int	fnum;

	for (f = mntopt_list; f->string; f++) {
		if (mask == f->mask)
			return f->string;
	}
	for (fnum = 0; mask >>= 1; fnum++);
	sprintf(buf, "MNTOPT_%d", fnum);
	return buf;
}

int e2p_string2mntopt(char *string, unsigned int *mask)
{
	struct mntopt  *f;
	char		*eptr;
	int		num;

	for (f = mntopt_list; f->string; f++) {
		if (!strcasecmp(string, f->string)) {
			*mask = f->mask;
			return 0;
		}
	}
	if (strncasecmp(string, "MNTOPT_", 7))
		return 1;

	if (string[8] == 0)
		return 1;
	num = strtol(string+8, &eptr, 10);
	if (num > 31 || num < 0)
		return 1;
	if (*eptr)
		return 1;
	*mask = 1 << num;
	return 0;
}

static char *skip_over_blanks(char *cp)
{
	while (*cp && isspace(*cp))
		cp++;
	return cp;
}

static char *skip_over_word(char *cp)
{
	while (*cp && !isspace(*cp) && *cp != ',')
		cp++;
	return cp;
}

/*
 * Edit a mntopt set array as requested by the user.  The ok
 * parameter, if non-zero, allows the application to limit what
 * mntopts the user is allowed to set or clear using this function.
 */
int e2p_edit_mntopts(const char *str, __u32 *mntopts, __u32 ok)
{
	char	*cp, *buf, *next;
	int	neg;
	unsigned int	mask;
	int	rc = 0;

	buf = malloc(strlen(str)+1);
	if (!buf)
		return 1;
	strcpy(buf, str);
	cp = buf;
	while (cp && *cp) {
		neg = 0;
		cp = skip_over_blanks(cp);
		next = skip_over_word(cp);
		if (*next == 0)
			next = 0;
		else
			*next = 0;
		switch (*cp) {
		case '-':
		case '^':
			neg++;
			/* fallthrough */
		case '+':
			cp++;
			break;
		}
		if (e2p_string2mntopt(cp, &mask)) {
			rc = 1;
			break;
		}
		if (ok && !(ok & mask)) {
			rc = 1;
			break;
		}
		if (mask & EXT3_DEFM_JMODE)
			*mntopts &= ~EXT3_DEFM_JMODE;
		if (neg)
			*mntopts &= ~mask;
		else
			*mntopts |= mask;
		cp = next ? next+1 : 0;
	}
	free(buf);
	return rc;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 54,1;54,8

// 113,1;113,7

