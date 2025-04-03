const PurpleStatusType *
purple_status_type_find_with_id(GList *status_types, const char *id)
{
	PurpleStatusType *status_type;
	g_return_val_if_fail(id != NULL, NULL);
	while (status_types != NULL)
	{
		status_type = status_types->data;
		if (purple_strequal(id, status_type->id))
			return status_type;
		status_types = status_types->next;
	}
	return NULL;
}
