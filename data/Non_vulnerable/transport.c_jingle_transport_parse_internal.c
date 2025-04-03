JingleTransport *
jingle_transport_parse_internal(xmlnode *transport)
{
	const gchar *type = xmlnode_get_namespace(transport);
	return jingle_transport_create(type);
}
