static void
purple_theme_init(GTypeInstance *instance,
		gpointer klass)
{
	PurpleTheme *theme = PURPLE_THEME(instance);
	theme->priv = g_new0(PurpleThemePrivate, 1);
}
