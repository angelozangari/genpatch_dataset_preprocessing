void
purple_theme_set_type_string(PurpleTheme *theme, const gchar *type)
{
	PurpleThemePrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->type);
	priv->type = g_strdup(type);
}
