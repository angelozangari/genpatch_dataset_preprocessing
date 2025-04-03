}
static void do_nick_set_nick(PurplePluginAction *action) {
	PurpleConnection *gc = action->context;
	JabberStream *js = purple_connection_get_protocol_data(gc);
	/* since the nickname might have been changed by another resource of this account, we always have to request the old one
		from the server to present as the default for the new one */
	jabber_pep_request_item(js, NULL, "http://jabber.org/protocol/nick", NULL, do_nick_got_own_nick_cb);
}
