PurplePresence *
purple_presence_new_for_account(PurpleAccount *account)
{
	PurplePresence *presence = NULL;
	g_return_val_if_fail(account != NULL, NULL);
	presence = purple_presence_new(PURPLE_PRESENCE_CONTEXT_ACCOUNT);
	presence->u.account = account;
	presence->statuses = purple_prpl_get_statuses(account, presence);
	return presence;
}
