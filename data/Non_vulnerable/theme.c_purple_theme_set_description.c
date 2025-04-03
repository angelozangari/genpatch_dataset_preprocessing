void
purple_theme_set_description(PurpleTheme *theme, const gchar *description)
{
	PurpleThemePrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->description);
	priv->description = theme_clean_text(description);
}
