void
oscar_user_info_append_extra_info(PurpleConnection *gc, PurpleNotifyUserInfo *user_info, PurpleBuddy *b, aim_userinfo_t *userinfo)
{
	OscarData *od;
	PurpleAccount *account;
	PurpleGroup *g = NULL;
	struct buddyinfo *bi = NULL;
	char *tmp;
	const char *bname = NULL, *gname = NULL;
	od = purple_connection_get_protocol_data(gc);
	account = purple_connection_get_account(gc);
	if ((user_info == NULL) || ((b == NULL) && (userinfo == NULL)))
		return;
	if (userinfo == NULL)
		userinfo = aim_locate_finduserinfo(od, purple_buddy_get_name(b));
	if (b == NULL)
		b = purple_find_buddy(account, userinfo->bn);
	if (b != NULL) {
		bname = purple_buddy_get_name(b);
		g = purple_buddy_get_group(b);
		gname = purple_group_get_name(g);
	}
	if (userinfo != NULL)
		bi = g_hash_table_lookup(od->buddyinfo, purple_normalize(account, userinfo->bn));
	if ((bi != NULL) && (bi->ipaddr != 0)) {
		tmp =  g_strdup_printf("%u.%u.%u.%u",
			0xFF & ((bi->ipaddr & 0xff000000) >> 24),
			0xFF & ((bi->ipaddr & 0x00ff0000) >> 16),
			0xFF & ((bi->ipaddr & 0x0000ff00) >> 8),
			0xFF & (bi->ipaddr & 0x000000ff));
		oscar_user_info_add_pair(user_info, _("IP Address"), tmp);
		g_free(tmp);
	}
	if ((userinfo != NULL) && (userinfo->warnlevel != 0)) {
		tmp = g_strdup_printf("%d", (int)(userinfo->warnlevel/10.0 + .5));
		oscar_user_info_add_pair(user_info, _("Warning Level"), tmp);
		g_free(tmp);
	}
	if ((b != NULL) && (bname != NULL) && (g != NULL) && (gname != NULL)) {
		tmp = aim_ssi_getcomment(od->ssi.local, gname, bname);
		if (tmp != NULL) {
			char *tmp2 = g_markup_escape_text(tmp, strlen(tmp));
			g_free(tmp);
			oscar_user_info_convert_and_add(account, od, user_info, _("Buddy Comment"), tmp2);
			g_free(tmp2);
		}
	}
}
