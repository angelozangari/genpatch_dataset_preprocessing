static void
oscar_user_info_convert_and_add(PurpleAccount *account, OscarData *od, PurpleNotifyUserInfo *user_info,
					const char *name, const char *value)
{
	gchar *utf8;
	if (value && value[0] && (utf8 = oscar_utf8_try_convert(account, od, value))) {
		purple_notify_user_info_add_pair(user_info, name, utf8);
		g_free(utf8);
	}
}
