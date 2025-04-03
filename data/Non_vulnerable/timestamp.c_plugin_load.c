static gboolean
plugin_load(PurplePlugin *plugin)
{
	void *conv_handle = purple_conversations_get_handle();
	void *gtkconv_handle = pidgin_conversations_get_handle();
	/* lower priority to display initial timestamp after logged messages */
	purple_signal_connect_priority(conv_handle, "conversation-created",
		plugin, PURPLE_CALLBACK(timestamp_new_convo), NULL,
		PURPLE_SIGNAL_PRIORITY_DEFAULT + 1);
	purple_signal_connect(gtkconv_handle, "displaying-chat-msg",
		plugin, PURPLE_CALLBACK(timestamp_displaying_conv_msg), NULL);
	purple_signal_connect(gtkconv_handle, "displaying-im-msg",
		plugin, PURPLE_CALLBACK(timestamp_displaying_conv_msg), NULL);
	interval = purple_prefs_get_int("/plugins/gtk/timestamp/interval") / 1000;
	return TRUE;
}
