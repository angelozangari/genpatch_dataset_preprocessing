static void
xmlnode_to_str_foreach_append_ns(const char *key, const char *value,
	GString *buf)
{
	if (*key) {
		g_string_append_printf(buf, " xmlns:%s='%s'", key, value);
	} else {
		g_string_append_printf(buf, " xmlns='%s'", value);
	}
}
