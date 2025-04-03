}
static zephyr_triple *new_triple(zephyr_account *zephyr,const char *c, const char *i, const char *r)
{
	zephyr_triple *zt;
	zt = g_new0(zephyr_triple, 1);
	zt->class = g_strdup(c);
	zt->instance = g_strdup(i);
	zt->recipient = g_strdup(r);
	zt->name = g_strdup_printf("%s,%s,%s", c, i?i:"", r?r:"");
	zt->id = ++(zephyr->last_id);
	zt->open = FALSE;
	return zt;
}
