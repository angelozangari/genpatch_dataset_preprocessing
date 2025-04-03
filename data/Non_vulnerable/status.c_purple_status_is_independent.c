gboolean
purple_status_is_independent(const PurpleStatus *status)
{
	g_return_val_if_fail(status != NULL, FALSE);
	return purple_status_type_is_independent(purple_status_get_type(status));
}
