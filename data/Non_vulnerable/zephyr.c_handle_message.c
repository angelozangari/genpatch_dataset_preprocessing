}
static void handle_message(PurpleConnection *gc, ZNotice_t *notice_p)
{
	ZNotice_t notice;
	zephyr_account* zephyr = gc->proto_data;
	memcpy(&notice, notice_p, sizeof(notice)); /* TODO - use pointer? */
	if (!g_ascii_strcasecmp(notice.z_class, LOGIN_CLASS)) {
		/* well, we'll be updating in 20 seconds anyway, might as well ignore this. */
	} else if (!g_ascii_strcasecmp(notice.z_class, LOCATE_CLASS)) {
		if (!g_ascii_strcasecmp(notice.z_opcode, LOCATE_LOCATE)) {
			int nlocs;
			char *user;
			PurpleBuddy *b;
			const char *bname;
			/* XXX add real error reporting */
			if (ZParseLocations(&notice, NULL, &nlocs, &user) != ZERR_NONE)
				return;
			if ((b = purple_find_buddy(gc->account, user)) == NULL) {
				char* stripped_user = zephyr_strip_local_realm(zephyr,user);
				b = purple_find_buddy(gc->account,stripped_user);
				g_free(stripped_user);
			}
			bname = b ? purple_buddy_get_name(b) : NULL;
			if ((b && pending_zloc(zephyr,bname)) || pending_zloc(zephyr,user)) {
				ZLocations_t locs;
				int one = 1;
				PurpleNotifyUserInfo *user_info = purple_notify_user_info_new();
				char *tmp;
				const char *balias;
				purple_notify_user_info_add_pair(user_info, _("User"), (b ? bname : user));
				balias = purple_buddy_get_local_buddy_alias(b);
				if (b && balias)
					purple_notify_user_info_add_pair(user_info, _("Alias"), balias);
				if (!nlocs) {
					purple_notify_user_info_add_pair(user_info, NULL, _("Hidden or not logged-in"));
				}
				for (; nlocs > 0; nlocs--) {
					/* XXX add real error reporting */
					ZGetLocations(&locs, &one);
					tmp = g_strdup_printf(_("<br>At %s since %s"), locs.host, locs.time);
					purple_notify_user_info_add_pair(user_info, _("Location"), tmp);
					g_free(tmp);
				}
				purple_notify_userinfo(gc, (b ? bname : user),
						     user_info, NULL, NULL);
				purple_notify_user_info_destroy(user_info);
			} else {
				if (nlocs>0)
					purple_prpl_got_user_status(gc->account, b ? bname : user, "available", NULL);
				else
					purple_prpl_got_user_status(gc->account, b ? bname : user, "offline", NULL);
			}
			g_free(user);
		}
	} else {
		char *buf, *buf2, *buf3;
		char *send_inst;
		PurpleConversation *gconv1;
		PurpleConvChat *gcc;
		char *ptr = (char *) notice.z_message + (strlen(notice.z_message) + 1);
		int len;
		char *stripped_sender;
		int signature_length = strlen(notice.z_message);
		PurpleMessageFlags flags = 0;
		gchar *tmpescape;
		/* Need to deal with 0 length  messages to handle typing notification (OPCODE) ping messages */
		/* One field zephyrs would have caused purple to crash */
		if ( (notice.z_message_len == 0) || (signature_length >= notice.z_message_len - 1)) {
			len = 0;
			purple_debug_info("zephyr","message_size %d %d %d\n",len,notice.z_message_len,signature_length);
			buf3 = g_strdup("");
		} else {
			len =  notice.z_message_len - ( signature_length +1);
			purple_debug_info("zephyr","message_size %d %d %d\n",len,notice.z_message_len,signature_length);
			buf = g_malloc(len + 1);
			g_snprintf(buf, len + 1, "%s", ptr);
			g_strchomp(buf);
			tmpescape = g_markup_escape_text(buf, -1);
			g_free(buf);
			buf2 = zephyr_to_html(tmpescape);
			buf3 = zephyr_recv_convert(gc, buf2);
			g_free(buf2);
			g_free(tmpescape);
		}
		stripped_sender = zephyr_strip_local_realm(zephyr,notice.z_sender);
		if (!g_ascii_strcasecmp(notice.z_class, "MESSAGE") && !g_ascii_strcasecmp(notice.z_class_inst, "PERSONAL")
		    && !g_ascii_strcasecmp(notice.z_recipient,zephyr->username)) {
			if (!g_ascii_strcasecmp(notice.z_message, "Automated reply:"))
				flags |= PURPLE_MESSAGE_AUTO_RESP;
			if (!g_ascii_strcasecmp(notice.z_opcode,"PING"))
				serv_got_typing(gc,stripped_sender,ZEPHYR_TYPING_RECV_TIMEOUT, PURPLE_TYPING);
			else
				serv_got_im(gc, stripped_sender, buf3, flags, time(NULL));
		} else {
			zephyr_triple *zt1, *zt2;
			gchar *send_inst_utf8;
			zephyr_account *zephyr = gc->proto_data;
			zt1 = new_triple(zephyr,notice.z_class, notice.z_class_inst, notice.z_recipient);
			zt2 = find_sub_by_triple(zephyr,zt1);
			if (!zt2) {
				/* This is a server supplied subscription */
				zephyr->subscrips = g_slist_append(zephyr->subscrips, new_triple(zephyr,zt1->class,zt1->instance,zt1->recipient));
				zt2 = find_sub_by_triple(zephyr,zt1);
			}
			if (!zt2->open) {
				zt2->open = TRUE;
				serv_got_joined_chat(gc, zt2->id, zt2->name);
				zephyr_chat_set_topic(gc,zt2->id,notice.z_class_inst);
			}
			if (!g_ascii_strcasecmp(notice.z_class_inst,"PERSONAL"))
				send_inst_utf8 = g_strdup(stripped_sender);
			else {
				send_inst = g_strdup_printf("[%s] %s",notice.z_class_inst,stripped_sender);
				send_inst_utf8 = zephyr_recv_convert(gc,send_inst);
				g_free(send_inst);
				if (!send_inst_utf8) {
					purple_debug_error("zephyr","Failed to convert instance for sender %s.\n", stripped_sender);
					send_inst_utf8 = g_strdup(stripped_sender);
				}
			}
			gconv1 = purple_find_conversation_with_account(PURPLE_CONV_TYPE_CHAT,
														 zt2->name, gc->account);
			gcc = purple_conversation_get_chat_data(gconv1);
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif
			if (!purple_conv_chat_find_user(gcc, stripped_sender)) {
				gchar ipaddr[INET_ADDRSTRLEN];
#ifdef HAVE_INET_NTOP
				inet_ntop(AF_INET, &notice.z_sender_addr.s_addr, ipaddr, sizeof(ipaddr));
#else
				memcpy(ipaddr,inet_ntoa(notice.z_sender_addr),sizeof(ipaddr));
#endif
				purple_conv_chat_add_user(gcc, stripped_sender, ipaddr, PURPLE_CBFLAGS_NONE, TRUE);
			}
			serv_got_chat_in(gc, zt2->id, send_inst_utf8, 0, buf3, time(NULL));
			g_free(send_inst_utf8);
			free_triple(zt1);
		}
		g_free(stripped_sender);
		g_free(buf3);
	}
}
