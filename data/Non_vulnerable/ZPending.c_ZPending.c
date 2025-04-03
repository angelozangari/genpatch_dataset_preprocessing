#include "internal.h"
int ZPending()
{
	int retval;
	if (ZGetFD() < 0) {
		errno = ZERR_NOPORT;
		return (-1);
	}
	if ((retval = Z_ReadEnqueue()) != ZERR_NONE) {
		errno = retval;
		return (-1);
	}
	return(ZQLength());
}
