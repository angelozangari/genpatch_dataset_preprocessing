void
oscar_user_info_display_error(OscarData *od, guint16 error_reason, gchar *buddy)
{
	PurpleNotifyUserInfo *user_info = purple_notify_user_info_new();
	gchar *buf = g_strdup_printf(_("User information not available: %s"), oscar_get_msgerr_reason(error_reason));
	purple_notify_user_info_add_pair(user_info, NULL, buf);
	purple_notify_userinfo(od->gc, buddy, user_info, NULL, NULL);
	purple_notify_user_info_destroy(user_info);
	if (!purple_conv_present_error(buddy, purple_connection_get_account(od->gc), buf))
		purple_notify_error(od->gc, NULL, buf, NULL);
	g_free(buf);
}
