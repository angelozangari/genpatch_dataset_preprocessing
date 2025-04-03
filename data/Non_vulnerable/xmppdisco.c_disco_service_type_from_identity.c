static XmppDiscoServiceType
disco_service_type_from_identity(xmlnode *identity)
{
	const char *category, *type;
	if (!identity)
		return XMPP_DISCO_SERVICE_TYPE_OTHER;
	category = xmlnode_get_attrib(identity, "category");
	type = xmlnode_get_attrib(identity, "type");
	if (!category)
		return XMPP_DISCO_SERVICE_TYPE_OTHER;
	if (g_str_equal(category, "conference"))
		return XMPP_DISCO_SERVICE_TYPE_CHAT;
	else if (g_str_equal(category, "directory"))
		return XMPP_DISCO_SERVICE_TYPE_DIRECTORY;
	else if (g_str_equal(category, "gateway"))
		return XMPP_DISCO_SERVICE_TYPE_GATEWAY;
	else if (g_str_equal(category, "pubsub")) {
		if (!type || g_str_equal(type, "collection"))
			return XMPP_DISCO_SERVICE_TYPE_PUBSUB_COLLECTION;
		else if (g_str_equal(type, "leaf"))
			return XMPP_DISCO_SERVICE_TYPE_PUBSUB_LEAF;
		else if (g_str_equal(type, "service"))
			return XMPP_DISCO_SERVICE_TYPE_OTHER;
		else {
			purple_debug_warning("xmppdisco", "Unknown pubsub type '%s'\n", type);
			return XMPP_DISCO_SERVICE_TYPE_OTHER;
		}
	}
	return XMPP_DISCO_SERVICE_TYPE_OTHER;
}
