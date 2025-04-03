void
purple_theme_set_dir(PurpleTheme *theme, const gchar *dir)
{
	PurpleThemePrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->dir);
	priv->dir = g_strdup(dir);
}
