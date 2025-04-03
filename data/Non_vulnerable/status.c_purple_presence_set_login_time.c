void
purple_presence_set_login_time(PurplePresence *presence, time_t login_time)
{
	g_return_if_fail(presence != NULL);
	if (presence->login_time == login_time)
		return;
	presence->login_time = login_time;
}
