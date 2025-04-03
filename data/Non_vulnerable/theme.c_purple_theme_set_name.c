void
purple_theme_set_name(PurpleTheme *theme, const gchar *name)
{
	PurpleThemePrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->name);
	priv->name = theme_clean_text(name);
}
