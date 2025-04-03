void
purple_theme_manager_unregister_type(PurpleThemeLoader *loader)
{
	const gchar *type;
	g_return_if_fail(PURPLE_IS_THEME_LOADER(loader));
	type = purple_theme_loader_get_type_string(loader);
	g_return_if_fail(type);
	if (g_hash_table_lookup(theme_table, type) == loader)
	{
		g_hash_table_remove(theme_table, type);
		g_hash_table_foreach_remove(theme_table,
				(GHRFunc)purple_theme_manager_is_theme_type, (gpointer)type);
	} /* only free if given registered loader */
}
