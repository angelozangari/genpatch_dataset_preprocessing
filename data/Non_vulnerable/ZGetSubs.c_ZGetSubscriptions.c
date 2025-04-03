Code_t ZGetSubscriptions(ZSubscription_t *subscription, int *numsubs);
Code_t ZGetSubscriptions(ZSubscription_t *subscription, int *numsubs)
{
    int i;
    if (!__subscriptions_list)
	return (ZERR_NOSUBSCRIPTIONS);
    if (__subscriptions_next == __subscriptions_num)
	return (ZERR_NOMORESUBSCRIPTIONS);
    for (i=0;i<min(*numsubs, __subscriptions_num-__subscriptions_next);i++)
	subscription[i] = __subscriptions_list[i+__subscriptions_next];
    if (__subscriptions_num-__subscriptions_next < *numsubs)
	*numsubs = __subscriptions_num-__subscriptions_next;
    __subscriptions_next += *numsubs;
    return (ZERR_NONE);
}
