static void
notify_buddy_status_update(PurpleBuddy *buddy, PurplePresence *presence,
		PurpleStatus *old_status, PurpleStatus *new_status)
{
	if (purple_prefs_get_bool("/purple/logging/log_system"))
	{
		time_t current_time = time(NULL);
		const char *buddy_alias = purple_buddy_get_alias(buddy);
		char *tmp, *logtmp;
		PurpleLog *log;
		if (old_status != NULL)
		{
			tmp = g_strdup_printf(_("%s (%s) changed status from %s to %s"), buddy_alias,
			                      purple_buddy_get_name(buddy),
			                      purple_status_get_name(old_status),
			                      purple_status_get_name(new_status));
			logtmp = g_markup_escape_text(tmp, -1);
		}
		else
		{
			/* old_status == NULL when an independent status is toggled. */
			if (purple_status_is_active(new_status))
			{
				tmp = g_strdup_printf(_("%s (%s) is now %s"), buddy_alias,
				                      purple_buddy_get_name(buddy),
				                      purple_status_get_name(new_status));
				logtmp = g_markup_escape_text(tmp, -1);
			}
			else
			{
				tmp = g_strdup_printf(_("%s (%s) is no longer %s"), buddy_alias,
				                      purple_buddy_get_name(buddy),
				                      purple_status_get_name(new_status));
				logtmp = g_markup_escape_text(tmp, -1);
			}
		}
		log = purple_account_get_log(purple_buddy_get_account(buddy), FALSE);
		if (log != NULL)
		{
			purple_log_write(log, PURPLE_MESSAGE_SYSTEM, buddy_alias,
			               current_time, logtmp);
		}
		g_free(tmp);
		g_free(logtmp);
	}
}
