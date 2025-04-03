const char *
purple_status_type_get_name(const PurpleStatusType *status_type)
{
	g_return_val_if_fail(status_type != NULL, NULL);
	return status_type->name;
}
