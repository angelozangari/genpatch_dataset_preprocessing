gboolean
purple_presence_is_status_primitive_active(const PurplePresence *presence,
		PurpleStatusPrimitive primitive)
{
	GList *l;
	g_return_val_if_fail(presence  != NULL,              FALSE);
	g_return_val_if_fail(primitive != PURPLE_STATUS_UNSET, FALSE);
	for (l = purple_presence_get_statuses(presence);
	     l != NULL; l = l->next)
	{
		PurpleStatus *temp_status = l->data;
		PurpleStatusType *type = purple_status_get_type(temp_status);
		if (purple_status_type_get_primitive(type) == primitive &&
		    purple_status_is_active(temp_status))
			return TRUE;
	}
	return FALSE;
}
