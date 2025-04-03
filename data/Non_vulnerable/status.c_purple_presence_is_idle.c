gboolean
purple_presence_is_idle(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, FALSE);
	return purple_presence_is_online(presence) && presence->idle;
}
