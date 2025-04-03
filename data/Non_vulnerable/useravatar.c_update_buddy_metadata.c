static void
update_buddy_metadata(JabberStream *js, const char *from, xmlnode *items)
{
	PurpleBuddy *buddy = purple_find_buddy(purple_connection_get_account(js->gc), from);
	const char *checksum;
	xmlnode *item, *metadata;
	if(!buddy)
		return;
	if (!items)
		return;
	item = xmlnode_get_child(items,"item");
	if (!item)
		return;
	metadata = xmlnode_get_child(item, "metadata");
	if(!metadata)
		return;
	checksum = purple_buddy_icons_get_checksum_for_user(buddy);
	/* <stop/> was the pre-v1.1 method of publishing an empty avatar */
	if(xmlnode_get_child(metadata, "stop")) {
		purple_buddy_icons_set_for_user(purple_connection_get_account(js->gc), from, NULL, 0, NULL);
	} else {
		xmlnode *info, *goodinfo = NULL;
		gboolean has_children = FALSE;
		/* iterate over all info nodes to get one we can use */
		for(info = metadata->child; info; info = info->next) {
			if(info->type == XMLNODE_TYPE_TAG)
				has_children = TRUE;
			if(info->type == XMLNODE_TYPE_TAG && !strcmp(info->name,"info")) {
				const char *type = xmlnode_get_attrib(info,"type");
				const char *id = xmlnode_get_attrib(info,"id");
				if(checksum && id && !strcmp(id, checksum)) {
					/* we already have that avatar, so we don't have to do anything */
					goodinfo = NULL;
					break;
				}
				/* We'll only pick the png one for now. It's a very nice image format anyways. */
				if(type && id && !goodinfo && !strcmp(type, "image/png"))
					goodinfo = info;
			}
		}
		if(has_children == FALSE) {
			purple_buddy_icons_set_for_user(purple_connection_get_account(js->gc), from, NULL, 0, NULL);
		} else if(goodinfo) {
			const char *url = xmlnode_get_attrib(goodinfo, "url");
			const char *id = xmlnode_get_attrib(goodinfo,"id");
			/* the avatar might either be stored in a pep node, or on a HTTP(S) URL */
			if(!url) {
				jabber_pep_request_item(js, from, NS_AVATAR_1_1_DATA, id,
				                        do_buddy_avatar_update_data);
			} else {
				PurpleUtilFetchUrlData *url_data;
				JabberBuddyAvatarUpdateURLInfo *info = g_new0(JabberBuddyAvatarUpdateURLInfo, 1);
				info->js = js;
				url_data = purple_util_fetch_url_len(url, TRUE, NULL, TRUE,
										  MAX_HTTP_BUDDYICON_BYTES,
										  do_buddy_avatar_update_fromurl, info);
				if (url_data) {
					info->from = g_strdup(from);
					info->id = g_strdup(id);
					js->url_datas = g_slist_prepend(js->url_datas, url_data);
				} else
					g_free(info);
			}
		}
	}
}
