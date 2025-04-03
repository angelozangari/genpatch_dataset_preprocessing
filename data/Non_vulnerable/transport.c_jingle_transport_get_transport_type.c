const gchar *
jingle_transport_get_transport_type(JingleTransport *transport)
{
	return JINGLE_TRANSPORT_GET_CLASS(transport)->transport_type;
}
