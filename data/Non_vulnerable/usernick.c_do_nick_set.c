}
static void do_nick_set(JabberStream *js, const char *nick) {
	xmlnode *publish, *nicknode;
	publish = xmlnode_new("publish");
	xmlnode_set_attrib(publish,"node","http://jabber.org/protocol/nick");
	nicknode = xmlnode_new_child(xmlnode_new_child(publish, "item"), "nick");
	xmlnode_set_namespace(nicknode, "http://jabber.org/protocol/nick");
	if(nick && nick[0] != '\0')
		xmlnode_insert_data(nicknode, nick, -1);
	jabber_pep_publish(js, publish);
	/* publish is freed by jabber_pep_publish -> jabber_iq_send -> jabber_iq_free
		(yay for well-defined memory management rules) */
}
