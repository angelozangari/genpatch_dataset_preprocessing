void
purple_theme_manager_refresh(void)
{
	gchar *path = NULL;
	const gchar *xdg = NULL;
	gint i = 0;
	g_hash_table_foreach_remove(theme_table,
			(GHRFunc) purple_theme_manager_is_theme, NULL);
	/* Add themes from ~/.purple */
	path = g_build_filename(purple_user_dir(), "themes", NULL);
	purple_theme_manager_build_dir(path);
	g_free(path);
	/* look for XDG_DATA_HOME.  If we don't have it use ~/.local, and add it */
	if ((xdg = g_getenv("XDG_DATA_HOME")) != NULL)
		path = g_build_filename(xdg, "themes", NULL);
	else
		path = g_build_filename(purple_home_dir(), ".local", "themes", NULL);
	purple_theme_manager_build_dir(path);
	g_free(path);
	/* now dig through XDG_DATA_DIRS and add those too */
	xdg = g_getenv("XDG_DATA_DIRS");
	if (xdg) {
		gchar **xdg_dirs = g_strsplit(xdg, G_SEARCHPATH_SEPARATOR_S, 0);
		for (i = 0; xdg_dirs[i]; i++) {
			path = g_build_filename(xdg_dirs[i], "themes", NULL);
			purple_theme_manager_build_dir(path);
			g_free(path);
		}
		g_strfreev(xdg_dirs);
	}
}
