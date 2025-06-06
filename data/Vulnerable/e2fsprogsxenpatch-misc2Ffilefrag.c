/*
 * filefrag.c -- report if a particular file is fragmented
 *
 * Copyright 2003 by Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#include "config.h"
#ifndef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
	fputs("This program is only supported on Linux!\n", stderr);
	exit(EXIT_FAILURE);
}
#else
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern char *optarg;
extern int optind;
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <linux/fd.h>
#include <ext2fs/ext2fs.h>
#include <ext2fs/ext2_types.h>
#include <ext2fs/fiemap.h>

int verbose = 0;
int blocksize;		/* Use specified blocksize (default 1kB) */
int sync_file = 0;	/* fsync file before getting the mapping */
int xattr_map = 0;	/* get xattr mapping */
int force_bmap;	/* force use of FIBMAP instead of FIEMAP */
int force_extent;	/* print output in extent format always */
int logical_width = 8;
int physical_width = 10;
const char *ext_fmt = "%4d: %*llu..%*llu: %*llu..%*llu: %6llu: %s\n";
const char *hex_fmt = "%4d: %*llx..%*llx: %*llx..%*llx: %6llx: %s\n";

#define FILEFRAG_FIEMAP_FLAGS_COMPAT (FIEMAP_FLAG_SYNC | FIEMAP_FLAG_XATTR)

#define FIBMAP		_IO(0x00, 1)	/* bmap access */
#define FIGETBSZ	_IO(0x00, 2)	/* get the block size used for bmap */

#define LUSTRE_SUPER_MAGIC 0x0BD00BD0

#define	EXT4_EXTENTS_FL			0x00080000 /* Inode uses extents */
#define	EXT3_IOC_GETFLAGS		_IOR('f', 1, long)

static int int_log2(int arg)
{
	int     l = 0;

	arg >>= 1;
	while (arg) {
		l++;
		arg >>= 1;
	}
	return l;
}

static int int_log10(unsigned long long arg)
{
	int     l = 0;

	arg = arg / 10;
	while (arg) {
		l++;
		arg = arg / 10;
	}
	return l;
}

static unsigned int div_ceil(unsigned int a, unsigned int b)
{
	if (!a)
		return 0;
	return ((a - 1) / b) + 1;
}

static int get_bmap(int fd, unsigned long block, unsigned long *phy_blk)
{
	int	ret;
	unsigned int b;

	b = block;
	ret = ioctl(fd, FIBMAP, &b); /* FIBMAP takes pointer to integer */
	if (ret < 0)
		return -errno;
	*phy_blk = b;

	return ret;
}

static void print_extent_header(void)
{
	printf(" ext: %*s %*s length: %*s flags:\n",
	       logical_width * 2 + 3,
	       "logical_offset:",
	       physical_width * 2 + 3, "physical_offset:",
	       physical_width + 1,
	       "expected:");
}

static void print_flag(__u32 *flags, __u32 mask, char *buf, const char *name)
{
	if ((*flags & mask) == 0)
		return;

	strcat(buf, name);
	*flags &= ~mask;
}

static void print_extent_info(struct fiemap_extent *fm_extent, int cur_ex,
			      unsigned long long expected, int blk_shift,
			      ext2fs_struct_stat *st)
{
	unsigned long long physical_blk;
	unsigned long long logical_blk;
	unsigned long long ext_len;
	unsigned long long ext_blks;
	__u32 fe_flags, mask;
	char flags[256] = "";

	/* For inline data all offsets should be in bytes, not blocks */
	if (fm_extent->fe_flags & FIEMAP_EXTENT_DATA_INLINE)
		blk_shift = 0;

	ext_len = fm_extent->fe_length >> blk_shift;
	ext_blks = (fm_extent->fe_length - 1) >> blk_shift;
	logical_blk = fm_extent->fe_logical >> blk_shift;
	if (fm_extent->fe_flags & FIEMAP_EXTENT_UNKNOWN) {
		physical_blk = 0;
	} else {
		physical_blk = fm_extent->fe_physical >> blk_shift;
	}

	if (expected)
		sprintf(flags, ext_fmt == hex_fmt ? "%*llx: " : "%*llu: ",
			physical_width, expected >> blk_shift);
	else
		sprintf(flags, "%.*s  ", physical_width, "                   ");

	fe_flags = fm_extent->fe_flags;
	print_flag(&fe_flags, FIEMAP_EXTENT_LAST, flags, "last,");
	print_flag(&fe_flags, FIEMAP_EXTENT_UNKNOWN, flags, "unknown_loc,");
	print_flag(&fe_flags, FIEMAP_EXTENT_DELALLOC, flags, "delalloc,");
	print_flag(&fe_flags, FIEMAP_EXTENT_ENCODED, flags, "encoded,");
	print_flag(&fe_flags, FIEMAP_EXTENT_DATA_ENCRYPTED, flags,"encrypted,");
	print_flag(&fe_flags, FIEMAP_EXTENT_NOT_ALIGNED, flags, "not_aligned,");
	print_flag(&fe_flags, FIEMAP_EXTENT_DATA_INLINE, flags, "inline,");
	print_flag(&fe_flags, FIEMAP_EXTENT_DATA_TAIL, flags, "tail_packed,");
	print_flag(&fe_flags, FIEMAP_EXTENT_UNWRITTEN, flags, "unwritten,");
	print_flag(&fe_flags, FIEMAP_EXTENT_MERGED, flags, "merged,");
	print_flag(&fe_flags, FIEMAP_EXTENT_SHARED, flags, "shared,");
	/* print any unknown flags as hex values */
	for (mask = 1; fe_flags != 0 && mask != 0; mask <<= 1) {
		char hex[6];

		if ((fe_flags & mask) == 0)
			continue;
		sprintf(hex, "%#04x,", mask);
		print_flag(&fe_flags, mask, flags, hex);
	}

	if (fm_extent->fe_logical + fm_extent->fe_length >= st->st_size)
		strcat(flags, "eof,");

	/* Remove trailing comma, if any */
	if (flags[0] != '\0')
		flags[strnlen(flags, sizeof(flags)) - 1] = '\0';

	printf(ext_fmt, cur_ex, logical_width, logical_blk,
	       logical_width, logical_blk + ext_blks,
	       physical_width, physical_blk,
	       physical_width, physical_blk + ext_blks,
	       ext_len, flags);
}

static int filefrag_fiemap(int fd, int blk_shift, int *num_extents,
			   ext2fs_struct_stat *st)
{
	char buf[16384];
	struct fiemap *fiemap = (struct fiemap *)buf;
	struct fiemap_extent *fm_ext = &fiemap->fm_extents[0];
	int count = (sizeof(buf) - sizeof(*fiemap)) /
			sizeof(struct fiemap_extent);
	unsigned long long expected = 0;
	unsigned long flags = 0;
	unsigned int i;
	int fiemap_header_printed = 0;
	int tot_extents = 0, n = 0;
	int last = 0;
	int rc;

	memset(fiemap, 0, sizeof(struct fiemap));

	if (sync_file)
		flags |= FIEMAP_FLAG_SYNC;

	if (xattr_map)
		flags |= FIEMAP_FLAG_XATTR;

	do {
		fiemap->fm_length = ~0ULL;
		fiemap->fm_flags = flags;
		fiemap->fm_extent_count = count;
		rc = ioctl(fd, FS_IOC_FIEMAP, (unsigned long) fiemap);
		if (rc < 0) {
			static int fiemap_incompat_printed;

			rc = -errno;
			if (rc == -EBADR && !fiemap_incompat_printed) {
				fprintf(stderr, "FIEMAP failed with unknown "
						"flags %x\n",
				       fiemap->fm_flags);
				fiemap_incompat_printed = 1;
			}
			return rc;
		}

		/* If 0 extents are returned, then more ioctls are not needed */
		if (fiemap->fm_mapped_extents == 0)
			break;

		if (verbose && !fiemap_header_printed) {
			print_extent_header();
			fiemap_header_printed = 1;
		}

		for (i = 0; i < fiemap->fm_mapped_extents; i++) {
			if (fm_ext[i].fe_logical != 0 &&
			    fm_ext[i].fe_physical != expected) {
				tot_extents++;
			} else {
				expected = 0;
				if (!tot_extents)
					tot_extents = 1;
			}
			if (verbose)
				print_extent_info(&fm_ext[i], n, expected,
						  blk_shift, st);

			expected = fm_ext[i].fe_physical + fm_ext[i].fe_length;
			if (fm_ext[i].fe_flags & FIEMAP_EXTENT_LAST)
				last = 1;
			n++;
		}

		fiemap->fm_start = (fm_ext[i - 1].fe_logical +
				    fm_ext[i - 1].fe_length);
	} while (last == 0);

	*num_extents = tot_extents;

	return 0;
}

#define EXT2_DIRECT	12

static int filefrag_fibmap(int fd, int blk_shift, int *num_extents,
			   ext2fs_struct_stat *st,
			   unsigned long numblocks, int is_ext2)
{
	struct fiemap_extent	fm_ext;
	unsigned long		i, last_block;
	unsigned long long	logical;
				/* Blocks per indirect block */
	const long		bpib = st->st_blksize / 4;
	int			count;

	if (force_extent) {
		memset(&fm_ext, 0, sizeof(fm_ext));
		fm_ext.fe_flags = FIEMAP_EXTENT_MERGED;
	}

	if (sync_file)
		fsync(fd);

	for (i = 0, logical = 0, *num_extents = 0, count = last_block = 0;
	     i < numblocks;
	     i++, logical += st->st_blksize) {
		unsigned long block = 0;
		int rc;

		if (is_ext2 && last_block) {
			if (((i - EXT2_DIRECT) % bpib) == 0)
				last_block++;
			if (((i - EXT2_DIRECT - bpib) % (bpib * bpib)) == 0)
				last_block++;
			if (((i - EXT2_DIRECT - bpib - bpib * bpib) %
			     (((unsigned long long)bpib) * bpib * bpib)) == 0)
				last_block++;
		}
		rc = get_bmap(fd, i, &block);
		if (rc < 0)
			return rc;
		if (block == 0)
			continue;
		if (*num_extents == 0) {
			(*num_extents)++;
			if (force_extent) {
				print_extent_header();
				fm_ext.fe_physical = block * st->st_blksize;
			}
		}
		count++;
		if (force_extent && last_block != 0 &&
		    (block != last_block + 1 ||
		     fm_ext.fe_logical + fm_ext.fe_length != logical)) {
			print_extent_info(&fm_ext, *num_extents - 1,
					  (last_block + 1) * st->st_blksize,
					  blk_shift, st);
			fm_ext.fe_length = 0;
			(*num_extents)++;
		} else if (last_block && (block != last_block + 1)) {
			if (verbose)
				printf("Discontinuity: Block %ld is at %lu (was "
				       "%lu)\n", i, block, last_block + 1);
			fm_ext.fe_length = 0;
			(*num_extents)++;
		}
		fm_ext.fe_logical = logical;
		fm_ext.fe_physical = block * st->st_blksize;
		fm_ext.fe_length += st->st_blksize;
		last_block = block;
	}

	if (force_extent)
		print_extent_info(&fm_ext, *num_extents - 1,
				  last_block * st->st_blksize, blk_shift, st);

	return count;
}

static int frag_report(const char *filename)
{
	static struct statfs fsinfo;
	static unsigned int blksize;
	ext2fs_struct_stat st;
	int		blk_shift;
	long		fd;
	unsigned long long	numblocks;
	int		data_blocks_per_cyl = 1;
	int		num_extents = 1, expected = ~0;
	int		is_ext2 = 0;
	static dev_t	last_device;
	int		width;
	int		rc = 0;

#if defined(HAVE_OPEN64) && !defined(__OSX_AVAILABLE_BUT_DEPRECATED)
	fd = open64(filename, O_RDONLY);
#else
	fd = open(filename, O_RDONLY);
#endif
	if (fd < 0) {
		rc = -errno;
		perror("open");
		return rc;
	}

#if defined(HAVE_FSTAT64) && !defined(__OSX_AVAILABLE_BUT_DEPRECATED)
	if (fstat64(fd, &st) < 0) {
#else
	if (fstat(fd, &st) < 0) {
#endif
		rc = -errno;
		perror("stat");
		goto out_close;
	}

	if (last_device != st.st_dev) {
		if (fstatfs(fd, &fsinfo) < 0) {
			rc = -errno;
			perror("fstatfs");
			goto out_close;
		}
		if (ioctl(fd, FIGETBSZ, &blksize) < 0)
			blksize = fsinfo.f_bsize;
		if (verbose)
			printf("Filesystem type is: %lx\n",
			       (unsigned long)fsinfo.f_type);
	}
	st.st_blksize = blksize;
	if (fsinfo.f_type == 0xef51 || fsinfo.f_type == 0xef52 ||
	    fsinfo.f_type == 0xef53) {
		unsigned int	flags;

		if (ioctl(fd, EXT3_IOC_GETFLAGS, &flags) == 0 &&
		    !(flags & EXT4_EXTENTS_FL))
			is_ext2 = 1;
	}

	if (is_ext2) {
		long cylgroups = div_ceil(fsinfo.f_blocks, blksize * 8);

		if (verbose && last_device != st.st_dev)
			printf("Filesystem cylinder groups approximately %ld\n",
			       cylgroups);

		data_blocks_per_cyl = blksize * 8 -
					(fsinfo.f_files / 8 / cylgroups) - 3;
	}
	last_device = st.st_dev;

	width = int_log10(fsinfo.f_blocks);
	if (width > physical_width)
		physical_width = width;

	numblocks = (st.st_size + blksize - 1) / blksize;
	if (blocksize != 0)
		blk_shift = int_log2(blocksize);
	else
		blk_shift = int_log2(blksize);

	width = int_log10(numblocks);
	if (width > logical_width)
		logical_width = width;
	if (verbose)
		printf("File size of %s is %llu (%llu block%s of %d bytes)\n",
		       filename, (unsigned long long)st.st_size,
		       numblocks * blksize >> blk_shift,
		       numblocks == 1 ? "" : "s", 1 << blk_shift);

	if (!force_bmap) {
		rc = filefrag_fiemap(fd, blk_shift, &num_extents, &st);
		expected = 0;
	}

	if (force_bmap || rc < 0) { /* FIEMAP failed, try FIBMAP instead */
		expected = filefrag_fibmap(fd, blk_shift, &num_extents,
					   &st, numblocks, is_ext2);
		if (expected < 0) {
			if (expected == -EINVAL || expected == -ENOTTY) {
				fprintf(stderr, "%s: FIBMAP unsupported\n",
					filename);
			} else if (expected == -EPERM) {
				fprintf(stderr,
					"%s: FIBMAP requires root privileges\n",
					filename);
			} else {
				fprintf(stderr, "%s: FIBMAP error: %s",
					filename, strerror(expected));
			}
			rc = expected;
			goto out_close;
		} else {
			rc = 0;
		}
		expected = expected / data_blocks_per_cyl + 1;
	}

	if (num_extents == 1)
		printf("%s: 1 extent found", filename);
	else
		printf("%s: %d extents found", filename, num_extents);
	/* count, and thus expected, only set for indirect FIBMAP'd files */
	if (is_ext2 && expected && expected < num_extents)
		printf(", perfection would be %d extent%s\n", expected,
			(expected > 1) ? "s" : "");
	else
		fputc('\n', stdout);
out_close:
	close(fd);

	return rc;
}

static void usage(const char *progname)
{
	fprintf(stderr, "Usage: %s [-b{blocksize}] [-BeklsvxX] file ...\n",
		progname);
	exit(1);
}

int main(int argc, char**argv)
{
	char **cpp;
	int rc = 0, c;

	while ((c = getopt(argc, argv, "Bb::eksvxX")) != EOF) {
		switch (c) {
		case 'B':
			force_bmap++;
			break;
		case 'b':
			if (optarg) {
				char *end;
				blocksize = strtoul(optarg, &end, 0);
				if (end) {
					switch (end[0]) {
					case 'g':
					case 'G':
						blocksize *= 1024;
						/* no break */
					case 'm':
					case 'M':
						blocksize *= 1024;
						/* no break */
					case 'k':
					case 'K':
						blocksize *= 1024;
						break;
					default:
						break;
					}
				}
			} else { /* Allow -b without argument for compat. Remove
				  * this eventually so "-b {blocksize}" works */
				fprintf(stderr, "%s: -b needs a blocksize "
					"option, assuming 1024-byte blocks.\n",
					argv[0]);
				blocksize = 1024;
			}
			break;
		case 'e':
			force_extent++;
			if (!verbose)
				verbose++;
			break;
		case 'k':
			blocksize = 1024;
			break;
		case 's':
			sync_file++;
			break;
		case 'v':
			verbose++;
			break;
		case 'x':
			xattr_map++;
			break;
		case 'X':
			ext_fmt = hex_fmt;
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	if (optind == argc)
		usage(argv[0]);

	for (cpp = argv + optind; *cpp != '\0'; cpp++) {
		int rc2 = frag_report(*cpp);

		if (rc2 < 0 && rc == 0)
			rc = rc2;
	}

	return -rc;
}
#endif

//						↓↓↓VULNERABLE LINES↓↓↓

// 128,1;128,7

// 157,2;157,9

// 160,2;160,9

// 180,2;180,9

// 185,2;185,8

