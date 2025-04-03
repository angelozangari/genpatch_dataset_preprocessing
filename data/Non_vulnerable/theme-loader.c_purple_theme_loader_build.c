PurpleTheme *
purple_theme_loader_build(PurpleThemeLoader *loader, const gchar *dir)
{
	return PURPLE_THEME_LOADER_GET_CLASS(loader)->purple_theme_loader_build(dir);
}
