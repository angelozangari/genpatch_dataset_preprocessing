void
purple_theme_loader_set_type_string(PurpleThemeLoader *loader, const gchar *type)
{
	PurpleThemeLoaderPrivate *priv;
	g_return_if_fail(PURPLE_IS_THEME_LOADER(loader));
	priv = PURPLE_THEME_LOADER_GET_PRIVATE(loader);
	g_free(priv->type);
	priv->type = g_strdup(type);
}
