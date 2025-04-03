PurplePresence *
purple_presence_new_for_conv(PurpleConversation *conv)
{
	PurplePresence *presence;
	g_return_val_if_fail(conv != NULL, NULL);
	presence = purple_presence_new(PURPLE_PRESENCE_CONTEXT_CONV);
	presence->u.chat.conv = conv;
	/* presence->statuses = purple_prpl_get_statuses(conv->account, presence); ? */
	return presence;
}
