static void
detach_signals(PurpleConversation *conv)
{
	PidginConversation *gtkconv = NULL;
	guint id;
	gtkconv = PIDGIN_CONVERSATION(conv);
	if (!gtkconv)
		return;
	id = GPOINTER_TO_INT(purple_conversation_get_data(conv, "unity-imhtml-signal"));
	g_signal_handler_disconnect(gtkconv->imhtml, id);
	id = GPOINTER_TO_INT(purple_conversation_get_data(conv, "unity-entry-signal"));
	g_signal_handler_disconnect(gtkconv->entry, id);
	purple_conversation_set_data(conv, "unity-message-count",
	                             GINT_TO_POINTER(0));
}
