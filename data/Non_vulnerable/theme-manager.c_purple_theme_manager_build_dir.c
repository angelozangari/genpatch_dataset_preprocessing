static void
purple_theme_manager_build_dir(const gchar *root)
{
	gchar *purple_dir, *theme_dir;
	const gchar *name = NULL, *type = NULL;
	GDir *rdir, *tdir;
	PurpleThemeLoader *loader;
	rdir = g_dir_open(root, 0, NULL);
	if (!rdir)
		return;
	/* Parses directory by root/name/purple/type */
	while ((name = g_dir_read_name(rdir))) {
		purple_dir = g_build_filename(root, name, "purple", NULL);
		tdir = g_dir_open(purple_dir, 0, NULL);
		if (!tdir) {
			g_free(purple_dir);
			continue;
		}
		while ((type = g_dir_read_name(tdir))) {
			if ((loader = g_hash_table_lookup(theme_table, type))) {
				PurpleTheme *theme = NULL;
				theme_dir = g_build_filename(purple_dir, type, NULL);
				theme = purple_theme_loader_build(loader, theme_dir);
				g_free(theme_dir);
				if (PURPLE_IS_THEME(theme))
					purple_theme_manager_add_theme(theme);
			}
		}
		g_dir_close(tdir);
		g_free(purple_dir);
	}
	g_dir_close(rdir);
}
