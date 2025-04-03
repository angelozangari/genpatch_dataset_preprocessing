const gchar *
purple_theme_loader_get_type_string(PurpleThemeLoader *theme_loader)
{
	PurpleThemeLoaderPrivate *priv = NULL;
	g_return_val_if_fail(PURPLE_IS_THEME_LOADER(theme_loader), NULL);
	priv = PURPLE_THEME_LOADER_GET_PRIVATE(theme_loader);
	return priv->type;
}
