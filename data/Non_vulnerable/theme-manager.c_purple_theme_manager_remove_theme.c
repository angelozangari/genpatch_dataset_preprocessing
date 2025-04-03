void
purple_theme_manager_remove_theme(PurpleTheme *theme)
{
	gchar *key;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	key = purple_theme_manager_make_key(purple_theme_get_name(theme),
			purple_theme_get_type_string(theme));
	g_return_if_fail(key);
	g_hash_table_remove(theme_table, key);
	g_free(key);
}
