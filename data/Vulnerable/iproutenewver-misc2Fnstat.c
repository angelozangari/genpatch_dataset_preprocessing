/*
 * nstat.c	handy utility to read counters /proc/net/netstat and snmp
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <fnmatch.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>
#include <getopt.h>

#include <json_writer.h>
#include <SNAPSHOT.h>
#include "utils.h"

int dump_zeros;
int reset_history;
int ignore_history;
int no_output;
int json_output;
int pretty;
int no_update;
int scan_interval;
int time_constant;
double W;
char **patterns;
int npatterns;

char info_source[128];
int source_mismatch;

static int generic_proc_open(const char *env, char *name)
{
	char store[128];
	char *p = getenv(env);

	if (!p) {
		p = getenv("PROC_ROOT") ? : "/proc";
		snprintf(store, sizeof(store)-1, "%s/%s", p, name);
		p = store;
	}
	return open(p, O_RDONLY);
}

static int net_netstat_open(void)
{
	return generic_proc_open("PROC_NET_NETSTAT", "net/netstat");
}

static int net_snmp_open(void)
{
	return generic_proc_open("PROC_NET_SNMP", "net/snmp");
}

static int net_snmp6_open(void)
{
	return generic_proc_open("PROC_NET_SNMP6", "net/snmp6");
}

static int net_sctp_snmp_open(void)
{
	return generic_proc_open("PROC_NET_SCTP_SNMP", "net/sctp/snmp");
}

struct nstat_ent {
	struct nstat_ent *next;
	char		 *id;
	unsigned long long val;
	double		   rate;
};

struct nstat_ent *kern_db;
struct nstat_ent *hist_db;

static const char *useless_numbers[] = {
	"IpForwarding", "IpDefaultTTL",
	"TcpRtoAlgorithm", "TcpRtoMin", "TcpRtoMax",
	"TcpMaxConn", "TcpCurrEstab"
};

static int useless_number(const char *id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(useless_numbers); i++)
		if (strcmp(id, useless_numbers[i]) == 0)
			return 1;
	return 0;
}

static int match(const char *id)
{
	int i;

	if (npatterns == 0)
		return 1;

	for (i = 0; i < npatterns; i++) {
		if (!fnmatch(patterns[i], id, 0))
			return 1;
	}
	return 0;
}

static void load_good_table(FILE *fp)
{
	char buf[4096];
	struct nstat_ent *db = NULL;
	struct nstat_ent *n;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		int nr;
		unsigned long long val;
		double rate;
		char idbuf[sizeof(buf)];

		if (buf[0] == '#') {
			buf[strlen(buf)-1] = 0;
			if (info_source[0] && strcmp(info_source, buf+1))
				source_mismatch = 1;
			info_source[0] = 0;
			strncat(info_source, buf+1, sizeof(info_source)-1);
			continue;
		}
		/* idbuf is as big as buf, so this is safe */
		nr = sscanf(buf, "%s%llu%lg", idbuf, &val, &rate);
		if (nr < 2)
			abort();
		if (nr < 3)
			rate = 0;
		if (useless_number(idbuf))
			continue;
		if ((n = malloc(sizeof(*n))) == NULL)
			abort();
		n->id = strdup(idbuf);
		n->val = val;
		n->rate = rate;
		n->next = db;
		db = n;
	}

	while (db) {
		n = db;
		db = db->next;
		n->next = kern_db;
		kern_db = n;
	}
}

static int count_spaces(const char *line)
{
	int count = 0;
	char c;

	while ((c = *line++) != 0)
		count += c == ' ' || c == '\n';
	return count;
}

static void load_ugly_table(FILE *fp)
{
	char buf[4096];
	struct nstat_ent *db = NULL;
	struct nstat_ent *n;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		char idbuf[sizeof(buf)];
		int  off;
		char *p;
		int count1, count2, skip = 0;

		p = strchr(buf, ':');
		if (!p)
			abort();
		count1 = count_spaces(buf);
		*p = 0;
		idbuf[0] = 0;
		strncat(idbuf, buf, sizeof(idbuf) - 1);
		off = p - buf;
		p += 2;

		while (*p) {
			char *next;

			if ((next = strchr(p, ' ')) != NULL)
				*next++ = 0;
			else if ((next = strchr(p, '\n')) != NULL)
				*next++ = 0;
			if (off < sizeof(idbuf)) {
				idbuf[off] = 0;
				strncat(idbuf, p, sizeof(idbuf) - off - 1);
			}
			n = malloc(sizeof(*n));
			if (!n)
				abort();
			n->id = strdup(idbuf);
			n->rate = 0;
			n->next = db;
			db = n;
			p = next;
		}
		n = db;
		if (fgets(buf, sizeof(buf), fp) == NULL)
			abort();
		count2 = count_spaces(buf);
		if (count2 > count1)
			skip = count2 - count1;
		do {
			p = strrchr(buf, ' ');
			if (!p)
				abort();
			*p = 0;
			if (sscanf(p+1, "%llu", &n->val) != 1)
				abort();
			/* Trick to skip "dummy" trailing ICMP MIB in 2.4 */
			if (skip)
				skip--;
			else
				n = n->next;
		} while (p > buf + off + 2);
	}

	while (db) {
		n = db;
		db = db->next;
		if (useless_number(n->id)) {
			free(n->id);
			free(n);
		} else {
			n->next = kern_db;
			kern_db = n;
		}
	}
}

static void load_sctp_snmp(void)
{
	FILE *fp = fdopen(net_sctp_snmp_open(), "r");

	if (fp) {
		load_good_table(fp);
		fclose(fp);
	}
}

static void load_snmp(void)
{
	FILE *fp = fdopen(net_snmp_open(), "r");

	if (fp) {
		load_ugly_table(fp);
		fclose(fp);
	}
}

static void load_snmp6(void)
{
	FILE *fp = fdopen(net_snmp6_open(), "r");

	if (fp) {
		load_good_table(fp);
		fclose(fp);
	}
}

static void load_netstat(void)
{
	FILE *fp = fdopen(net_netstat_open(), "r");

	if (fp) {
		load_ugly_table(fp);
		fclose(fp);
	}
}


static void dump_kern_db(FILE *fp, int to_hist)
{
	json_writer_t *jw = json_output ? jsonw_new(fp) : NULL;
	struct nstat_ent *n, *h;

	h = hist_db;
	if (jw) {
		jsonw_start_object(jw);
		jsonw_pretty(jw, pretty);
		jsonw_name(jw, info_source);
		jsonw_start_object(jw);
	} else
		fprintf(fp, "#%s\n", info_source);

	for (n = kern_db; n; n = n->next) {
		unsigned long long val = n->val;

		if (!dump_zeros && !val && !n->rate)
			continue;
		if (!match(n->id)) {
			struct nstat_ent *h1;

			if (!to_hist)
				continue;
			for (h1 = h; h1; h1 = h1->next) {
				if (strcmp(h1->id, n->id) == 0) {
					val = h1->val;
					h = h1->next;
					break;
				}
			}
		}

		if (jw)
			jsonw_uint_field(jw, n->id, val);
		else
			fprintf(fp, "%-32s%-16llu%6.1f\n", n->id, val, n->rate);
	}

	if (jw) {
		jsonw_end_object(jw);

		jsonw_end_object(jw);
		jsonw_destroy(&jw);
	}
}

static void dump_incr_db(FILE *fp)
{
	json_writer_t *jw = json_output ? jsonw_new(fp) : NULL;
	struct nstat_ent *n, *h;

	h = hist_db;
	if (jw) {
		jsonw_start_object(jw);
		jsonw_pretty(jw, pretty);
		jsonw_name(jw, info_source);
		jsonw_start_object(jw);
	} else
		fprintf(fp, "#%s\n", info_source);

	for (n = kern_db; n; n = n->next) {
		int ovfl = 0;
		unsigned long long val = n->val;
		struct nstat_ent *h1;

		for (h1 = h; h1; h1 = h1->next) {
			if (strcmp(h1->id, n->id) == 0) {
				if (val < h1->val) {
					ovfl = 1;
					val = h1->val;
				}
				val -= h1->val;
				h = h1->next;
				break;
			}
		}
		if (!dump_zeros && !val && !n->rate)
			continue;
		if (!match(n->id))
			continue;

		if (jw)
			jsonw_uint_field(jw, n->id, val);
		else
			fprintf(fp, "%-32s%-16llu%6.1f%s\n", n->id, val,
				n->rate, ovfl?" (overflow)":"");
	}

	if (jw) {
		jsonw_end_object(jw);

		jsonw_end_object(jw);
		jsonw_destroy(&jw);
	}
}

static int children;

static void sigchild(int signo)
{
}

static void update_db(int interval)
{
	struct nstat_ent *n, *h;

	n = kern_db;
	kern_db = NULL;

	load_netstat();
	load_snmp6();
	load_snmp();
	load_sctp_snmp();

	h = kern_db;
	kern_db = n;

	for (n = kern_db; n; n = n->next) {
		struct nstat_ent *h1;

		for (h1 = h; h1; h1 = h1->next) {
			if (strcmp(h1->id, n->id) == 0) {
				double sample;
				unsigned long long incr = h1->val - n->val;

				n->val = h1->val;
				sample = (double)incr * 1000.0 / interval;
				if (interval >= scan_interval) {
					n->rate += W*(sample-n->rate);
				} else if (interval >= 1000) {
					if (interval >= time_constant) {
						n->rate = sample;
					} else {
						double w = W*(double)interval/scan_interval;

						n->rate += w*(sample-n->rate);
					}
				}

				while (h != h1) {
					struct nstat_ent *tmp = h;

					h = h->next;
					free(tmp->id);
					free(tmp);
				};
				h = h1->next;
				free(h1->id);
				free(h1);
				break;
			}
		}
	}
}

#define T_DIFF(a, b) (((a).tv_sec-(b).tv_sec)*1000 + ((a).tv_usec-(b).tv_usec)/1000)


static void server_loop(int fd)
{
	struct timeval snaptime = { 0 };
	struct pollfd p;

	p.fd = fd;
	p.events = p.revents = POLLIN;

	sprintf(info_source, "%d.%lu sampling_interval=%d time_const=%d",
		getpid(), (unsigned long)random(), scan_interval/1000, time_constant/1000);

	load_netstat();
	load_snmp6();
	load_snmp();
	load_sctp_snmp();

	for (;;) {
		int status;
		time_t tdiff;
		struct timeval now;

		gettimeofday(&now, NULL);
		tdiff = T_DIFF(now, snaptime);
		if (tdiff >= scan_interval) {
			update_db(tdiff);
			snaptime = now;
			tdiff = 0;
		}
		if (poll(&p, 1, scan_interval - tdiff) > 0
		    && (p.revents&POLLIN)) {
			int clnt = accept(fd, NULL, NULL);

			if (clnt >= 0) {
				pid_t pid;

				if (children >= 5) {
					close(clnt);
				} else if ((pid = fork()) != 0) {
					if (pid > 0)
						children++;
					close(clnt);
				} else {
					FILE *fp = fdopen(clnt, "w");

					if (fp)
						dump_kern_db(fp, 0);
					exit(0);
				}
			}
		}
		while (children && waitpid(-1, &status, WNOHANG) > 0)
			children--;
	}
}

static int verify_forging(int fd)
{
	struct ucred cred;
	socklen_t olen = sizeof(cred);

	if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, (void *)&cred, &olen) ||
	    olen < sizeof(cred))
		return -1;
	if (cred.uid == getuid() || cred.uid == 0)
		return 0;
	return -1;
}

static void usage(void) __attribute__((noreturn));

static void usage(void)
{
	fprintf(stderr,
"Usage: nstat [OPTION] [ PATTERN [ PATTERN ] ]\n"
"   -h, --help           this message\n"
"   -a, --ignore         ignore history\n"
"   -d, --scan=SECS      sample every statistics every SECS\n"
"   -j, --json           format output in JSON\n"
"   -n, --nooutput       do history only\n"
"   -p, --pretty         pretty print\n"
"   -r, --reset          reset history\n"
"   -s, --noupdate       don't update history\n"
"   -t, --interval=SECS  report average over the last SECS\n"
"   -V, --version        output version information\n"
"   -z, --zeros          show entries with zero activity\n");
	exit(-1);
}

static const struct option longopts[] = {
	{ "help", 0, 0, 'h' },
	{ "ignore",  0,  0, 'a' },
	{ "scan", 1, 0, 'd'},
	{ "nooutput", 0, 0, 'n' },
	{ "json", 0, 0, 'j' },
	{ "reset", 0, 0, 'r' },
	{ "noupdate", 0, 0, 's' },
	{ "pretty", 0, 0, 'p' },
	{ "interval", 1, 0, 't' },
	{ "version", 0, 0, 'V' },
	{ "zeros", 0, 0, 'z' },
	{ 0 }
};

int main(int argc, char *argv[])
{
	char *hist_name;
	struct sockaddr_un sun;
	FILE *hist_fp = NULL;
	int ch;
	int fd;

	while ((ch = getopt_long(argc, argv, "h?vVzrnasd:t:jp",
				 longopts, NULL)) != EOF) {
		switch (ch) {
		case 'z':
			dump_zeros = 1;
			break;
		case 'r':
			reset_history = 1;
			break;
		case 'a':
			ignore_history = 1;
			break;
		case 's':
			no_update = 1;
			break;
		case 'n':
			no_output = 1;
			break;
		case 'd':
			scan_interval = 1000*atoi(optarg);
			break;
		case 't':
			if (sscanf(optarg, "%d", &time_constant) != 1 ||
			    time_constant <= 0) {
				fprintf(stderr, "nstat: invalid time constant divisor\n");
				exit(-1);
			}
			break;
		case 'j':
			json_output = 1;
			break;
		case 'p':
			pretty = 1;
			break;
		case 'v':
		case 'V':
			printf("nstat utility, iproute2-ss%s\n", SNAPSHOT);
			exit(0);
		case 'h':
		case '?':
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	sun.sun_family = AF_UNIX;
	sun.sun_path[0] = 0;
	sprintf(sun.sun_path+1, "nstat%d", getuid());

	if (scan_interval > 0) {
		if (time_constant == 0)
			time_constant = 60;
		time_constant *= 1000;
		W = 1 - 1/exp(log(10)*(double)scan_interval/time_constant);
		if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
			perror("nstat: socket");
			exit(-1);
		}
		if (bind(fd, (struct sockaddr *)&sun, 2+1+strlen(sun.sun_path+1)) < 0) {
			perror("nstat: bind");
			exit(-1);
		}
		if (listen(fd, 5) < 0) {
			perror("nstat: listen");
			exit(-1);
		}
		if (daemon(0, 0)) {
			perror("nstat: daemon");
			exit(-1);
		}
		signal(SIGPIPE, SIG_IGN);
		signal(SIGCHLD, sigchild);
		server_loop(fd);
		exit(0);
	}

	patterns = argv;
	npatterns = argc;

	if ((hist_name = getenv("NSTAT_HISTORY")) == NULL) {
		hist_name = malloc(128);
		sprintf(hist_name, "/tmp/.nstat.u%d", getuid());
	}

	if (reset_history)
		unlink(hist_name);

	if (!ignore_history || !no_update) {
		struct stat stb;

		fd = open(hist_name, O_RDWR|O_CREAT|O_NOFOLLOW, 0600);
		if (fd < 0) {
			perror("nstat: open history file");
			exit(-1);
		}
		if ((hist_fp = fdopen(fd, "r+")) == NULL) {
			perror("nstat: fdopen history file");
			exit(-1);
		}
		if (flock(fileno(hist_fp), LOCK_EX)) {
			perror("nstat: flock history file");
			exit(-1);
		}
		if (fstat(fileno(hist_fp), &stb) != 0) {
			perror("nstat: fstat history file");
			exit(-1);
		}
		if (stb.st_nlink != 1 || stb.st_uid != getuid()) {
			fprintf(stderr, "nstat: something is so wrong with history file, that I prefer not to proceed.\n");
			exit(-1);
		}
		if (!ignore_history) {
			FILE *tfp;
			long uptime = -1;

			if ((tfp = fopen("/proc/uptime", "r")) != NULL) {
				if (fscanf(tfp, "%ld", &uptime) != 1)
					uptime = -1;
				fclose(tfp);
			}
			if (uptime >= 0 && time(NULL) >= stb.st_mtime+uptime) {
				fprintf(stderr, "nstat: history is aged out, resetting\n");
				if (ftruncate(fileno(hist_fp), 0) < 0)
					perror("nstat: ftruncate");
			}
		}

		load_good_table(hist_fp);

		hist_db = kern_db;
		kern_db = NULL;
	}

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0 &&
	    (connect(fd, (struct sockaddr *)&sun, 2+1+strlen(sun.sun_path+1)) == 0
	     || (strcpy(sun.sun_path+1, "nstat0"),
		 connect(fd, (struct sockaddr *)&sun, 2+1+strlen(sun.sun_path+1)) == 0))
	    && verify_forging(fd) == 0) {
		FILE *sfp = fdopen(fd, "r");

		if (!sfp) {
			fprintf(stderr, "nstat: fdopen failed: %s\n",
				strerror(errno));
			close(fd);
		} else {
			load_good_table(sfp);
			if (hist_db && source_mismatch) {
				fprintf(stderr, "nstat: history is stale, ignoring it.\n");
				hist_db = NULL;
			}
			fclose(sfp);
		}
	} else {
		if (fd >= 0)
			close(fd);
		if (hist_db && info_source[0] && strcmp(info_source, "kernel")) {
			fprintf(stderr, "nstat: history is stale, ignoring it.\n");
			hist_db = NULL;
			info_source[0] = 0;
		}
		load_netstat();
		load_snmp6();
		load_snmp();
		load_sctp_snmp();
		if (info_source[0] == 0)
			strcpy(info_source, "kernel");
	}

	if (!no_output) {
		if (ignore_history || hist_db == NULL)
			dump_kern_db(stdout, 0);
		else
			dump_incr_db(stdout);
	}
	if (!no_update) {
		if (ftruncate(fileno(hist_fp), 0) < 0)
			perror("nstat: ftruncate");
		rewind(hist_fp);

		json_output = 0;
		dump_kern_db(hist_fp, 1);
		fclose(hist_fp);
	}
	exit(0);
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 463,1;463,8

// 616,1;616,8

// 650,2;650,9

// 704,10;704,16

// 734,3;734,9

