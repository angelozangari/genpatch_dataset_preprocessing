static const gchar *
disco_type_from_string(const gchar *str)
{
	int i = 0;
	g_return_val_if_fail(str != NULL, "");
	for ( ; disco_type_mappings[i].from; ++i) {
		if (!strcasecmp(str, disco_type_mappings[i].from))
			return disco_type_mappings[i].to;
	}
	/* fallback to the string itself */
	return str;
}
