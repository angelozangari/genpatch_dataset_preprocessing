#include "internal.h"
Code_t ZFlushLocations()
{
	int i;
	if (!__locate_list)
		return (ZERR_NONE);
	for (i=0;i<__locate_num;i++) {
		free(__locate_list[i].host);
		free(__locate_list[i].time);
		free(__locate_list[i].tty);
	}
	free((char *)__locate_list);
	__locate_list = 0;
	__locate_num = 0;
	return (ZERR_NONE);
}
