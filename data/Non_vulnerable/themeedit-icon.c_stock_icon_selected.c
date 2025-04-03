static void
stock_icon_selected(const char *filename, gpointer image)
{
	GError *error = NULL;
	GdkPixbuf *scale;
	int i;
	GdkPixbuf *pixbuf;
	if (!filename)
		return;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if (error || !pixbuf) {
		purple_debug_error("theme-editor-icon", "Unable to load icon file '%s' (%s)\n",
				filename, error ? error->message : "Reason unknown");
		if (error)
			g_error_free(error);
		return;
	}
	scale = gdk_pixbuf_scale_simple(pixbuf, 16, 16, GDK_INTERP_BILINEAR);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), scale);
	g_object_unref(G_OBJECT(scale));
	/* Update the size previews */
	for (i = 0; stocksizes[i]; i++) {
		int width, height;
		GtkIconSize iconsize;
		GtkWidget *prev = g_object_get_data(G_OBJECT(image), stocksizes[i]);
		if (!prev)
			continue;
		iconsize = gtk_icon_size_from_name(stocksizes[i]);
		gtk_icon_size_lookup(iconsize, &width, &height);
		scale = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
		gtk_image_set_from_pixbuf(GTK_IMAGE(prev), scale);
		g_object_unref(G_OBJECT(scale));
	}
	/* Save the original pixbuf so we can use it for resizing later */
	g_object_set_data_full(G_OBJECT(image), "pixbuf", pixbuf,
			(GDestroyNotify)g_object_unref);
}
