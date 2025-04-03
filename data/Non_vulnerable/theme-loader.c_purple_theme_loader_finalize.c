static void
purple_theme_loader_finalize(GObject *obj)
{
	PurpleThemeLoader *loader = PURPLE_THEME_LOADER(obj);
	PurpleThemeLoaderPrivate *priv = PURPLE_THEME_LOADER_GET_PRIVATE(loader);
	g_free(priv->type);
	g_free(priv);
	parent_class->finalize(obj);
}
