gboolean
purple_presence_is_available(const PurplePresence *presence)
{
	PurpleStatus *status;
	g_return_val_if_fail(presence != NULL, FALSE);
	status = purple_presence_get_active_status(presence);
	return ((status != NULL && purple_status_is_available(status)) &&
			!purple_presence_is_idle(presence));
}
