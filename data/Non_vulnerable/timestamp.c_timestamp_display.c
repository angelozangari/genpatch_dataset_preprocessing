static void
timestamp_display(PurpleConversation *conv, time_t then, time_t now)
{
	PidginConversation *gtk_conv = PIDGIN_CONVERSATION(conv);
	GtkWidget *imhtml = gtk_conv->imhtml;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(imhtml));
	GtkTextIter iter;
	const char *mdate;
	int y, height;
	GdkRectangle rect;
	gboolean scrolled = FALSE;
	GtkTextTag *tag;
	/* display timestamp */
	mdate = purple_utf8_strftime(then == 0 ? "%H:%M" : "\n%H:%M",
		localtime(&now));
	gtk_text_buffer_get_end_iter(buffer, &iter);
	/* is the view already scrolled? */
	gtk_text_view_get_visible_rect(GTK_TEXT_VIEW(imhtml), &rect);
	gtk_text_view_get_line_yrange(GTK_TEXT_VIEW(imhtml), &iter, &y, &height);
	if (((y + height) - (rect.y + rect.height)) > height)
		scrolled = TRUE;
	if ((tag = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), "TIMESTAMP")) == NULL)
		tag = gtk_text_buffer_create_tag(buffer, "TIMESTAMP",
			"foreground", "#888888", "justification", GTK_JUSTIFY_CENTER,
			"weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_insert_with_tags(buffer, &iter, mdate,
		strlen(mdate), tag, NULL);
	/* scroll view if necessary */
	gtk_text_view_get_visible_rect(GTK_TEXT_VIEW(imhtml), &rect);
	gtk_text_view_get_line_yrange(
		GTK_TEXT_VIEW(imhtml), &iter, &y, &height);
	if (!scrolled && ((y + height) - (rect.y + rect.height)) > height &&
	    gtk_text_buffer_get_char_count(buffer)) {
		gboolean smooth = purple_prefs_get_bool(
			PIDGIN_PREFS_ROOT "/conversations/use_smooth_scrolling");
		gtk_imhtml_scroll_to_end(GTK_IMHTML(imhtml), smooth);
	}
}
