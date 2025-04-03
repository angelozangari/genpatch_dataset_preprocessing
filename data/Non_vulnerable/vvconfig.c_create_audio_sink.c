static GstElement *
create_audio_sink(PurpleMedia *media,
		const gchar *session_id, const gchar *participant)
{
	const gchar *plugin = purple_prefs_get_string(
			"/plugins/core/vvconfig/audio/sink/plugin");
	const gchar *device = purple_prefs_get_string(
			"/plugins/core/vvconfig/audio/sink/device");
	GstElement *ret;
	if (plugin[0] == '\0')
		return purple_media_element_info_call_create(old_audio_sink,
				media, session_id, participant);
	ret = gst_element_factory_make(plugin, NULL);
	if (device[0] != '\0')
		g_object_set(G_OBJECT(ret), "device", device, NULL);
	return ret;
}
