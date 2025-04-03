PurpleTheme *
purple_theme_manager_find_theme(const gchar *name,
		const gchar *type)
{
	gchar *key;
	PurpleTheme *theme;
	key = purple_theme_manager_make_key(name, type);
	g_return_val_if_fail(key, NULL);
	theme = g_hash_table_lookup(theme_table, key);
	g_free(key);
	return theme;
}
