void
purple_theme_set_author(PurpleTheme *theme, const gchar *author)
{
	PurpleThemePrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME(theme));
	priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->author);
	priv->author = theme_clean_text(author);
}
