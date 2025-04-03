static gboolean
plugin_load(PurplePlugin *plugin)
{
	void *conv_handle = purple_conversations_get_handle();
	GList *convs;
	load_conf();
	/* Attach to existing conversations */
	for (convs = purple_get_conversations(); convs != NULL; convs = convs->next)
	{
		spellchk_new_attach((PurpleConversation *)convs->data);
	}
	purple_signal_connect(conv_handle, "conversation-created",
			    plugin, PURPLE_CALLBACK(spellchk_new_attach), NULL);
	return TRUE;
}
