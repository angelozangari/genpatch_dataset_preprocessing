PurpleStatus *
purple_status_new(PurpleStatusType *status_type, PurplePresence *presence)
{
	PurpleStatus *status;
	GList *l;
	g_return_val_if_fail(status_type != NULL, NULL);
	g_return_val_if_fail(presence    != NULL, NULL);
	status = g_new0(PurpleStatus, 1);
	PURPLE_DBUS_REGISTER_POINTER(status, PurpleStatus);
	status->type     = status_type;
	status->presence = presence;
	status->attr_values =
		g_hash_table_new_full(g_str_hash, g_str_equal, NULL,
		(GDestroyNotify)purple_value_destroy);
	for (l = purple_status_type_get_attrs(status_type); l != NULL; l = l->next)
	{
		PurpleStatusAttr *attr = (PurpleStatusAttr *)l->data;
		PurpleValue *value = purple_status_attr_get_value(attr);
		PurpleValue *new_value = purple_value_dup(value);
		g_hash_table_insert(status->attr_values,
							(char *)purple_status_attr_get_id(attr),
							new_value);
	}
	return status;
}
