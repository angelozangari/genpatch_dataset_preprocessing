static gboolean
purple_theme_manager_is_theme_type(gchar *key,
		gpointer value,
		gchar *user_data)
{
	return g_str_has_prefix(key, g_strconcat(user_data, "/", NULL));
}
