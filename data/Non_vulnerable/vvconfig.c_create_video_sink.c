static GstElement *
create_video_sink(PurpleMedia *media,
		const gchar *session_id, const gchar *participant)
{
	const gchar *plugin = purple_prefs_get_string(
			"/plugins/gtk/vvconfig/video/sink/plugin");
	const gchar *device = purple_prefs_get_string(
			"/plugins/gtk/vvconfig/video/sink/device");
	GstElement *ret;
	if (plugin[0] == '\0')
		return purple_media_element_info_call_create(old_video_sink,
				media, session_id, participant);
	ret = gst_element_factory_make(plugin, NULL);
	if (device[0] != '\0')
		g_object_set(G_OBJECT(ret), "device", device, NULL);
	if (g_strcmp0(plugin, "autovideosink") == 0) {
		g_signal_connect(ret, "child-added",
			G_CALLBACK(autovideosink_child_added_cb), NULL);
	} else {
		videosink_disable_last_sample(ret);
	}
	return ret;
}
