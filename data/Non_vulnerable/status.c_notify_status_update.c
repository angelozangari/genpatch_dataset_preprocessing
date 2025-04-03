static void
notify_status_update(PurplePresence *presence, PurpleStatus *old_status,
					 PurpleStatus *new_status)
{
	PurplePresenceContext context = purple_presence_get_context(presence);
	if (context == PURPLE_PRESENCE_CONTEXT_ACCOUNT)
	{
		PurpleAccount *account = purple_presence_get_account(presence);
		PurpleAccountUiOps *ops = purple_accounts_get_ui_ops();
		if (purple_account_get_enabled(account, purple_core_get_ui()))
			purple_prpl_change_account_status(account, old_status, new_status);
		if (ops != NULL && ops->status_changed != NULL)
		{
			ops->status_changed(account, new_status);
		}
	}
	else if (context == PURPLE_PRESENCE_CONTEXT_BUDDY)
	{
			notify_buddy_status_update(purple_presence_get_buddy(presence), presence,
					old_status, new_status);
	}
}
