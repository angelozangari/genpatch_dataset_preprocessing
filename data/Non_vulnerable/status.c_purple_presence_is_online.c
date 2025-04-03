gboolean
purple_presence_is_online(const PurplePresence *presence)
{
	PurpleStatus *status;
	g_return_val_if_fail(presence != NULL, FALSE);
	if ((status = purple_presence_get_active_status(presence)) == NULL)
		return FALSE;
	return purple_status_is_online(status);
}
