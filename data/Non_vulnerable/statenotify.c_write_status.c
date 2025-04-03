static void
write_status(PurpleBuddy *buddy, const char *message)
{
	PurpleAccount *account = NULL;
	PurpleConversation *conv;
	const char *who;
	char buf[256];
	char *escaped;
	const gchar *buddy_name = NULL;
	account = purple_buddy_get_account(buddy);
	buddy_name = purple_buddy_get_name(buddy);
	conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM,
												 buddy_name, account);
	if (conv == NULL)
		return;
	g_return_if_fail(conv->type == PURPLE_CONV_TYPE_IM);
	/* Prevent duplicate notifications for buddies in multiple groups */
	if (buddy != purple_find_buddy(account, buddy_name))
		return;
	who = purple_buddy_get_alias(buddy);
	escaped = g_markup_escape_text(who, -1);
	g_snprintf(buf, sizeof(buf), message, escaped);
	g_free(escaped);
	purple_conv_im_write(conv->u.im, NULL, buf, PURPLE_MESSAGE_SYSTEM | PURPLE_MESSAGE_ACTIVE_ONLY | PURPLE_MESSAGE_NO_LINKIFY, time(NULL));
}
