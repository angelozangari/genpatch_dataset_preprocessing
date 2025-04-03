const gchar *
purple_theme_get_image(PurpleTheme *theme)
{
	PurpleThemePrivate *priv;
	g_return_val_if_fail(PURPLE_IS_THEME(theme), NULL);
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	return priv->img;
}
