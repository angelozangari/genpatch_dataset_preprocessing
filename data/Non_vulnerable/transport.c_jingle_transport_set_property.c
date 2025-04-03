static void
jingle_transport_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	g_return_if_fail(object != NULL);
	g_return_if_fail(JINGLE_IS_TRANSPORT(object));
	switch (prop_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}
