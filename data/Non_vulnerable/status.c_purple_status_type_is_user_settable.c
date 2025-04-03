gboolean
purple_status_type_is_user_settable(const PurpleStatusType *status_type)
{
	g_return_val_if_fail(status_type != NULL, FALSE);
	return status_type->user_settable;
}
