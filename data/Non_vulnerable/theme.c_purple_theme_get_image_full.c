gchar *
purple_theme_get_image_full(PurpleTheme *theme)
{
	const gchar *filename = purple_theme_get_image(theme);
	if (filename)
		return g_build_filename(purple_theme_get_dir(PURPLE_THEME(theme)), filename, NULL);
	else
		return NULL;
}
