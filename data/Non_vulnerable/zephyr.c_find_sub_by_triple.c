}
static zephyr_triple *find_sub_by_triple(zephyr_account *zephyr,zephyr_triple * zt)
{
	zephyr_triple *curr_t;
	GSList *curr = zephyr->subscrips;
	while (curr) {
		curr_t = curr->data;
		if (triple_subset(zt, curr_t))
			return curr_t;
		curr = curr->next;
	}
	return NULL;
}
