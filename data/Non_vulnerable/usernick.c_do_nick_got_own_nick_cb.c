}
static void do_nick_got_own_nick_cb(JabberStream *js, const char *from, xmlnode *items) {
	char *oldnickname = NULL;
	xmlnode *item = NULL;
	if (items)
		item = xmlnode_get_child(items,"item");
	if(item) {
		xmlnode *nick = xmlnode_get_child_with_namespace(item,"nick","http://jabber.org/protocol/nick");
		if(nick)
			oldnickname = xmlnode_get_data(nick);
	}
	purple_request_input(js->gc, _("Set User Nickname"), _("Please specify a new nickname for you."),
		_("This information is visible to all contacts on your contact list, so choose something appropriate."),
		oldnickname, FALSE, FALSE, NULL, _("Set"), PURPLE_CALLBACK(do_nick_set), _("Cancel"), NULL,
		purple_connection_get_account(js->gc), NULL, NULL, js);
	g_free(oldnickname);
}
