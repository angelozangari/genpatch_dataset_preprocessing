static void
timestamp_new_convo(PurpleConversation *conv)
{
	if (!g_list_find(purple_get_conversations(), conv))
		return;
	purple_conversation_set_data(conv, "timestamp-last", GINT_TO_POINTER(0));
}
