void
purple_theme_manager_add_theme(PurpleTheme *theme)
{
	gchar *key;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	key = purple_theme_manager_make_key(purple_theme_get_name(theme),
			purple_theme_get_type_string(theme));
	g_return_if_fail(key);
	/* if something is already there do nothing */
	if (g_hash_table_lookup(theme_table, key) == NULL)
		g_hash_table_insert(theme_table, key, theme);
}
