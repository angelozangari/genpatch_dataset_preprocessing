static void
jingle_transport_class_init (JingleTransportClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass*)klass;
	parent_class = g_type_class_peek_parent(klass);
	gobject_class->finalize = jingle_transport_finalize;
	gobject_class->set_property = jingle_transport_set_property;
	gobject_class->get_property = jingle_transport_get_property;
	klass->to_xml = jingle_transport_to_xml_internal;
	klass->parse = jingle_transport_parse_internal;
	g_type_class_add_private(klass, sizeof(JingleTransportPrivate));
}
