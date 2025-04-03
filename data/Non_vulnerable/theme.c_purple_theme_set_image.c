void
purple_theme_set_image(PurpleTheme *theme, const gchar *img)
{
	PurpleThemePrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->img);
	priv->img = g_strdup(img);
}
