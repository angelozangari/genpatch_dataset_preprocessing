static void
message_send_cb(GtkWidget *widget, spellchk *spell)
{
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	GtkTextMark *mark;
	gboolean replaced;
	if (spell->ignore_correction_on_send)
	{
		spell->ignore_correction_on_send = FALSE;
		return;
	}
#if 0
	if (!purple_prefs_get_bool("/plugins/gtk/spellchk/last_word_replace"))
		return;
#endif
	buffer = gtk_text_view_get_buffer(spell->view);
	gtk_text_buffer_get_end_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	spell->inserting = TRUE;
	replaced = check_range(spell, buffer, start, end, TRUE);
	spell->inserting = FALSE;
	/* if check_range modified the buffer, iter has been invalidated */
	mark = gtk_text_buffer_get_insert(buffer);
	gtk_text_buffer_get_iter_at_mark(buffer, &end, mark);
	gtk_text_buffer_move_mark(buffer, spell->mark_insert_end, &end);
	if (replaced)
	{
		g_signal_stop_emission_by_name(widget, "message_send");
		spell->ignore_correction_on_send = TRUE;
	}
}
