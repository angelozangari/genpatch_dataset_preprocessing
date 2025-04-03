static void
videosink_disable_last_sample(GstElement *sink)
{
	GObjectClass *klass = G_OBJECT_GET_CLASS(sink);
	if (g_object_class_find_property(klass, "enable-last-sample")) {
		g_object_set(sink, "enable-last-sample", FALSE, NULL);
	}
}
