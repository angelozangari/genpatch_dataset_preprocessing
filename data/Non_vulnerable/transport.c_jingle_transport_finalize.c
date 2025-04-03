static void
jingle_transport_finalize (GObject *transport)
{
	/* JingleTransportPrivate *priv = JINGLE_TRANSPORT_GET_PRIVATE(transport); */
	purple_debug_info("jingle","jingle_transport_finalize\n");
	parent_class->finalize(transport);
}
