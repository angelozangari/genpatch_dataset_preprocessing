JingleTransport *
jingle_transport_parse(xmlnode *transport)
{
	const gchar *type_name = xmlnode_get_namespace(transport);
	GType type = jingle_get_type(type_name);
	if (type == G_TYPE_NONE)
		return NULL;
	return JINGLE_TRANSPORT_CLASS(g_type_class_ref(type))->parse(transport);
}
