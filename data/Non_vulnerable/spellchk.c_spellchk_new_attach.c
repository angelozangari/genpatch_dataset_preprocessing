static void
spellchk_new_attach(PurpleConversation *conv)
{
	spellchk *spell;
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	PidginConversation *gtkconv;
	GtkTextView *view;
	gtkconv = PIDGIN_CONVERSATION(conv);
	view = GTK_TEXT_VIEW(gtkconv->entry);
	spell = g_object_get_data(G_OBJECT(view), SPELLCHK_OBJECT_KEY);
	if (spell != NULL)
		return;
	/* attach to the widget */
	spell = g_new0(spellchk, 1);
	spell->view = view;
	g_object_set_data_full(G_OBJECT(view), SPELLCHK_OBJECT_KEY, spell,
			(GDestroyNotify)spellchk_free);
	buffer = gtk_text_view_get_buffer(view);
	/* we create the mark here, but we don't use it until text is
	 * inserted, so we don't really care where iter points.  */
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	spell->mark_insert_start = gtk_text_buffer_create_mark(buffer,
			"spellchk-insert-start",
			&start, TRUE);
	spell->mark_insert_end = gtk_text_buffer_create_mark(buffer,
			"spellchk-insert-end",
			&start, TRUE);
	g_signal_connect_after(G_OBJECT(buffer),
			"delete-range",
			G_CALLBACK(delete_range_after), spell);
	g_signal_connect(G_OBJECT(buffer),
			"insert-text",
			G_CALLBACK(insert_text_before), spell);
	g_signal_connect_after(G_OBJECT(buffer),
			"insert-text",
			G_CALLBACK(insert_text_after), spell);
	g_signal_connect(G_OBJECT(gtkconv->entry), "message_send",
	                 G_CALLBACK(message_send_cb), spell);
	return;
}
