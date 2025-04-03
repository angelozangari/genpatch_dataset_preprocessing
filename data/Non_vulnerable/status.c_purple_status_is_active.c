gboolean
purple_status_is_active(const PurpleStatus *status)
{
	g_return_val_if_fail(status != NULL, FALSE);
	return status->active;
}
