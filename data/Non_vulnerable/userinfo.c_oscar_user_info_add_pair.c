static void
oscar_user_info_add_pair(PurpleNotifyUserInfo *user_info, const char *name, const char *value)
{
	if (value && value[0]) {
		purple_notify_user_info_add_pair(user_info, name, value);
	}
}
