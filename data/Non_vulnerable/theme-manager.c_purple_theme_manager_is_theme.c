static gboolean
purple_theme_manager_is_theme(gchar *key,
		gpointer value,
		gchar *user_data)
{
	return PURPLE_IS_THEME(value);
}
