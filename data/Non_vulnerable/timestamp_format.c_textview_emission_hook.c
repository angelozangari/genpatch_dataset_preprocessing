static gboolean
textview_emission_hook(GSignalInvocationHint *hint, guint n_params,
		const GValue *pvalues, gpointer data)
{
	GtkTextView *view = GTK_TEXT_VIEW(g_value_get_object(pvalues));
	GtkWidget *menu, *item;
	GtkTextBuffer *buffer;
	GtkTextIter cursor;
	int cx, cy, bx, by;
	if (!GTK_IS_IMHTML(view))
		return TRUE;
#if GTK_CHECK_VERSION(2,14,0)
	if (!gdk_window_get_pointer(gtk_widget_get_window(GTK_WIDGET(view)), &cx, &cy, NULL))
		return TRUE;
#else
	if (!gdk_window_get_pointer(GTK_WIDGET(view)->window, &cx, &cy, NULL))
		return TRUE;
#endif
	buffer = gtk_text_view_get_buffer(view);
	gtk_text_view_window_to_buffer_coords(view, GTK_TEXT_WINDOW_TEXT, cx, cy, &bx, &by);
	gtk_text_view_get_iter_at_location(view, &cursor, bx, by);
	if (!gtk_text_iter_has_tag(&cursor,
				gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), "comment")))
		return TRUE;
	menu = g_value_get_object(&pvalues[1]);
	item = gtk_menu_item_new_with_label(_("Timestamp Format Options"));
	gtk_widget_show_all(item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(menu_cb), data);
	gtk_menu_shell_insert(GTK_MENU_SHELL(menu), item, 0);
	item = gtk_separator_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_shell_insert(GTK_MENU_SHELL(menu), item, 1);
	return TRUE;
}
