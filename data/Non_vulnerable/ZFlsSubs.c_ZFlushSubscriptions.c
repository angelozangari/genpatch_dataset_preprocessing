#include "internal.h"
Code_t ZFlushSubscriptions()
{
	register int i;
	if (!__subscriptions_list)
		return (ZERR_NONE);
	for (i=0;i<__subscriptions_num;i++) {
		free(__subscriptions_list[i].zsub_class);
		free(__subscriptions_list[i].zsub_classinst);
		free(__subscriptions_list[i].zsub_recipient);
	}
	free((char *)__subscriptions_list);
	__subscriptions_list = 0;
	__subscriptions_num = 0;
	return (ZERR_NONE);
}
