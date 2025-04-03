JingleTransport *
jingle_transport_create(const gchar *type)
{
	return g_object_new(jingle_get_type(type), NULL);
}
