/*
 * bootlogd.c	Store output from the console during bootup into a file.
 *		The file is usually located on the /var partition, and
 *		gets written (and fsynced) as soon as possible.
 *
 * Version:	@(#)bootlogd  2.86pre  12-Jan-2004  miquels@cistron.nl
 *
 * Bugs:	Uses openpty(), only available in glibc. Sorry.
 *
 *		This file is part of the sysvinit suite,
 *		Copyright (C) 1991-2004 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation; either version 2 of the License, or
 *		(at your option) any later version.
 *
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details.
 *
 *		You should have received a copy of the GNU General Public License
 *		along with this program; if not, write to the Free Software
 *		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *				*NOTE* *NOTE* *NOTE*
 *			This is a PROOF OF CONCEPT IMPLEMENTATION
 *
 *		I have bigger plans for Debian, but for now
 *		this has to do ;)
 *
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>
#include <pty.h>
#include <ctype.h>
#ifdef __linux__
#include <sys/mount.h>
#endif

char *Version = "@(#) bootlogd 2.86 03-Jun-2004 miquels@cistron.nl";

#define LOGFILE	"/var/log/boot"
#define MAX_CONSOLES 16

char ringbuf[32768];
char *endptr = ringbuf + sizeof(ringbuf);
char *inptr  = ringbuf;
char *outptr = ringbuf;

int got_signal = 0;
int didnl = 1;
int createlogfile = 0;
int syncalot = 0;

struct line {
	char buf[256];
	int pos;
} line;

struct real_cons {
	char name[1024];
	int fd;
};

/*
 *	Console devices as listed on the kernel command line and
 *	the mapping to actual devices in /dev
 */
struct consdev {
	char	*cmdline;
	char	*dev1;
	char	*dev2;
} consdev[] = {
	{ "ttyB",	"/dev/ttyB%s",		NULL		},
	{ "ttySC",	"/dev/ttySC%s",		"/dev/ttsc/%s"	},
	{ "ttyS",	"/dev/ttyS%s",		"/dev/tts/%s"	},
	{ "tty",	"/dev/tty%s",		"/dev/vc/%s"	},
	{ "hvc",	"/dev/hvc%s",		"/dev/hvc/%s"	},
	{ NULL,		NULL,			NULL		},
};

/*
 *	Devices to try as console if not found on kernel command line.
 *	Tried from left to right (as opposed to kernel cmdline).
 */
char *defcons[] = { "tty0", "hvc0", "ttyS0", "ttySC0", "ttyB0", NULL };

/*
 *	Catch signals.
 */
void handler(int sig)
{
	got_signal = sig;
}


/*
 *	Scan /dev and find the device name.
 */
static int findtty(char *res, const char *startdir, int rlen, dev_t dev)
{
	DIR		*dir;
	struct dirent	*ent;
	struct stat	st;
	int		r = -1;
	char *olddir = getcwd(NULL, 0);

	if (chdir(startdir) < 0 || (dir = opendir(".")) == NULL) {
		int msglen = strlen(startdir) + 11;
		char *msg = malloc(msglen);
		snprintf(msg, msglen, "bootlogd: %s", startdir);
		perror(msg);
		free(msg);
		chdir(olddir);
		return -1;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (lstat(ent->d_name, &st) != 0)
			continue;
		if (S_ISDIR(st.st_mode)
		    && 0 != strcmp(".", ent->d_name)
		    && 0 != strcmp("..", ent->d_name)) {
			char *path = malloc(rlen);
			snprintf(path, rlen, "%s/%s", startdir, ent->d_name);
			r = findtty(res, path, rlen, dev);
			free(path);
			if (0 == r) { /* device found, return */
				closedir(dir);
				chdir(olddir);
				return 0;
			}
			continue;
		}
		if (!S_ISCHR(st.st_mode))
			continue;
		if (st.st_rdev == dev) {
			if ( (int) (strlen(ent->d_name) + strlen(startdir) + 1) >= rlen) {
				fprintf(stderr, "bootlogd: console device name too long\n");
				closedir(dir);
				chdir(olddir);
				return -1;
			} else {
				snprintf(res, rlen, "%s/%s", startdir, ent->d_name);
				closedir(dir);
				chdir(olddir);
				return 0;
			}
		}
	}
	closedir(dir);

	chdir(olddir);
	return r;
}

/*
 *	For some reason, openpty() in glibc sometimes doesn't
 *	work at boot-time. It must be a bug with old-style pty
 *	names, as new-style (/dev/pts) is not available at that
 *	point. So, we find a pty/tty pair ourself if openpty()
 *	fails for whatever reason.
 */
int findpty(int *master, int *slave, char *name)
{
	char	pty[16];
	char	tty[16];
	int	i, j;
	int	found;

	if (openpty(master, slave, name, NULL, NULL) >= 0)
		return 0;

	found = 0;

	for (i = 'p'; i <= 'z'; i++) {
		for (j = '0'; j <= 'f'; j++) {
			if (j == '9' + 1) j = 'a';
			sprintf(pty, "/dev/pty%c%c", i, j);
			sprintf(tty, "/dev/tty%c%c", i, j);
			if ((*master = open(pty, O_RDWR|O_NOCTTY)) >= 0) {
				*slave = open(tty, O_RDWR|O_NOCTTY);
				if (*slave >= 0) {
					found = 1;
					break;
				}
			}
		}
		if (found) break;
	}
	if (!found) return -1;

	if (name) strcpy(name, tty);

	return 0;
}
/*
 *	See if a console taken from the kernel command line maps
 *	to a character device we know about, and if we can open it.
 */
int isconsole(char *s, char *res, int rlen)
{
	struct consdev	*c;
	int		l, sl, i, fd;
	char		*p, *q;

	sl = strlen(s);

	for (c = consdev; c->cmdline; c++) {
		l = strlen(c->cmdline);
		if (sl <= l) continue;
		p = s + l;
		if (strncmp(s, c->cmdline, l) != 0 || !isdigit(*p))
			continue;
		for (i = 0; i < 2; i++) {
			snprintf(res, rlen, i ? c->dev1 : c->dev2, p);
			if ((q = strchr(res, ',')) != NULL) *q = 0;
			if ((fd = open(res, O_RDONLY|O_NONBLOCK)) >= 0) {
				close(fd);
				return 1;
			}
		}
	}
	return 0;
}

/*
 *	Find out the _real_ console(s). Assume that stdin is connected to
 *	the console device (/dev/console).
 */
int consolenames(struct real_cons *cons, int max_consoles)
{
#ifdef TIOCGDEV
	unsigned int	kdev;
#endif
	struct stat	st, st2;
	char		buf[256];
	char		*p;
	int		didmount = 0;
	int		n;
	int		fd;
	int		considx, num_consoles = 0;

#ifdef __linux__
	/*
	 *	Read /proc/cmdline.
	 */
	stat("/", &st);
	if (stat("/proc", &st2) < 0) {
		perror("bootlogd: /proc");
		return 0;
	}
	if (st.st_dev == st2.st_dev) {
		if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
			perror("bootlogd: mount /proc");
			return -1;
		}
		didmount = 1;
	}

	n = -1;
	if ((fd = open("/proc/cmdline", O_RDONLY)) < 0) {
		perror("bootlogd: /proc/cmdline");
	} else {
		buf[0] = 0;
		if ((n = read(fd, buf, sizeof(buf) - 1)) < 0)
			perror("bootlogd: /proc/cmdline");
		close(fd);
	}
	if (didmount) umount("/proc");
                

	if (n < 0) return 0;



	/*
	 *	OK, so find console= in /proc/cmdline.
	 *	Parse in reverse, opening as we go.
	 */
	p = buf + n;
	*p-- = 0;
	while (p >= buf) {
		if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
			*p-- = 0;
			continue;
		}
		if (strncmp(p, "console=", 8) == 0 &&
			isconsole(p + 8, cons[num_consoles].name, sizeof(cons[num_consoles].name))) {
				/*
				 *	Suppress duplicates
				 */
				for (considx = 0; considx < num_consoles; considx++) {
					if (!strcmp(cons[num_consoles].name, cons[considx].name)) {
						goto dontuse;
					}
				}
			
			num_consoles++;
			if (num_consoles >= max_consoles) {
				break;
			}
		}
dontuse:
		p--;
	}

	if (num_consoles > 0) return num_consoles;
#endif

	/*
	 *	Okay, no console on the command line -
	 *	guess the default console.
	 */
	for (n = 0; defcons[n]; n++)
		if (isconsole(defcons[n], cons[0].name, sizeof(cons[0].name))) 
			return 1;

	fprintf(stderr, "bootlogd: cannot deduce real console device\n");

	return 0;
}


/*
 *	Write data and make sure it's on disk.
 */
void writelog(FILE *fp, unsigned char *ptr, int len)
{
	time_t		t;
	char		*s;
	char		tmp[8];
	int		olen = len;
	int		dosync = 0;
	int		tlen;

	while (len > 0) {
		tmp[0] = 0;
		if (didnl) {
			time(&t);
			s = ctime(&t);
			fprintf(fp, "%.24s: ", s);
			didnl = 0;
		}
		switch (*ptr) {
			case 27: /* ESC */
				strcpy(tmp, "^[");
				break;
			case '\r':
				line.pos = 0;
				break;
			case 8: /* ^H */
				if (line.pos > 0) line.pos--;
				break;
			case '\n':
				didnl = 1;
				dosync = 1;
				break;
			case '\t':
				line.pos += (line.pos / 8 + 1) * 8;
				if (line.pos >= (int)sizeof(line.buf))
					line.pos = sizeof(line.buf) - 1;
				break;
			case  32 ... 127:
			case 161 ... 255:
				tmp[0] = *ptr;
				tmp[1] = 0;
				break;
			default:
				sprintf(tmp, "\\%03o", *ptr);
				break;
		}
		ptr++;
		len--;

		tlen = strlen(tmp);
		if (tlen && (line.pos + tlen < (int)sizeof(line.buf))) {
			memcpy(line.buf + line.pos, tmp, tlen);
			line.pos += tlen;
		}
		if (didnl) {
			fprintf(fp, "%s\n", line.buf);
			memset(&line, 0, sizeof(line));
		}
	}

	if (dosync) {
		fflush(fp);
		if (syncalot) {
			fdatasync(fileno(fp));
		}
	}

	outptr += olen;
	if (outptr >= endptr)
		outptr = ringbuf;

}


/*
 *	Print usage message and exit.
 */
void usage(void)
{
	fprintf(stderr, "Usage: bootlogd [-v] [-r] [-d] [-s] [-c] [-p pidfile] [-l logfile]\n");
	exit(1);
}

int open_nb(char *buf)
{
	int	fd, n;

	if ((fd = open(buf, O_WRONLY|O_NONBLOCK|O_NOCTTY)) < 0)
		return -1;
	n = fcntl(fd, F_GETFL);
	n &= ~(O_NONBLOCK);
	fcntl(fd, F_SETFL, n);

	return fd;
}

/*
 *	We got a write error on the real console. If its an EIO,
 *	somebody hung up our filedescriptor, so try to re-open it.
 */
int write_err(int pts, int realfd, char *realcons, int e)
{
	int	fd;

	if (e != EIO) {
werr:
		close(pts);
		fprintf(stderr, "bootlogd: writing to console: %s\n",
			strerror(e));
		return -1;
	}
	close(realfd);
	if ((fd = open_nb(realcons)) < 0)
		goto werr;

	return fd;
}

int main(int argc, char **argv)
{
	FILE		*fp;
	struct timeval	tv;
	fd_set		fds;
	char		buf[1024];
	char		*p;
	char		*logfile;
	char		*pidfile;
	int		rotate;
	int		dontfork;
	int		ptm, pts;
	int		realfd;
	int		n, m, i;
	int		todo;
#ifndef __linux__	/* BSD-style ioctl needs an argument. */
	int		on = 1;
#endif
	int		considx;
	struct real_cons cons[MAX_CONSOLES];
	int		num_consoles, consoles_left;

	fp = NULL;
	logfile = LOGFILE;
	pidfile = NULL;
	rotate = 0;
	dontfork = 0;

	while ((i = getopt(argc, argv, "cdsl:p:rv")) != EOF) switch(i) {
		case 'l':
			logfile = optarg;
			break;
		case 'r':
			rotate = 1;
			break;
		case 'v':
			printf("%s\n", Version);
			exit(0);
			break;
		case 'p':
			pidfile = optarg;
			break;
		case 'c':
			createlogfile = 1;
			break;
		case 'd':
			dontfork = 1;
			break;
		case 's':
			syncalot = 1;
			break;
		default:
			usage();
			break;
	}
	if (optind < argc) usage();

	signal(SIGTERM, handler);
	signal(SIGQUIT, handler);
	signal(SIGINT,  handler);
	signal(SIGTTIN,  SIG_IGN);
	signal(SIGTTOU,  SIG_IGN);
	signal(SIGTSTP,  SIG_IGN);

	/*
	 *	Open console device directly.
	 */
	if ((num_consoles = consolenames(cons, MAX_CONSOLES)) <= 0)
		return 1;
	consoles_left = num_consoles;
	for (considx = 0; considx < num_consoles; considx++) {
		if (strcmp(cons[considx].name, "/dev/tty0") == 0)
			strcpy(cons[considx].name, "/dev/tty1");
		if (strcmp(cons[considx].name, "/dev/vc/0") == 0)
			strcpy(cons[considx].name, "/dev/vc/1");

		if ((cons[considx].fd = open_nb(cons[considx].name)) < 0) {
			fprintf(stderr, "bootlogd: %s: %s\n", cons[considx].name, strerror(errno));
			consoles_left--;
		}
	}
	if (!consoles_left)
		return 1;

	/*
	 *	Grab a pty, and redirect console messages to it.
	 */
	ptm = -1;
	pts = -1;
	buf[0] = 0;
	if (findpty(&ptm, &pts, buf) < 0) {
		fprintf(stderr,
			"bootlogd: cannot allocate pseudo tty: %s\n",
			strerror(errno));
		return 1;
	}

#ifdef __linux__
	(void)ioctl(0, TIOCCONS, NULL);
	/* Work around bug in 2.1/2.2 kernels. Fixed in 2.2.13 and 2.3.18 */
	if ((n = open("/dev/tty0", O_RDWR)) >= 0) {
		(void)ioctl(n, TIOCCONS, NULL);
		close(n);
	}
#endif
#ifdef __linux__
	if (ioctl(pts, TIOCCONS, NULL) < 0)
#else	/* BSD usage of ioctl TIOCCONS. */
	if (ioctl(pts, TIOCCONS, &on) < 0)
#endif
	{
		fprintf(stderr, "bootlogd: ioctl(%s, TIOCCONS): %s\n",
			buf, strerror(errno));
		return 1;
	}

	/*
	 *	Fork and write pidfile if needed.
	 */
	if (!dontfork) {
		pid_t child_pid = fork();
		switch (child_pid) {
		case -1: /* I am parent and the attempt to create a child failed */
			fprintf(stderr, "bootlogd: fork failed: %s\n",
				strerror(errno));
			exit(1);
			break;
		case 0: /* I am the child */
			break;
		default: /* I am parent and got child's pid */
			exit(0);
			break;
		}
		setsid();
	}
	if (pidfile) {
		unlink(pidfile);
		if ((fp = fopen(pidfile, "w")) != NULL) {
			fprintf(fp, "%d\n", (int)getpid());
			fclose(fp);
		}
		fp = NULL;
	}

	/*
	 *	Read the console messages from the pty, and write
	 *	to the real console and the logfile.
	 */
	while (!got_signal) {

		/*
		 *	We timeout after 5 seconds if we still need to
		 *	open the logfile. There might be buffered messages
		 *	we want to write.
		 */
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		FD_ZERO(&fds);
		FD_SET(ptm, &fds);
		if (select(ptm + 1, &fds, NULL, NULL, &tv) == 1) {
			/*
			 *	See how much space there is left, read.
			 */
			if ((n = read(ptm, inptr, endptr - inptr)) >= 0) {
				/*
				 *	Write data (in chunks if needed)
				 *	to the real output devices.
				 */
				for (considx = 0; considx < num_consoles; considx++) {
					if (cons[considx].fd < 0) continue;
					m = n;
					p = inptr;
					while (m > 0) {
						i = write(cons[considx].fd, p, m);
						if (i >= 0) {
							m -= i;
							p += i;
							continue;
						}
						/*
						 *	Handle EIO (somebody hung
						 *	up our filedescriptor)
						 */
						cons[considx].fd = write_err(pts,
							cons[considx].fd,
							cons[considx].name, errno);
						if (cons[considx].fd >= 0) continue;
						/*	
						 *	If this was the last console,
						 *	generate a fake signal
						 */
						if (--consoles_left <= 0) got_signal = 1;
						break;
 					}
				}

				/*
				 *	Increment buffer position. Handle
				 *	wraps, and also drag output pointer
				 *	along if we cross it.
				 */
				inptr += n;
				if (inptr - n < outptr && inptr > outptr)
					outptr = inptr;
				if (inptr >= endptr)
					inptr = ringbuf;
				if (outptr >= endptr)
					outptr = ringbuf;
			}
		}

		/*
		 *	Perhaps we need to open the logfile.
		 */
		if (fp == NULL && access(logfile, F_OK) == 0) {
			if (rotate) {
				snprintf(buf, sizeof(buf), "%s~", logfile);
				rename(logfile, buf);
			}
			fp = fopen(logfile, "a");
		}
		if (fp == NULL && createlogfile)
			fp = fopen(logfile, "a");

		if (inptr >= outptr)
			todo = inptr - outptr;
		else
			todo = endptr - outptr;
		if (fp && todo)
			writelog(fp, (unsigned char *)outptr, todo);
	}

	if (fp) {
		if (!didnl) fputc('\n', fp);
		fclose(fp);
	}

	close(pts);
	close(ptm);
	for (considx = 0; considx < num_consoles; considx++) {
		close(cons[considx].fd);
	}

	return 0;
}


//						↓↓↓VULNERABLE LINES↓↓↓

// 195,3;195,10

// 196,3;196,10

// 209,11;209,17

// 363,4;363,10

// 386,4;386,11

// 533,3;533,9

// 535,3;535,9

