 */
PurpleWhiteboard *purple_whiteboard_get_session(const PurpleAccount *account, const char *who)
{
	PurpleWhiteboard *wb;
	GList *l = wbList;
	/* Look for a whiteboard session between the local user and the remote user
	 */
	while(l != NULL)
	{
		wb = l->data;
		if(wb->account == account && purple_strequal(wb->who, who))
			return wb;
		l = l->next;
	}
	return NULL;
}
