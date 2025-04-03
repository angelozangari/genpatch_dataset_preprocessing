PurpleStatus *
purple_presence_get_active_status(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, NULL);
	return presence->active_status;
}
