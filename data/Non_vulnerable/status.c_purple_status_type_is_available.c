gboolean
purple_status_type_is_available(const PurpleStatusType *status_type)
{
	PurpleStatusPrimitive primitive;
	g_return_val_if_fail(status_type != NULL, FALSE);
	primitive = purple_status_type_get_primitive(status_type);
	return (primitive == PURPLE_STATUS_AVAILABLE);
}
