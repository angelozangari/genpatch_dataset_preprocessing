static int
alert(PurpleConversation *conv)
{
	gint count;
	PidginWindow *purplewin = NULL;
	if (conv == NULL || PIDGIN_CONVERSATION(conv) == NULL)
		return 0;
	purplewin = PIDGIN_CONVERSATION(conv)->win;
	if (!pidgin_conv_window_has_focus(purplewin) ||
		!pidgin_conv_window_is_active_conversation(conv))
	{
		count = GPOINTER_TO_INT(purple_conversation_get_data(conv,
		                        "unity-message-count"));
		if (!count++)
			++n_sources;
		purple_conversation_set_data(conv, "unity-message-count",
		                             GINT_TO_POINTER(count));
		messaging_menu_add_conversation(conv, count);
		update_launcher();
	}
	return 0;
}
static void
unalert(PurpleConversation *conv)
{
	if (GPOINTER_TO_INT(purple_conversation_get_data(conv, "unity-message-count")) > 0)
		--n_sources;
	purple_conversation_set_data(conv, "unity-message-count",
	                             GINT_TO_POINTER(0));
	messaging_menu_remove_conversation(conv);
	update_launcher();
}
