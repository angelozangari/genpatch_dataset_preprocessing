const char *
purple_status_type_get_id(const PurpleStatusType *status_type)
{
	g_return_val_if_fail(status_type != NULL, NULL);
	return status_type->id;
}
