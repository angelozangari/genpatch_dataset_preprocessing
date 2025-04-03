}
PurpleWhiteboard *purple_whiteboard_create(PurpleAccount *account, const char *who, int state)
{
	PurplePluginProtocolInfo *prpl_info;
	PurpleWhiteboard *wb = g_new0(PurpleWhiteboard, 1);
	wb->account = account;
	wb->state   = state;
	wb->who     = g_strdup(who);
	prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(purple_connection_get_prpl(
				purple_account_get_connection(account)));
	purple_whiteboard_set_prpl_ops(wb, prpl_info->whiteboard_prpl_ops);
	/* Start up protocol specifics */
	if(wb->prpl_ops && wb->prpl_ops->start)
		wb->prpl_ops->start(wb);
	wbList = g_list_append(wbList, wb);
	return wb;
}
