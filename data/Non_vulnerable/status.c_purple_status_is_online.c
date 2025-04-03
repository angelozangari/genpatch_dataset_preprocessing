gboolean
purple_status_is_online(const PurpleStatus *status)
{
	PurpleStatusPrimitive primitive;
	g_return_val_if_fail( status != NULL, FALSE);
	primitive = purple_status_type_get_primitive(purple_status_get_type(status));
	return (primitive != PURPLE_STATUS_UNSET &&
			primitive != PURPLE_STATUS_OFFLINE);
}
