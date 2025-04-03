static void
oscar_user_info_convert_and_add_hyperlink(PurpleAccount *account, OscarData *od, PurpleNotifyUserInfo *user_info,
						const char *name, const char *value, const char *url_prefix)
{
	gchar *utf8;
	if (value && value[0] && (utf8 = oscar_utf8_try_convert(account, od, value))) {
		gchar *tmp = g_strdup_printf("<a href=\"%s%s\">%s</a>", url_prefix, utf8, utf8);
		purple_notify_user_info_add_pair(user_info, name, tmp);
		g_free(utf8);
		g_free(tmp);
	}
}
