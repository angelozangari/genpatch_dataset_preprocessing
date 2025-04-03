static gboolean
plugin_unload(PurplePlugin *plugin)
{
	GList *convs;
	/* Detach from existing conversations */
	for (convs = purple_get_conversations(); convs != NULL; convs = convs->next)
	{
		PidginConversation *gtkconv = PIDGIN_CONVERSATION((PurpleConversation *)convs->data);
		spellchk *spell = g_object_get_data(G_OBJECT(gtkconv->entry), SPELLCHK_OBJECT_KEY);
		g_signal_handlers_disconnect_by_func(gtkconv->entry, message_send_cb, spell);
		g_object_set_data(G_OBJECT(gtkconv->entry), SPELLCHK_OBJECT_KEY, NULL);
	}
	return TRUE;
}
