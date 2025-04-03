static void
autovideosink_child_added_cb(GstChildProxy *child_proxy, GObject *object,
#if GST_CHECK_VERSION(1,0,0)
		gchar *name,
#endif
		gpointer user_data)
{
	videosink_disable_last_sample(GST_ELEMENT(object));
}
