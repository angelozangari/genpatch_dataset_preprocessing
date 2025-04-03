static void
purple_theme_finalize(GObject *obj)
{
	PurpleTheme *theme = PURPLE_THEME(obj);
	PurpleThemePrivate *priv = PURPLE_THEME_GET_PRIVATE(theme);
	g_free(priv->name);
	g_free(priv->description);
	g_free(priv->author);
	g_free(priv->type);
	g_free(priv->dir);
	g_free(priv->img);
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}
