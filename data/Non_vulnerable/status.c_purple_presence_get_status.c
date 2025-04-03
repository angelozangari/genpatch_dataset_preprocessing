PurpleStatus *
purple_presence_get_status(const PurplePresence *presence, const char *status_id)
{
	PurpleStatus *status;
	GList *l = NULL;
	g_return_val_if_fail(presence  != NULL, NULL);
	g_return_val_if_fail(status_id != NULL, NULL);
	/* What's the purpose of this hash table? */
	status = (PurpleStatus *)g_hash_table_lookup(presence->status_table,
						   status_id);
	if (status == NULL) {
		for (l = purple_presence_get_statuses(presence);
			 l != NULL && status == NULL; l = l->next)
		{
			PurpleStatus *temp_status = l->data;
			if (purple_strequal(status_id, purple_status_get_id(temp_status)))
				status = temp_status;
		}
		if (status != NULL)
			g_hash_table_insert(presence->status_table,
								g_strdup(purple_status_get_id(status)), status);
	}
	return status;
}
