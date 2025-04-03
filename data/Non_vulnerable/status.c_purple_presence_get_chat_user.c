const char *
purple_presence_get_chat_user(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, NULL);
	g_return_val_if_fail(purple_presence_get_context(presence) ==
			PURPLE_PRESENCE_CONTEXT_CONV, NULL);
	return presence->u.chat.user;
}
