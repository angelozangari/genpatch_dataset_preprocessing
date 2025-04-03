gboolean
purple_presence_is_status_active(const PurplePresence *presence,
		const char *status_id)
{
	PurpleStatus *status;
	g_return_val_if_fail(presence  != NULL, FALSE);
	g_return_val_if_fail(status_id != NULL, FALSE);
	status = purple_presence_get_status(presence, status_id);
	return (status != NULL && purple_status_is_active(status));
}
