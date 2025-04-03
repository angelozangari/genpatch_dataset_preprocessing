xmlnode *
jingle_transport_to_xml(JingleTransport *transport, xmlnode *content, JingleActionType action)
{
	g_return_val_if_fail(transport != NULL, NULL);
	g_return_val_if_fail(JINGLE_IS_TRANSPORT(transport), NULL);
	return JINGLE_TRANSPORT_GET_CLASS(transport)->to_xml(transport, content, action);
}
