PurpleStatusPrimitive
purple_status_type_get_primitive(const PurpleStatusType *status_type)
{
	g_return_val_if_fail(status_type != NULL, PURPLE_STATUS_UNSET);
	return status_type->primitive;
}
