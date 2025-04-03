static void
do_got_own_avatar_cb(JabberStream *js, const char *from, xmlnode *items)
{
	xmlnode *item = NULL, *metadata = NULL, *info = NULL;
	PurpleAccount *account = purple_connection_get_account(js->gc);
	const char *server_hash = NULL;
	if (items && (item = xmlnode_get_child(items, "item")) &&
			(metadata = xmlnode_get_child(item, "metadata")) &&
			(info = xmlnode_get_child(metadata, "info"))) {
		server_hash = xmlnode_get_attrib(info, "id");
	}
	/*
	 * If we have an avatar and the server returned an error/malformed data,
	 * push our avatar. If the server avatar doesn't match the local one, push
	 * our avatar.
	 */
	if ((!items || !metadata) ||
			!purple_strequal(server_hash, js->initial_avatar_hash)) {
		PurpleStoredImage *img = purple_buddy_icons_find_account_icon(account);
		jabber_avatar_set(js, img);
		purple_imgstore_unref(img);
	}
}
