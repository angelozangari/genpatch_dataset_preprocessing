static void
purple_theme_loader_init(GTypeInstance *instance,
			gpointer klass)
{
	PurpleThemeLoader *loader = PURPLE_THEME_LOADER(instance);
	loader->priv = g_new0(PurpleThemeLoaderPrivate, 1);
}
