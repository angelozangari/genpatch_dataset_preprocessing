static void
do_buddy_avatar_update_fromurl(PurpleUtilFetchUrlData *url_data,
                               gpointer user_data, const gchar *url_text,
                               gsize len, const gchar *error_message)
{
	JabberBuddyAvatarUpdateURLInfo *info = user_data;
	gpointer icon_data;
	if(!url_text) {
		purple_debug_error("jabber",
		             "do_buddy_avatar_update_fromurl got error \"%s\"",
		             error_message);
		goto out;
	}
	icon_data = g_memdup(url_text, len);
	purple_buddy_icons_set_for_user(purple_connection_get_account(info->js->gc), info->from, icon_data, len, info->id);
out:
	g_free(info->from);
	g_free(info->id);
	g_free(info);
}
