static void buddy_ticker_update_contact(PurpleContact *contact);
static gboolean buddy_ticker_destroy_window(GtkWidget *window,
		GdkEventAny *event, gpointer data) {
	if(window)
		gtk_widget_hide(window);
	return TRUE; /* don't actually destroy the window */
}
