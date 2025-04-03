PurpleTheme *
purple_theme_manager_load_theme(const gchar *theme_dir, const gchar *type)
{
	PurpleThemeLoader *loader;
	g_return_val_if_fail(theme_dir != NULL && type != NULL, NULL);
	loader = g_hash_table_lookup(theme_table, type);
	g_return_val_if_fail(PURPLE_IS_THEME_LOADER(loader), NULL);
	return purple_theme_loader_build(loader, theme_dir);
}
