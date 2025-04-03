GType
jingle_transport_get_type()
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof(JingleTransportClass),
			NULL,
			NULL,
			(GClassInitFunc) jingle_transport_class_init,
			NULL,
			NULL,
			sizeof(JingleTransport),
			0,
			(GInstanceInitFunc) jingle_transport_init,
			NULL
		};
		type = g_type_register_static(G_TYPE_OBJECT, "JingleTransport", &info, 0);
	}
	return type;
}
