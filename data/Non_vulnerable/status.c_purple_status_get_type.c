PurpleStatusType *
purple_status_get_type(const PurpleStatus *status)
{
	g_return_val_if_fail(status != NULL, NULL);
	return status->type;
}
