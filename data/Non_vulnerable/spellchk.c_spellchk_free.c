static void
spellchk_free(spellchk *spell)
{
	GtkTextBuffer *buffer;
	g_return_if_fail(spell != NULL);
	buffer = gtk_text_view_get_buffer(spell->view);
	g_signal_handlers_disconnect_matched(buffer,
			G_SIGNAL_MATCH_DATA,
			0, 0, NULL, NULL,
			spell);
	g_free(spell->word);
	g_free(spell);
}
