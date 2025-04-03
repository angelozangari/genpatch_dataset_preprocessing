static gboolean
change_stock_image(GtkWidget *widget, GdkEventButton *event, GtkWidget *image)
{
	GtkWidget *win = pidgin_buddy_icon_chooser_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
			stock_icon_selected, image);
	gtk_window_set_title(GTK_WINDOW(win),
	                     g_object_get_data(G_OBJECT(image), "localized-name"));
	gtk_widget_show_all(win);
	return TRUE;
}
