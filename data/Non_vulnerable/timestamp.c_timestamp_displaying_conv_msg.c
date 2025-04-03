static gboolean
timestamp_displaying_conv_msg(PurpleAccount *account, const char *who,
			      char **buffer, PurpleConversation *conv,
			      PurpleMessageFlags flags, void *data)
{
	time_t now = time(NULL) / interval * interval;
	time_t then;
	if (!g_list_find(purple_get_conversations(), conv))
		return FALSE;
	then = GPOINTER_TO_INT(purple_conversation_get_data(
		conv, "timestamp-last"));
	if (now - then >= interval) {
		timestamp_display(conv, then, now);
		purple_conversation_set_data(
			conv, "timestamp-last", GINT_TO_POINTER(now));
	}
	return FALSE;
}
