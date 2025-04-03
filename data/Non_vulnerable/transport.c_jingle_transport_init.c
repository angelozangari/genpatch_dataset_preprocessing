static void
jingle_transport_init (JingleTransport *transport)
{
	transport->priv = JINGLE_TRANSPORT_GET_PRIVATE(transport);
	transport->priv->dummy = NULL;
}
