/*
 * util.c --- helper functions used by tune2fs and mke2fs
 *
 * Copyright 1995, 1996, 1997, 1998, 1999, 2000 by Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include "config.h"
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_LINUX_MAJOR_H
#include <linux/major.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <time.h>

#include "et/com_err.h"
#include "e2p/e2p.h"
#include "ext2fs/ext2_fs.h"
#include "ext2fs/ext2fs.h"
#include "nls-enable.h"
#include "blkid/blkid.h"
#include "util.h"

char *journal_location_string = NULL;

#ifndef HAVE_STRCASECMP
int strcasecmp (char *s1, char *s2)
{
	while (*s1 && *s2) {
		int ch1 = *s1++, ch2 = *s2++;
		if (isupper (ch1))
			ch1 = tolower (ch1);
		if (isupper (ch2))
			ch2 = tolower (ch2);
		if (ch1 != ch2)
			return ch1 - ch2;
	}
	return *s1 ? 1 : *s2 ? -1 : 0;
}
#endif

/*
 * Given argv[0], return the program name.
 */
char *get_progname(char *argv_zero)
{
	char	*cp;

	cp = strrchr(argv_zero, '/');
	if (!cp )
		return argv_zero;
	else
		return cp+1;
}

static jmp_buf alarm_env;

static void alarm_signal(int signal)
{
	longjmp(alarm_env, 1);
}

void proceed_question(int delay)
{
	char buf[256];
	const char *short_yes = _("yY");

	fflush(stdout);
	fflush(stderr);
	if (delay > 0) {
		if (setjmp(alarm_env)) {
			signal(SIGALRM, SIG_IGN);
			printf("%s", _("<proceeding>\n"));
			return;
		}
		signal(SIGALRM, alarm_signal);
		printf(_("Proceed anyway (or wait %d seconds) ? (y,n) "),
		       delay);
		alarm(delay);
	} else
		fputs(_("Proceed anyway? (y,n) "), stdout);
	buf[0] = 0;
	if (!fgets(buf, sizeof(buf), stdin) ||
	    strchr(short_yes, buf[0]) == 0) {
		putc('\n', stdout);
		exit(1);
	}
	signal(SIGALRM, SIG_IGN);
}

static void print_ext2_info(const char *device)

{
	struct ext2_super_block	*sb;
	ext2_filsys		fs;
	errcode_t		retval;
	time_t 			tm;
	char			buf[80];

	retval = ext2fs_open2(device, 0, EXT2_FLAG_64BITS, 0, 0,
			      unix_io_manager, &fs);
	if (retval)
		return;
	sb = fs->super;

	if (sb->s_mtime) {
		tm = sb->s_mtime;
		if (sb->s_last_mounted[0]) {
			memset(buf, 0, sizeof(buf));
			strncpy(buf, sb->s_last_mounted,
				sizeof(sb->s_last_mounted));
			printf(_("\tlast mounted on %s on %s"), buf,
			       ctime(&tm));
		} else
			printf(_("\tlast mounted on %s"), ctime(&tm));
	} else if (sb->s_mkfs_time) {
		tm = sb->s_mkfs_time;
		printf(_("\tcreated on %s"), ctime(&tm));
	} else if (sb->s_wtime) {
		tm = sb->s_wtime;
		printf(_("\tlast modified on %s"), ctime(&tm));
	}
	ext2fs_close_free(&fs);
}

/*
 * return 1 if there is no partition table, 0 if a partition table is
 * detected, and -1 on an error.
 */
static int check_partition_table(const char *device)
{
#ifdef HAVE_BLKID_PROBE_ENABLE_PARTITIONS
	blkid_probe pr;
	const char *value;
	int ret;

	pr = blkid_new_probe_from_filename(device);
	if (!pr)
		return -1;

        ret = blkid_probe_enable_partitions(pr, 1);
        if (ret < 0)
		goto errout;

	ret = blkid_probe_enable_superblocks(pr, 0);
	if (ret < 0)
		goto errout;

	ret = blkid_do_fullprobe(pr);
	if (ret < 0)
		goto errout;

	ret = blkid_probe_lookup_value(pr, "PTTYPE", &value, NULL);
	if (ret == 0)
		fprintf(stderr, _("Found a %s partition table in %s\n"),
			value, device);
	else
		ret = 1;

errout:
	blkid_free_probe(pr);
	return ret;
#else
	return -1;
#endif
}

/*
 * return 1 if the device looks plausible, creating the file if necessary
 */
int check_plausibility(const char *device, int flags, int *ret_is_dev)
{
	int fd, ret, is_dev = 0;
	ext2fs_struct_stat s;
	int fl = O_RDONLY;
	blkid_cache cache = NULL;
	char *fs_type = NULL;
	char *fs_label = NULL;

	fd = ext2fs_open_file(device, fl, 0666);
	if ((fd < 0) && (errno == ENOENT) && (flags & NO_SIZE)) {
		fprintf(stderr, _("The file %s does not exist and no "
				  "size was specified.\n"), device);
		exit(1);
	}
	if ((fd < 0) && (errno == ENOENT) && (flags & CREATE_FILE)) {
		fl |= O_CREAT;
		fd = ext2fs_open_file(device, fl, 0666);
		if (fd >= 0 && (flags & VERBOSE_CREATE))
			printf(_("Creating regular file %s\n"), device);
	}
	if (fd < 0) {
		fprintf(stderr, _("Could not open %s: %s\n"),
			device, error_message(errno));
		if (errno == ENOENT)
			fputs(_("\nThe device apparently does not exist; "
				"did you specify it correctly?\n"), stderr);
		exit(1);
	}

	if (ext2fs_fstat(fd, &s) < 0) {
		perror("stat");
		exit(1);
	}
	close(fd);

	if (S_ISBLK(s.st_mode))
		is_dev = 1;
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	/* On FreeBSD, all disk devices are character specials */
	if (S_ISCHR(s.st_mode))
		is_dev = 1;
#endif
	if (ret_is_dev)
		*ret_is_dev = is_dev;

	if ((flags & CHECK_BLOCK_DEV) && !is_dev) {
		printf(_("%s is not a block special device.\n"), device);
		return 0;
	}

	/*
	 * Note: we use the older-style blkid API's here because we
	 * want as much functionality to be available when using the
	 * internal blkid library, when e2fsprogs is compiled for
	 * non-Linux systems that will probably not have the libraries
	 * from util-linux available.  We only use the newer
	 * blkid-probe interfaces to access functionality not
	 * available in the original blkid library.
	 */
	if ((flags & CHECK_FS_EXIST) && blkid_get_cache(&cache, NULL) >= 0) {
		fs_type = blkid_get_tag_value(cache, "TYPE", device);
		if (fs_type)
			fs_label = blkid_get_tag_value(cache, "LABEL", device);
		blkid_put_cache(cache);
	}

	if (fs_type) {
		if (fs_label)
			printf(_("%s contains a %s file system "
				 "labelled '%s'\n"), device, fs_type, fs_label);
		else
			printf(_("%s contains a %s file system\n"), device,
			       fs_type);
		if (strncmp(fs_type, "ext", 3) == 0)
			print_ext2_info(device);
		free(fs_type);
		free(fs_label);
		return 0;
	}

	ret = check_partition_table(device);
	if (ret >= 0)
		return ret;

#ifdef HAVE_LINUX_MAJOR_H
#ifndef MAJOR
#define MAJOR(dev)	((dev)>>8)
#define MINOR(dev)	((dev) & 0xff)
#endif
#ifndef SCSI_BLK_MAJOR
#ifdef SCSI_DISK0_MAJOR
#ifdef SCSI_DISK8_MAJOR
#define SCSI_DISK_MAJOR(M) ((M) == SCSI_DISK0_MAJOR || \
  ((M) >= SCSI_DISK1_MAJOR && (M) <= SCSI_DISK7_MAJOR) || \
  ((M) >= SCSI_DISK8_MAJOR && (M) <= SCSI_DISK15_MAJOR))
#else
#define SCSI_DISK_MAJOR(M) ((M) == SCSI_DISK0_MAJOR || \
  ((M) >= SCSI_DISK1_MAJOR && (M) <= SCSI_DISK7_MAJOR))
#endif /* defined(SCSI_DISK8_MAJOR) */
#define SCSI_BLK_MAJOR(M) (SCSI_DISK_MAJOR((M)) || (M) == SCSI_CDROM_MAJOR)
#else
#define SCSI_BLK_MAJOR(M)  ((M) == SCSI_DISK_MAJOR || (M) == SCSI_CDROM_MAJOR)
#endif /* defined(SCSI_DISK0_MAJOR) */
#endif /* defined(SCSI_BLK_MAJOR) */
	if (((MAJOR(s.st_rdev) == HD_MAJOR &&
	      MINOR(s.st_rdev)%64 == 0) ||
	     (SCSI_BLK_MAJOR(MAJOR(s.st_rdev)) &&
	      MINOR(s.st_rdev)%16 == 0))) {
		printf(_("%s is entire device, not just one partition!\n"),
		       device);
		return 0;
	}
#endif
	return 1;
}

void check_mount(const char *device, int force, const char *type)
{
	errcode_t	retval;
	int		mount_flags;

	retval = ext2fs_check_if_mounted(device, &mount_flags);
	if (retval) {
		com_err("ext2fs_check_if_mount", retval,
			_("while determining whether %s is mounted."),
			device);
		return;
	}
	if (mount_flags & EXT2_MF_MOUNTED) {
		fprintf(stderr, _("%s is mounted; "), device);
		if (force >= 2) {
			fputs(_("mke2fs forced anyway.  Hope /etc/mtab is "
				"incorrect.\n"), stderr);
			return;
		}
	abort_mke2fs:
		fprintf(stderr, _("will not make a %s here!\n"), type);
		exit(1);
	}
	if (mount_flags & EXT2_MF_BUSY) {
		fprintf(stderr, _("%s is apparently in use by the system; "),
			device);
		if (force >= 2) {
			fputs(_("mke2fs forced anyway.\n"), stderr);
			return;
		}
		goto abort_mke2fs;
	}
}

void parse_journal_opts(const char *opts)
{
	char	*buf, *token, *next, *p, *arg;
	int	len;
	int	journal_usage = 0;

	len = strlen(opts);
	buf = malloc(len+1);
	if (!buf) {
		fputs(_("Couldn't allocate memory to parse journal "
			"options!\n"), stderr);
		exit(1);
	}
	strcpy(buf, opts);
	for (token = buf; token && *token; token = next) {
		p = strchr(token, ',');
		next = 0;
		if (p) {
			*p = 0;
			next = p+1;
		}
		arg = strchr(token, '=');
		if (arg) {
			*arg = 0;
			arg++;
		}
#if 0
		printf("Journal option=%s, argument=%s\n", token,
		       arg ? arg : "NONE");
#endif
		if (strcmp(token, "device") == 0) {
			journal_device = blkid_get_devname(NULL, arg, NULL);
			if (!journal_device) {
				if (arg)
					fprintf(stderr, _("\nCould not find "
						"journal device matching %s\n"),
						arg);
				journal_usage++;
				continue;
			}
		} else if (strcmp(token, "size") == 0) {
			if (!arg) {
				journal_usage++;
				continue;
			}
			journal_size = strtoul(arg, &p, 0);
			if (*p)
				journal_usage++;
		} else if (!strcmp(token, "location")) {
			if (!arg) {
				journal_usage++;
				continue;
			}
			journal_location_string = strdup(arg);
		} else if (strcmp(token, "v1_superblock") == 0) {
			journal_flags |= EXT2_MKJOURNAL_V1_SUPER;
			continue;
		} else
			journal_usage++;
	}
	if (journal_usage) {
		fputs(_("\nBad journal options specified.\n\n"
			"Journal options are separated by commas, "
			"and may take an argument which\n"
			"\tis set off by an equals ('=') sign.\n\n"
			"Valid journal options are:\n"
			"\tsize=<journal size in megabytes>\n"
			"\tdevice=<journal device>\n"
			"\tlocation=<journal location>\n\n"
			"The journal size must be between "
			"1024 and 10240000 filesystem blocks.\n\n"), stderr);
		free(buf);
		exit(1);
	}
	free(buf);
}

/*
 * Determine the number of journal blocks to use, either via
 * user-specified # of megabytes, or via some intelligently selected
 * defaults.
 *
 * Find a reasonable journal file size (in blocks) given the number of blocks
 * in the filesystem.  For very small filesystems, it is not reasonable to
 * have a journal that fills more than half of the filesystem.
 */
unsigned int figure_journal_size(int size, ext2_filsys fs)
{
	int j_blocks;

	j_blocks = ext2fs_default_journal_size(ext2fs_blocks_count(fs->super));
	if (j_blocks < 0) {
		fputs(_("\nFilesystem too small for a journal\n"), stderr);
		return 0;
	}

	if (size > 0) {
		j_blocks = size * 1024 / (fs->blocksize	/ 1024);
		if (j_blocks < 1024 || j_blocks > 10240000) {
			fprintf(stderr, _("\nThe requested journal "
				"size is %d blocks; it must be\n"
				"between 1024 and 10240000 blocks.  "
				"Aborting.\n"),
				j_blocks);
			exit(1);
		}
		if ((unsigned) j_blocks > ext2fs_free_blocks_count(fs->super) / 2) {
			fputs(_("\nJournal size too big for filesystem.\n"),
			      stderr);
			exit(1);
		}
	}
	return j_blocks;
}

void print_check_message(int mnt, unsigned int check)
{
	if (mnt < 0)
		mnt = 0;
	if (!mnt && !check)
		return;
	printf(_("This filesystem will be automatically "
		 "checked every %d mounts or\n"
		 "%g days, whichever comes first.  "
		 "Use tune2fs -c or -i to override.\n"),
	       mnt, ((double) check) / (3600 * 24));
}

void dump_mmp_msg(struct mmp_struct *mmp, const char *msg)
{

	if (msg)
		printf("MMP check failed: %s\n", msg);
	if (mmp) {
		time_t t = mmp->mmp_time;

		printf("MMP error info: last update: %s node: %s device: %s\n",
		       ctime(&t), mmp->mmp_nodename, mmp->mmp_bdevname);
	}
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 355,1;355,7

