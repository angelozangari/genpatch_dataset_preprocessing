}
static void free_triple(zephyr_triple * zt)
{
	g_free(zt->class);
	g_free(zt->instance);
	g_free(zt->recipient);
	g_free(zt->name);
	g_free(zt);
}
