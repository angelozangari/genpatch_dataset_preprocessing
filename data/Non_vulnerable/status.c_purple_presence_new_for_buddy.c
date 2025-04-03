PurplePresence *
purple_presence_new_for_buddy(PurpleBuddy *buddy)
{
	PurplePresence *presence;
	PurpleAccount *account;
	g_return_val_if_fail(buddy != NULL, NULL);
	account = purple_buddy_get_account(buddy);
	presence = purple_presence_new(PURPLE_PRESENCE_CONTEXT_BUDDY);
	presence->u.buddy.name    = g_strdup(purple_buddy_get_name(buddy));
	presence->u.buddy.account = account;
	presence->statuses = purple_prpl_get_statuses(account, presence);
	presence->u.buddy.buddy = buddy;
	return presence;
}
