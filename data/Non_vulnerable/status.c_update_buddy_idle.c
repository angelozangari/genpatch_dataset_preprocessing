static void
update_buddy_idle(PurpleBuddy *buddy, PurplePresence *presence,
		time_t current_time, gboolean old_idle, gboolean idle)
{
	PurpleBlistUiOps *ops = purple_blist_get_ui_ops();
	PurpleAccount *account = purple_buddy_get_account(buddy);
	if (!old_idle && idle)
	{
		if (purple_prefs_get_bool("/purple/logging/log_system"))
		{
			PurpleLog *log = purple_account_get_log(account, FALSE);
			if (log != NULL)
			{
				char *tmp, *tmp2;
				tmp = g_strdup_printf(_("%s became idle"),
				purple_buddy_get_alias(buddy));
				tmp2 = g_markup_escape_text(tmp, -1);
				g_free(tmp);
				purple_log_write(log, PURPLE_MESSAGE_SYSTEM,
				purple_buddy_get_alias(buddy), current_time, tmp2);
				g_free(tmp2);
			}
		}
	}
	else if (old_idle && !idle)
	{
		if (purple_prefs_get_bool("/purple/logging/log_system"))
		{
			PurpleLog *log = purple_account_get_log(account, FALSE);
			if (log != NULL)
			{
				char *tmp, *tmp2;
				tmp = g_strdup_printf(_("%s became unidle"),
				purple_buddy_get_alias(buddy));
				tmp2 = g_markup_escape_text(tmp, -1);
				g_free(tmp);
				purple_log_write(log, PURPLE_MESSAGE_SYSTEM,
				purple_buddy_get_alias(buddy), current_time, tmp2);
				g_free(tmp2);
			}
		}
	}
	if (old_idle != idle)
		purple_signal_emit(purple_blist_get_handle(), "buddy-idle-changed", buddy,
		                 old_idle, idle);
	purple_contact_invalidate_priority_buddy(purple_buddy_get_contact(buddy));
	/* Should this be done here? It'd perhaps make more sense to
	 * connect to buddy-[un]idle signals and update from there
	 */
	if (ops != NULL && ops->update != NULL)
		ops->update(purple_get_blist(), (PurpleBlistNode *)buddy);
}
