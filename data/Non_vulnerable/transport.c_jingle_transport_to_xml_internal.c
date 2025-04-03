xmlnode *
jingle_transport_to_xml_internal(JingleTransport *transport, xmlnode *content, JingleActionType action)
{
	xmlnode *node = xmlnode_new_child(content, "transport");
	xmlnode_set_namespace(node, jingle_transport_get_transport_type(transport));
	return node;
}
