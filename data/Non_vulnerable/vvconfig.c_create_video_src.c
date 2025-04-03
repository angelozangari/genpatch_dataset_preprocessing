static GstElement *
create_video_src(PurpleMedia *media,
		const gchar *session_id, const gchar *participant)
{
	const gchar *plugin = purple_prefs_get_string(
			"/plugins/core/vvconfig/video/src/plugin");
	const gchar *device = purple_prefs_get_string(
			"/plugins/core/vvconfig/video/src/device");
	GstElement *ret;
	if (plugin[0] == '\0')
		return purple_media_element_info_call_create(old_video_src,
				media, session_id, participant);
	ret = gst_element_factory_make(plugin, "vvconfig-videosrc");
	if (device[0] != '\0')
		g_object_set(G_OBJECT(ret), "device", device, NULL);
	if (!strcmp(plugin, "videotestsrc"))
		g_object_set(G_OBJECT(ret), "is-live", 1, NULL);
	return ret;
}
