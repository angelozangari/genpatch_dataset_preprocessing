PurpleAccount *
purple_presence_get_account(const PurplePresence *presence)
{
	PurplePresenceContext context;
	g_return_val_if_fail(presence != NULL, NULL);
	context = purple_presence_get_context(presence);
	g_return_val_if_fail(context == PURPLE_PRESENCE_CONTEXT_ACCOUNT ||
			context == PURPLE_PRESENCE_CONTEXT_BUDDY, NULL);
	return presence->u.account;
}
