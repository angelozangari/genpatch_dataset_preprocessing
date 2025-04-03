static PidginStatusIconTheme *
create_icon_theme(GtkWidget *window)
{
	int s, i, j;
	const char *dirname = g_get_tmp_dir();
	PidginStatusIconTheme *theme;
	const char *author;
#ifndef _WIN32
	author = getlogin();
#else
	author = "user";
#endif
	theme = g_object_new(PIDGIN_TYPE_STATUS_ICON_THEME, "type", "status-icon",
				"author", author,
				"directory", dirname,
				NULL);
	for (s = 0; sections[s].heading; s++) {
		GtkWidget *vbox = g_object_get_data(G_OBJECT(window), sections[s].heading);
		for (i = 0; sections[s].options[i].stockid; i++) {
			GtkWidget *image = g_object_get_data(G_OBJECT(vbox), sections[s].options[i].stockid);
			GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(image), "pixbuf");
			if (!pixbuf)
				continue;
			pidgin_icon_theme_set_icon(PIDGIN_ICON_THEME(theme), sections[s].options[i].stockid,
					sections[s].options[i].stockid);
			for (j = 0; stocksizes[j]; j++) {
				int width, height;
				GtkIconSize iconsize;
				char size[8];
				char *name;
				GdkPixbuf *scale;
				GError *error = NULL;
				if (!(sections[s].flags & (1 << j)))
					continue;
				iconsize = gtk_icon_size_from_name(stocksizes[j]);
				gtk_icon_size_lookup(iconsize, &width, &height);
				g_snprintf(size, sizeof(size), "%d", width);
				if (i == 0) {
					name = g_build_filename(dirname, size, NULL);
					purple_build_dir(name, S_IRUSR | S_IWUSR | S_IXUSR);
					g_free(name);
				}
				name = g_build_filename(dirname, size, sections[s].options[i].stockid, NULL);
				scale = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
				gdk_pixbuf_save(scale, name, "png", &error, "compression", "9", NULL);
				g_free(name);
				g_object_unref(G_OBJECT(scale));
				if (error)
					g_error_free(error);
			}
		}
	}
	return theme;
}
