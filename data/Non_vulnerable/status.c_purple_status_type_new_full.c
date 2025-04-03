PurpleStatusType *
purple_status_type_new_full(PurpleStatusPrimitive primitive, const char *id,
						  const char *name, gboolean saveable,
						  gboolean user_settable, gboolean independent)
{
	PurpleStatusType *status_type;
	g_return_val_if_fail(primitive != PURPLE_STATUS_UNSET, NULL);
	status_type = g_new0(PurpleStatusType, 1);
	PURPLE_DBUS_REGISTER_POINTER(status_type, PurpleStatusType);
	status_type->primitive     = primitive;
	status_type->saveable      = saveable;
	status_type->user_settable = user_settable;
	status_type->independent   = independent;
	if (id != NULL)
		status_type->id = g_strdup(id);
	else
		status_type->id = g_strdup(purple_primitive_get_id_from_type(primitive));
	if (name != NULL)
		status_type->name = g_strdup(name);
	else
		status_type->name = g_strdup(purple_primitive_get_name_from_type(primitive));
	return status_type;
}
