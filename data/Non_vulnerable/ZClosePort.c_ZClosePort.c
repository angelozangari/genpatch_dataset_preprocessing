#include "internal.h"
Code_t ZClosePort()
{
    if (__Zephyr_fd >= 0 && __Zephyr_open)
	(void) close(__Zephyr_fd);
    __Zephyr_fd = -1;
    __Zephyr_open = 0;
    return (ZERR_NONE);
}
