void
oscar_user_info_display_aim(OscarData *od, aim_userinfo_t *userinfo)
{
	PurpleConnection *gc = od->gc;
	PurpleAccount *account = purple_connection_get_account(gc);
	PurpleNotifyUserInfo *user_info = purple_notify_user_info_new();
	gchar *tmp = NULL, *info_utf8 = NULL, *base_profile_url = NULL;
	oscar_user_info_append_status(gc, user_info, /* PurpleBuddy */ NULL, userinfo, /* use_html_status */ TRUE);
	if ((userinfo->present & AIM_USERINFO_PRESENT_IDLE) && userinfo->idletime != 0) {
		tmp = purple_str_seconds_to_string(userinfo->idletime*60);
		oscar_user_info_add_pair(user_info, _("Idle"), tmp);
		g_free(tmp);
	}
	oscar_user_info_append_extra_info(gc, user_info, NULL, userinfo);
	if ((userinfo->present & AIM_USERINFO_PRESENT_ONLINESINCE) && !oscar_util_valid_name_sms(userinfo->bn)) {
		/* An SMS contact is always online; its Online Since value is not useful */
		time_t t = userinfo->onlinesince;
		oscar_user_info_add_pair(user_info, _("Online Since"), purple_date_format_full(localtime(&t)));
	}
	if (userinfo->present & AIM_USERINFO_PRESENT_MEMBERSINCE) {
		time_t t = userinfo->membersince;
		oscar_user_info_add_pair(user_info, _("Member Since"), purple_date_format_full(localtime(&t)));
	}
	if (userinfo->capabilities != 0) {
		tmp = oscar_caps_to_string(userinfo->capabilities);
		oscar_user_info_add_pair(user_info, _("Capabilities"), tmp);
		g_free(tmp);
	}
	/* Info */
	if ((userinfo->info_len > 0) && (userinfo->info != NULL) && (userinfo->info_encoding != NULL)) {
		info_utf8 = oscar_encoding_to_utf8(userinfo->info_encoding, userinfo->info, userinfo->info_len);
		tmp = oscar_util_format_string(info_utf8, purple_account_get_username(account));
		purple_notify_user_info_add_section_break(user_info);
		oscar_user_info_add_pair(user_info, _("Profile"), tmp);
		g_free(tmp);
		g_free(info_utf8);
	}
	purple_notify_user_info_add_section_break(user_info);
	base_profile_url = oscar_util_valid_name_icq(userinfo->bn) ? "http://www.icq.com/people" : "http://profiles.aim.com";
	tmp = g_strdup_printf("<a href=\"%s/%s\">%s</a>",
			base_profile_url, purple_normalize(account, userinfo->bn), _("View web profile"));
	purple_notify_user_info_add_pair(user_info, NULL, tmp);
	g_free(tmp);
	purple_notify_userinfo(gc, userinfo->bn, user_info, NULL, NULL);
	purple_notify_user_info_destroy(user_info);
}
