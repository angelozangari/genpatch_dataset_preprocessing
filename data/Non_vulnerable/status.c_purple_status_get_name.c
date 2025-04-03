const char *
purple_status_get_name(const PurpleStatus *status)
{
	g_return_val_if_fail(status != NULL, NULL);
	return purple_status_type_get_name(purple_status_get_type(status));
}
