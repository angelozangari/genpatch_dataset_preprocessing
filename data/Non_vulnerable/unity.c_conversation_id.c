static gchar *
conversation_id(PurpleConversation *conv)
{
	PurpleConversationType conv_type = purple_conversation_get_type(conv);
	PurpleAccount *account = purple_conversation_get_account(conv);
	char type[2] = "0";
	type[0] += conv_type;
	return g_strconcat(type, ":",
	                   purple_conversation_get_name(conv), ":",
	                   purple_account_get_username(account), ":",
	                   purple_account_get_protocol_id(account), NULL);
}
