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
