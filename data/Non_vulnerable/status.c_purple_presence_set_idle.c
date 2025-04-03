void
purple_presence_set_idle(PurplePresence *presence, gboolean idle, time_t idle_time)
{
	gboolean old_idle;
	time_t current_time;
	g_return_if_fail(presence != NULL);
	if (presence->idle == idle && presence->idle_time == idle_time)
		return;
	old_idle            = presence->idle;
	presence->idle      = idle;
	presence->idle_time = (idle ? idle_time : 0);
	current_time = time(NULL);
	if (purple_presence_get_context(presence) == PURPLE_PRESENCE_CONTEXT_BUDDY)
	{
		update_buddy_idle(purple_presence_get_buddy(presence), presence, current_time,
		                  old_idle, idle);
	}
	else if (purple_presence_get_context(presence) == PURPLE_PRESENCE_CONTEXT_ACCOUNT)
	{
		PurpleAccount *account;
		PurpleConnection *gc = NULL;
		PurplePlugin *prpl = NULL;
		PurplePluginProtocolInfo *prpl_info = NULL;
		account = purple_presence_get_account(presence);
		if (purple_prefs_get_bool("/purple/logging/log_system"))
		{
			PurpleLog *log = purple_account_get_log(account, FALSE);
			if (log != NULL)
			{
				char *msg, *tmp;
				if (idle)
					tmp = g_strdup_printf(_("+++ %s became idle"), purple_account_get_username(account));
				else
					tmp = g_strdup_printf(_("+++ %s became unidle"), purple_account_get_username(account));
				msg = g_markup_escape_text(tmp, -1);
				g_free(tmp);
				purple_log_write(log, PURPLE_MESSAGE_SYSTEM,
				                 purple_account_get_username(account),
				                 (idle ? idle_time : current_time), msg);
				g_free(msg);
			}
		}
		gc = purple_account_get_connection(account);
		if(gc)
			prpl = purple_connection_get_prpl(gc);
		if(PURPLE_CONNECTION_IS_CONNECTED(gc) && prpl != NULL)
			prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);
		if (prpl_info && prpl_info->set_idle)
			prpl_info->set_idle(gc, (idle ? (current_time - idle_time) : 0));
	}
}
