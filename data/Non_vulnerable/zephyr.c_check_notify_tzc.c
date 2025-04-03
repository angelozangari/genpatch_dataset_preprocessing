}
static gint check_notify_tzc(gpointer data)
{
	PurpleConnection *gc = (PurpleConnection *)data;
	zephyr_account* zephyr = gc->proto_data;
	parse_tree *newparsetree = read_from_tzc(zephyr);
	if (newparsetree != NULL) {
		gchar *spewtype;
		if ( (spewtype =  tree_child(find_node(newparsetree,"tzcspew"),2)->contents) ) {
			if (!g_ascii_strncasecmp(spewtype,"message",7)) {
				ZNotice_t notice;
				parse_tree *msgnode = tree_child(find_node(newparsetree,"message"),2);
				parse_tree *bodynode = tree_child(msgnode,1);
				/*				char *zsig = g_strdup(" "); */ /* purple doesn't care about zsigs */
				char *msg  = zephyr_tzc_deescape_str(bodynode->contents);
				size_t bufsize = strlen(msg) + 3;
				char *buf = g_new0(char,bufsize);
				g_snprintf(buf,1+strlen(msg)+2," %c%s",'\0',msg);
				memset((char *)&notice, 0, sizeof(notice));
				notice.z_kind = ACKED;
				notice.z_port = 0;
				notice.z_opcode = tree_child(find_node(newparsetree,"opcode"),2)->contents;
				notice.z_class = zephyr_tzc_deescape_str(tree_child(find_node(newparsetree,"class"),2)->contents);
				notice.z_class_inst = tree_child(find_node(newparsetree,"instance"),2)->contents;
				notice.z_recipient = local_zephyr_normalize(zephyr,tree_child(find_node(newparsetree,"recipient"),2)->contents);
				notice.z_sender = local_zephyr_normalize(zephyr,tree_child(find_node(newparsetree,"sender"),2)->contents);
				notice.z_default_format = "Class $class, Instance $instance:\n" "To: @bold($recipient) at $time $date\n" "From: @bold($1) <$sender>\n\n$2";
				notice.z_message_len = strlen(msg) + 3;
				notice.z_message = buf;
				handle_message(gc, &notice);
				g_free(msg);
				/*				  g_free(zsig); */
				g_free(buf);
				/* free_parse_tree(msgnode);
				   free_parse_tree(bodynode);
				   g_free(msg);
				   g_free(zsig);
				   g_free(buf);
				*/
			}
			else if (!g_ascii_strncasecmp(spewtype,"zlocation",9)) {
				/* check_loc or zephyr_zloc respectively */
				/* XXX fix */
				char *user;
				PurpleBuddy *b;
				const char *bname;
				int nlocs = 0;
				parse_tree *locations;
				gchar *locval;
				user = tree_child(find_node(newparsetree,"user"),2)->contents;
				if ((b = purple_find_buddy(gc->account, user)) == NULL) {
					gchar *stripped_user = zephyr_strip_local_realm(zephyr,user);
					b = purple_find_buddy(gc->account, stripped_user);
					g_free(stripped_user);
				}
				locations = find_node(newparsetree,"locations");
				locval = tree_child(tree_child(tree_child(tree_child(locations,2),0),0),2)->contents;
				if (!locval || !g_ascii_strcasecmp(locval," ") || (strlen(locval) == 0)) {
					nlocs = 0;
				} else {
					nlocs = 1;
				}
				bname = b ? purple_buddy_get_name(b) : NULL;
				if ((b && pending_zloc(zephyr,bname)) || pending_zloc(zephyr,user) || pending_zloc(zephyr,local_zephyr_normalize(zephyr,user))){
					PurpleNotifyUserInfo *user_info = purple_notify_user_info_new();
					char *tmp;
					const char *balias;
					purple_notify_user_info_add_pair(user_info, _("User"), (b ? bname : user));
					balias = b ? purple_buddy_get_local_buddy_alias(b) : NULL;
					if (balias)
						purple_notify_user_info_add_pair(user_info, _("Alias"), balias);
					if (!nlocs) {
						purple_notify_user_info_add_pair(user_info, NULL, _("Hidden or not logged-in"));
					} else {
						tmp = g_strdup_printf(_("<br>At %s since %s"),
									  tree_child(tree_child(tree_child(tree_child(locations,2),0),0),2)->contents,
									  tree_child(tree_child(tree_child(tree_child(locations,2),0),2),2)->contents);
						purple_notify_user_info_add_pair(user_info, _("Location"), tmp);
						g_free(tmp);
					}
					purple_notify_userinfo(gc, b ? bname : user,
							     user_info, NULL, NULL);
					purple_notify_user_info_destroy(user_info);
				} else {
					if (nlocs>0)
						purple_prpl_got_user_status(gc->account, b ? bname : user, "available", NULL);
					else
						purple_prpl_got_user_status(gc->account, b ? bname : user, "offline", NULL);
				}
			}
			else if (!g_ascii_strncasecmp(spewtype,"subscribed",10)) {
			}
			else if (!g_ascii_strncasecmp(spewtype,"start",5)) {
			}
			else if (!g_ascii_strncasecmp(spewtype,"error",5)) {
				/* XXX handle */
			}
		} else {
		}
	} else {
	}
	free_parse_tree(newparsetree);
	return TRUE;
}
