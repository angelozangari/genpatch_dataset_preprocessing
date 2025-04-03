void
oscar_user_info_append_status(PurpleConnection *gc, PurpleNotifyUserInfo *user_info, PurpleBuddy *b, aim_userinfo_t *userinfo, gboolean use_html_status)
{
	PurpleAccount *account = purple_connection_get_account(gc);
	OscarData *od;
	PurplePresence *presence = NULL;
	PurpleStatus *status = NULL;
	gchar *message = NULL, *itmsurl = NULL, *tmp;
	gboolean escaping_needed = TRUE;
	od = purple_connection_get_protocol_data(gc);
	if (b == NULL && userinfo == NULL)
		return;
	if (b == NULL)
		b = purple_find_buddy(purple_connection_get_account(gc), userinfo->bn);
	else
		userinfo = aim_locate_finduserinfo(od, purple_buddy_get_name(b));
	if (b) {
		presence = purple_buddy_get_presence(b);
		status = purple_presence_get_active_status(presence);
	}
	/* If we have both b and userinfo we favor userinfo, because if we're
	   viewing someone's profile then we want the HTML away message, and
	   the "message" attribute of the status contains only the plaintext
	   message. */
	if (userinfo) {
		if ((userinfo->flags & AIM_FLAG_AWAY) && use_html_status && userinfo->away_len > 0 && userinfo->away != NULL && userinfo->away_encoding != NULL) {
			/* Away message */
			message = oscar_encoding_to_utf8(userinfo->away_encoding, userinfo->away, userinfo->away_len);
			escaping_needed = FALSE;
		} else {
			/*
			 * Available message or non-HTML away message (because that's
			 * all we have right now.
			 */
			if ((userinfo->status != NULL) && userinfo->status[0] != '\0') {
				message = oscar_encoding_to_utf8(userinfo->status_encoding, userinfo->status, userinfo->status_len);
			}
#if defined (_WIN32) || defined (__APPLE__)
			if (userinfo->itmsurl && (userinfo->itmsurl[0] != '\0')) {
				itmsurl = oscar_encoding_to_utf8(userinfo->itmsurl_encoding, userinfo->itmsurl, userinfo->itmsurl_len);
			}
#endif
		}
	} else {
		message = g_strdup(purple_status_get_attr_string(status, "message"));
		itmsurl = g_strdup(purple_status_get_attr_string(status, "itmsurl"));
	}
	if (message) {
		tmp = oscar_util_format_string(message, purple_account_get_username(account));
		g_free(message);
		message = tmp;
		if (escaping_needed) {
			tmp = purple_markup_escape_text(message, -1);
			g_free(message);
			message = tmp;
		}
	}
	if (use_html_status && itmsurl) {
		tmp = g_strdup_printf("<a href=\"%s\">%s</a>", itmsurl, message);
		g_free(message);
		message = tmp;
	}
	if (b) {
		if (purple_presence_is_online(presence)) {
			gboolean is_away = ((status && !purple_status_is_available(status)) || (userinfo && (userinfo->flags & AIM_FLAG_AWAY)));
			if (oscar_util_valid_name_icq(purple_buddy_get_name(b)) || is_away || !message || !(*message)) {
				/* Append the status name for online ICQ statuses, away AIM statuses, and for all buddies with no message.
				 * If the status name and the message are the same, only show one. */
				const char *status_name = purple_status_get_name(status);
				if (status_name && message && !strcmp(status_name, message))
					status_name = NULL;
				tmp = g_strdup_printf("%s%s%s",
									   status_name ? status_name : "",
									   ((status_name && message) && *message) ? ": " : "",
									   (message && *message) ? message : "");
				g_free(message);
				message = tmp;
			}
		} else if (aim_ssi_waitingforauth(od->ssi.local,
			aim_ssi_itemlist_findparentname(od->ssi.local, purple_buddy_get_name(b)),
			purple_buddy_get_name(b)))
		{
			/* Note if an offline buddy is not authorized */
			tmp = g_strdup_printf("%s%s%s",
					_("Not Authorized"),
					(message && *message) ? ": " : "",
					(message && *message) ? message : "");
			g_free(message);
			message = tmp;
		} else {
			g_free(message);
			message = g_strdup(_("Offline"));
		}
	}
	if (presence) {
		const char *mood;
		const char *comment;
		char *description;
		status = purple_presence_get_status(presence, "mood");
		mood = icq_get_custom_icon_description(purple_status_get_attr_string(status, PURPLE_MOOD_NAME));
		if (mood) {
			comment = purple_status_get_attr_string(status, PURPLE_MOOD_COMMENT);
			if (comment) {
				char *escaped_comment = purple_markup_escape_text(comment, -1);
				description = g_strdup_printf("%s (%s)", _(mood), escaped_comment);
				g_free(escaped_comment);
			} else {
				description = g_strdup(_(mood));
			}
			purple_notify_user_info_add_pair(user_info, _("Mood"), description);
			g_free(description);
		}
	}
	purple_notify_user_info_add_pair(user_info, _("Status"), message);
	g_free(message);
}
