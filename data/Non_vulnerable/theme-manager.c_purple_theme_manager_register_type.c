void
purple_theme_manager_register_type(PurpleThemeLoader *loader)
{
	gchar *type;
	g_return_if_fail(PURPLE_IS_THEME_LOADER(loader));
	type = g_strdup(purple_theme_loader_get_type_string(loader));
	g_return_if_fail(type);
	/* if something is already there do nothing */
	if (!g_hash_table_lookup(theme_table, type))
		g_hash_table_insert(theme_table, type, loader);
}
