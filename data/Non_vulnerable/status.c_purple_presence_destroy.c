void
purple_presence_destroy(PurplePresence *presence)
{
	g_return_if_fail(presence != NULL);
	if (purple_presence_get_context(presence) == PURPLE_PRESENCE_CONTEXT_BUDDY)
	{
		g_free(presence->u.buddy.name);
	}
	else if (purple_presence_get_context(presence) == PURPLE_PRESENCE_CONTEXT_CONV)
	{
		g_free(presence->u.chat.user);
	}
	g_list_foreach(presence->statuses, (GFunc)purple_status_destroy, NULL);
	g_list_free(presence->statuses);
	g_hash_table_destroy(presence->status_table);
	PURPLE_DBUS_UNREGISTER_POINTER(presence);
	g_free(presence);
}
