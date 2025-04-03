static gchar *
purple_theme_manager_make_key(const gchar *name, const gchar *type)
{
	g_return_val_if_fail(name && *name, NULL);
	g_return_val_if_fail(type && *type, NULL);
	return g_strconcat(type, "/", name, NULL);
}
