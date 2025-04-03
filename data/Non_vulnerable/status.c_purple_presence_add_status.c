void
purple_presence_add_status(PurplePresence *presence, PurpleStatus *status)
{
	g_return_if_fail(presence != NULL);
	g_return_if_fail(status   != NULL);
	presence->statuses = g_list_append(presence->statuses, status);
	g_hash_table_insert(presence->status_table,
	g_strdup(purple_status_get_id(status)), status);
}
