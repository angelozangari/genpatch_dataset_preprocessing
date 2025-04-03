time_t
purple_presence_get_idle_time(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, 0);
	return presence->idle_time;
}
