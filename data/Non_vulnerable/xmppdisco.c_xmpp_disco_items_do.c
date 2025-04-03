static void
xmpp_disco_items_do(PurpleConnection *pc, gpointer cbdata, const char *jid,
                    const char *node, XmppIqCallback cb)
{
	xmlnode *iq, *query;
	char *id = generate_next_id();
	iq = xmlnode_new("iq");
	xmlnode_set_attrib(iq, "type", "get");
	xmlnode_set_attrib(iq, "to", jid);
	xmlnode_set_attrib(iq, "id", id);
	query = xmlnode_new_child(iq, "query");
	xmlnode_set_namespace(query, NS_DISCO_ITEMS);
	if (node)
		xmlnode_set_attrib(query, "node", node);
	/* Steals id */
	xmpp_iq_register_callback(pc, id, cbdata, cb);
	purple_signal_emit(purple_connection_get_prpl(pc), "jabber-sending-xmlnode",
	                   pc, &iq);
	if (iq != NULL)
		xmlnode_free(iq);
}
