void
oscar_user_info_display_icq(OscarData *od, struct aim_icq_info *info)
{
	PurpleConnection *gc = od->gc;
	PurpleAccount *account = purple_connection_get_account(gc);
	PurpleBuddy *buddy;
	struct buddyinfo *bi;
	gchar who[16];
	PurpleNotifyUserInfo *user_info;
	if (!info->uin)
		return;
	user_info = purple_notify_user_info_new();
	g_snprintf(who, sizeof(who), "%u", info->uin);
	buddy = purple_find_buddy(account, who);
	if (buddy != NULL)
		bi = g_hash_table_lookup(od->buddyinfo, purple_normalize(account, purple_buddy_get_name(buddy)));
	else
		bi = NULL;
	purple_notify_user_info_add_pair(user_info, _("UIN"), who);
	oscar_user_info_convert_and_add(account, od, user_info, _("Nick"), info->nick);
	if ((bi != NULL) && (bi->ipaddr != 0)) {
		char *tstr =  g_strdup_printf("%u.%u.%u.%u",
			0xFF & ((bi->ipaddr & 0xff000000) >> 24),
			0xFF & ((bi->ipaddr & 0x00ff0000) >> 16),
			0xFF & ((bi->ipaddr & 0x0000ff00) >> 8),
			0xFF & (bi->ipaddr & 0x000000ff));
		purple_notify_user_info_add_pair(user_info, _("IP Address"), tstr);
		g_free(tstr);
	}
	oscar_user_info_convert_and_add(account, od, user_info, _("First Name"), info->first);
	oscar_user_info_convert_and_add(account, od, user_info, _("Last Name"), info->last);
	oscar_user_info_convert_and_add_hyperlink(account, od, user_info, _("Email Address"), info->email, "mailto:");
	if (info->numaddresses && info->email2) {
		int i;
		for (i = 0; i < info->numaddresses; i++) {
			oscar_user_info_convert_and_add_hyperlink(account, od, user_info, _("Email Address"), info->email2[i], "mailto:");
		}
	}
	oscar_user_info_convert_and_add(account, od, user_info, _("Mobile Phone"), info->mobile);
	if (info->gender != 0)
		purple_notify_user_info_add_pair(user_info, _("Gender"), (info->gender == 1 ? _("Female") : _("Male")));
	if ((info->birthyear > 1900) && (info->birthmonth > 0) && (info->birthday > 0)) {
		/* Initialize the struct properly or strftime() will crash
		 * under some conditions (e.g. Debian sarge w/ LANG=en_HK). */
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		tm->tm_mday = (int)info->birthday;
		tm->tm_mon  = (int)info->birthmonth - 1;
		tm->tm_year = (int)info->birthyear - 1900;
		/* Ignore dst setting of today to avoid timezone shift between 
		 * dates in summer and winter time. */
		tm->tm_isdst = -1;
		/* To be 100% sure that the fields are re-normalized.
		 * If you're sure strftime() ALWAYS does this EVERYWHERE,
		 * feel free to remove it.  --rlaager */
		mktime(tm);
		oscar_user_info_convert_and_add(account, od, user_info, _("Birthday"), purple_date_format_short(tm));
	}
	if ((info->age > 0) && (info->age < 255)) {
		char age[5];
		snprintf(age, sizeof(age), "%hhd", info->age);
		purple_notify_user_info_add_pair(user_info, _("Age"), age);
	}
	oscar_user_info_convert_and_add_hyperlink(account, od, user_info, _("Personal Web Page"), info->email, "");
	if (buddy != NULL)
		oscar_user_info_append_status(gc, user_info, buddy, /* aim_userinfo_t */ NULL, /* use_html_status */ TRUE);
	oscar_user_info_convert_and_add(account, od, user_info, _("Additional Information"), info->info);
	purple_notify_user_info_add_section_break(user_info);
	if ((info->homeaddr && (info->homeaddr[0])) || (info->homecity && info->homecity[0]) || (info->homestate && info->homestate[0]) || (info->homezip && info->homezip[0])) {
		purple_notify_user_info_add_section_header(user_info, _("Home Address"));
		oscar_user_info_convert_and_add(account, od, user_info, _("Address"), info->homeaddr);
		oscar_user_info_convert_and_add(account, od, user_info, _("City"), info->homecity);
		oscar_user_info_convert_and_add(account, od, user_info, _("State"), info->homestate);
		oscar_user_info_convert_and_add(account, od, user_info, _("Zip Code"), info->homezip);
	}
	if ((info->workaddr && info->workaddr[0]) || (info->workcity && info->workcity[0]) || (info->workstate && info->workstate[0]) || (info->workzip && info->workzip[0])) {
		purple_notify_user_info_add_section_header(user_info, _("Work Address"));
		oscar_user_info_convert_and_add(account, od, user_info, _("Address"), info->workaddr);
		oscar_user_info_convert_and_add(account, od, user_info, _("City"), info->workcity);
		oscar_user_info_convert_and_add(account, od, user_info, _("State"), info->workstate);
		oscar_user_info_convert_and_add(account, od, user_info, _("Zip Code"), info->workzip);
	}
	if ((info->workcompany && info->workcompany[0]) || (info->workdivision && info->workdivision[0]) || (info->workposition && info->workposition[0]) || (info->workwebpage && info->workwebpage[0])) {
		purple_notify_user_info_add_section_header(user_info, _("Work Information"));
		oscar_user_info_convert_and_add(account, od, user_info, _("Company"), info->workcompany);
		oscar_user_info_convert_and_add(account, od, user_info, _("Division"), info->workdivision);
		oscar_user_info_convert_and_add(account, od, user_info, _("Position"), info->workposition);
		oscar_user_info_convert_and_add_hyperlink(account, od, user_info, _("Web Page"), info->email, "");
	}
	purple_notify_userinfo(gc, who, user_info, NULL, NULL);
	purple_notify_user_info_destroy(user_info);
}
