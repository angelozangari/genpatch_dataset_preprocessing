/*
 * $Id: clientid.c,v 1.2 1997/12/25 23:28:09 lf Exp $
 *
 * Copyright (C) 1995,1996,1997 Lars Fenneberg
 *
 * See the file COPYRIGHT for the respective terms and conditions. 
 * If the file is missing contact me at lf@elemental.net 
 * and I'll send you a copy.
 *
 */

#include <config.h>
#include <includes.h>
#include <radiusclient.h>

struct map2id_s {
	char *name;
	UINT4 id;

	struct map2id_s *next;
};

static struct map2id_s *map2id_list = NULL; 

/*
 * Function: rc_read_mapfile
 *
 * Purpose: Read in the ttyname to port id map file
 *
 * Arguments: the file name of the map file
 *
 * Returns: zero on success, negative integer on failure 
 */

int rc_read_mapfile(char *filename)
{
	char buffer[1024];
	FILE *mapfd;
	char *c, *name, *id, *q;
	struct map2id_s *p;
	int lnr = 0;

        if ((mapfd = fopen(filename,"r")) == NULL)
        {
		rc_log(LOG_ERR,"rc_read_mapfile: can't read %s: %s", filename, strerror(errno));                                                        
		return (-1);
	}
	
#define SKIP(p) while(*p && isspace(*p)) p++;

        while (fgets(buffer, sizeof(buffer), mapfd) != NULL)
        {
        	lnr++;
        
		q = buffer;
		
                SKIP(q);
                                                
                if ((*q == '\n') || (*q == '#') || (*q == '\0'))
			continue;

		if (( c = strchr(q, ' ')) || (c = strchr(q,'\t'))) {

			*c = '\0'; c++;
			SKIP(c);
			
			name = q;
			id = c;
			
			if ((p = (struct map2id_s *)malloc(sizeof(*p))) == NULL) {
				rc_log(LOG_CRIT,"rc_read_mapfile: out of memory");
				return (-1);
			}
			
			p->name = strdup(name);
			p->id = atoi(id);
			p->next = map2id_list;
			map2id_list = p;			
		
		} else {
			
			rc_log(LOG_ERR, "rc_read_mapfile: malformed line in %s, line %d", filename, lnr);  
			return (-1);

		}
	}
	
#undef SKIP

	fclose(mapfd);

	return 0;
}

/*
 * Function: rc_map2id
 *
 * Purpose: Map ttyname to port id
 *
 * Arguments: full pathname of the tty
 *
 * Returns: port id, zero if no entry found
 */

UINT4 rc_map2id(char *name)
{
	struct map2id_s *p;
	char ttyname[PATH_MAX];

	*ttyname = '\0';
	if (*name != '/')
		strcpy(ttyname, "/dev/");
		
	strncat(ttyname, name, sizeof(ttyname));
	
	for(p = map2id_list; p; p = p->next)
		if (!strcmp(ttyname, p->name)) return p->id;

	rc_log(LOG_WARNING,"rc_map2id: can't find tty %s in map database", ttyname);
	
	return 0;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 112,2;112,8

