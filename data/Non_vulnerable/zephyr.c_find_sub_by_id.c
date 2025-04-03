}
static zephyr_triple *find_sub_by_id(zephyr_account *zephyr,int id)
{
	zephyr_triple *zt;
	GSList *curr = zephyr->subscrips;
	while (curr) {
		zt = curr->data;
		if (zt->id == id)
			return zt;
		curr = curr->next;
	}
	return NULL;
}
