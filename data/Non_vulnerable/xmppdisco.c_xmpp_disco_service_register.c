}
void xmpp_disco_service_register(XmppDiscoService *service)
{
	xmlnode *iq, *query;
	char *id = generate_next_id();
	iq = xmlnode_new("iq");
	xmlnode_set_attrib(iq, "type", "get");
	xmlnode_set_attrib(iq, "to", service->jid);
	xmlnode_set_attrib(iq, "id", id);
	query = xmlnode_new_child(iq, "query");
	xmlnode_set_namespace(query, NS_REGISTER);
	purple_signal_emit(purple_connection_get_prpl(service->list->pc),
			"jabber-sending-xmlnode", service->list->pc, &iq);
	if (iq != NULL)
		xmlnode_free(iq);
	g_free(id);
}
