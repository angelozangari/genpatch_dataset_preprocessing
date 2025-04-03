 */
static gboolean plugin_load(PurplePlugin *plugin) {
	purple_signal_connect(purple_conversations_get_handle(),
		"conversation-created", plugin,
		PURPLE_CALLBACK(new_conversation_cb), NULL);
	/* Set callback to remove window from the list, if the window is destroyed */
	purple_signal_connect(purple_conversations_get_handle(),
		"deleting-conversation", plugin,
		PURPLE_CALLBACK(conversation_delete_cb), NULL);
	purple_signal_connect(pidgin_conversations_get_handle(),
		"conversation-dragging", plugin,
		PURPLE_CALLBACK(set_conv_window_trans), NULL);
	purple_signal_connect(purple_conversations_get_handle(),
		"conversation-updated", plugin,
		PURPLE_CALLBACK(conv_updated_cb), NULL);
	update_existing_convs();
	if (blist)
		blist_created_cb(NULL, NULL);
	else
		purple_signal_connect(pidgin_blist_get_handle(),
			"gtkblist-created", plugin,
			PURPLE_CALLBACK(blist_created_cb), NULL);
	return TRUE;
}
