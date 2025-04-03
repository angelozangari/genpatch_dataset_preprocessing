}
static void message_send_cb(GtkWidget *widget, gpointer p)
{
	GtkTextIter start, end;
	PurplePluginProtocolInfo *prpl_info = NULL;
	PurpleConnection *gc;
	GtkTextBuffer *buffer;
	char *text;
	gc = console->gc;
	if (gc)
		prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console->entry));
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	text = gtk_imhtml_get_text(GTK_IMHTML(console->entry), &start, &end);
	if (prpl_info && prpl_info->send_raw != NULL)
		prpl_info->send_raw(gc, text, strlen(text));
	g_free(text);
	gtk_imhtml_clear(GTK_IMHTML(console->entry));
}
