static void
conv_created(PurpleConversation *conv)
{
	purple_conversation_set_data(conv, "unity-message-count",
	                             GINT_TO_POINTER(0));
	attach_signals(conv);
}
