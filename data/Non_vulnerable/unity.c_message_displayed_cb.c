static gboolean
message_displayed_cb(PurpleAccount *account, const char *who, char *message,
                     PurpleConversation *conv, PurpleMessageFlags flags)
{
	if ((purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_CHAT &&
	     alert_chat_nick && !(flags & PURPLE_MESSAGE_NICK)))
		return FALSE;
	if ((flags & PURPLE_MESSAGE_RECV) && !(flags & PURPLE_MESSAGE_DELAYED))
		alert(conv);
	return FALSE;
}
