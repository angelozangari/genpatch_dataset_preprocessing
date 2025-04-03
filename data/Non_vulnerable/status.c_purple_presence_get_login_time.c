time_t
purple_presence_get_login_time(const PurplePresence *presence)
{
	g_return_val_if_fail(presence != NULL, 0);
	return purple_presence_is_online(presence) ? presence->login_time : 0;
}
