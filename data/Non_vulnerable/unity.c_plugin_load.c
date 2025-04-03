static gboolean
plugin_load(PurplePlugin *plugin)
{
	GList *convs = purple_get_conversations();
	PurpleSavedStatus *saved_status;
	void *conv_handle = purple_conversations_get_handle();
	void *gtk_conv_handle = pidgin_conversations_get_handle();
	void *savedstat_handle = purple_savedstatuses_get_handle();
	alert_chat_nick = purple_prefs_get_bool("/plugins/gtk/unity/alert_chat_nick");
	mmapp = messaging_menu_app_new("pidgin.desktop");
	g_object_ref(mmapp);
	messaging_menu_app_register(mmapp);
	messaging_menu_text = purple_prefs_get_int("/plugins/gtk/unity/messaging_menu_text");
	g_signal_connect(mmapp, "activate-source",
	                 G_CALLBACK(message_source_activated), NULL);
	g_signal_connect(mmapp, "status-changed",
	                 G_CALLBACK(messaging_menu_status_changed), NULL);
	saved_status = purple_savedstatus_get_current();
	status_changed_cb(saved_status);
	purple_signal_connect(savedstat_handle, "savedstatus-changed", plugin,
	                    PURPLE_CALLBACK(status_changed_cb), NULL);
	launcher = unity_launcher_entry_get_for_desktop_id("pidgin.desktop");
	g_object_ref(launcher);
	launcher_count = purple_prefs_get_int("/plugins/gtk/unity/launcher_count");
	purple_signal_connect(gtk_conv_handle, "displayed-im-msg", plugin,
	                    PURPLE_CALLBACK(message_displayed_cb), NULL);
	purple_signal_connect(gtk_conv_handle, "displayed-chat-msg", plugin,
	                    PURPLE_CALLBACK(message_displayed_cb), NULL);
	purple_signal_connect(conv_handle, "sent-im-msg", plugin,
	                    PURPLE_CALLBACK(im_sent_im), NULL);
	purple_signal_connect(conv_handle, "sent-chat-msg", plugin,
	                    PURPLE_CALLBACK(chat_sent_im), NULL);
	purple_signal_connect(conv_handle, "conversation-created", plugin,
	                    PURPLE_CALLBACK(conv_created), NULL);
	purple_signal_connect(conv_handle, "deleting-conversation", plugin,
	                    PURPLE_CALLBACK(deleting_conv), NULL);
	while (convs) {
		PurpleConversation *conv = (PurpleConversation *)convs->data;
		attach_signals(conv);
		convs = convs->next;
	}
	return TRUE;
}
