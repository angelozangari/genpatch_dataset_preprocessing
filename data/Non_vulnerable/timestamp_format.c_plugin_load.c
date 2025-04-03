static gboolean
plugin_load(PurplePlugin *plugin)
{
	gpointer klass = NULL;
	purple_signal_connect(pidgin_conversations_get_handle(), "conversation-timestamp",
	                    plugin, PURPLE_CALLBACK(conversation_timestamp_cb), NULL);
	purple_signal_connect(purple_log_get_handle(), "log-timestamp",
	                    plugin, PURPLE_CALLBACK(log_timestamp_cb), NULL);
	klass = g_type_class_ref(GTK_TYPE_TEXT_VIEW);
	/* In 3.0.0, use purple_g_signal_connect_flags */
	g_signal_parse_name("populate_popup", GTK_TYPE_TEXT_VIEW, &signal_id, NULL, FALSE);
	hook_id = g_signal_add_emission_hook(signal_id, 0, textview_emission_hook,
			plugin, NULL);
	g_type_class_unref(klass);
	return TRUE;
}
