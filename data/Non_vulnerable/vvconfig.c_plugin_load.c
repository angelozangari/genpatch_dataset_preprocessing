static gboolean
plugin_load(PurplePlugin *plugin)
{
	PurpleMediaManager *manager;
	PurpleMediaElementInfo *video_src, *video_sink,
			*audio_src, *audio_sink;
	/* Disable the plugin if the UI doesn't support VV */
	if (purple_media_manager_get_ui_caps(purple_media_manager_get()) ==
			PURPLE_MEDIA_CAPS_NONE)
		return FALSE;
	purple_prefs_add_none("/plugins/core/vvconfig");
	purple_prefs_add_none("/plugins/core/vvconfig/audio");
	purple_prefs_add_none("/plugins/core/vvconfig/audio/src");
	purple_prefs_add_string("/plugins/core/vvconfig/audio/src/plugin", "");
	purple_prefs_add_string("/plugins/core/vvconfig/audio/src/device", "");
	purple_prefs_add_none("/plugins/core/vvconfig/audio/sink");
	purple_prefs_add_string("/plugins/core/vvconfig/audio/sink/plugin", "");
	purple_prefs_add_string("/plugins/core/vvconfig/audio/sink/device", "");
	purple_prefs_add_none("/plugins/core/vvconfig/video");
	purple_prefs_add_none("/plugins/core/vvconfig/video/src");
	purple_prefs_add_string("/plugins/core/vvconfig/video/src/plugin", "");
	purple_prefs_add_string("/plugins/core/vvconfig/video/src/device", "");
	purple_prefs_add_none("/plugins/gtk/vvconfig");
	purple_prefs_add_none("/plugins/gtk/vvconfig/video");
	purple_prefs_add_none("/plugins/gtk/vvconfig/video/sink");
	purple_prefs_add_string("/plugins/gtk/vvconfig/video/sink/plugin", "");
	purple_prefs_add_string("/plugins/gtk/vvconfig/video/sink/device", "");
	video_src = g_object_new(PURPLE_TYPE_MEDIA_ELEMENT_INFO,
			"id", "vvconfig-videosrc",
			"name", "VV Conf Plugin Video Source",
			"type", PURPLE_MEDIA_ELEMENT_VIDEO
					| PURPLE_MEDIA_ELEMENT_SRC
					| PURPLE_MEDIA_ELEMENT_ONE_SRC
					| PURPLE_MEDIA_ELEMENT_UNIQUE,
			"create-cb", create_video_src, NULL);
	video_sink = g_object_new(PURPLE_TYPE_MEDIA_ELEMENT_INFO,
			"id", "vvconfig-videosink",
			"name", "VV Conf Plugin Video Sink",
			"type", PURPLE_MEDIA_ELEMENT_VIDEO
					| PURPLE_MEDIA_ELEMENT_SINK
					| PURPLE_MEDIA_ELEMENT_ONE_SINK,
			"create-cb", create_video_sink, NULL);
	audio_src = g_object_new(PURPLE_TYPE_MEDIA_ELEMENT_INFO,
			"id", "vvconfig-audiosrc",
			"name", "VV Conf Plugin Audio Source",
			"type", PURPLE_MEDIA_ELEMENT_AUDIO
					| PURPLE_MEDIA_ELEMENT_SRC
					| PURPLE_MEDIA_ELEMENT_ONE_SRC
					| PURPLE_MEDIA_ELEMENT_UNIQUE,
			"create-cb", create_audio_src, NULL);
	audio_sink = g_object_new(PURPLE_TYPE_MEDIA_ELEMENT_INFO,
			"id", "vvconfig-audiosink",
			"name", "VV Conf Plugin Audio Sink",
			"type", PURPLE_MEDIA_ELEMENT_AUDIO
					| PURPLE_MEDIA_ELEMENT_SINK
					| PURPLE_MEDIA_ELEMENT_ONE_SINK,
			"create-cb", create_audio_sink, NULL);
	purple_debug_info("gtkmedia", "Registering media element types\n");
	manager = purple_media_manager_get();
	old_video_src = purple_media_manager_get_active_element(manager,
			PURPLE_MEDIA_ELEMENT_VIDEO | PURPLE_MEDIA_ELEMENT_SRC);
	old_video_sink = purple_media_manager_get_active_element(manager,
			PURPLE_MEDIA_ELEMENT_VIDEO | PURPLE_MEDIA_ELEMENT_SINK);
	old_audio_src = purple_media_manager_get_active_element(manager,
			PURPLE_MEDIA_ELEMENT_AUDIO | PURPLE_MEDIA_ELEMENT_SRC);
	old_audio_sink = purple_media_manager_get_active_element(manager,
			PURPLE_MEDIA_ELEMENT_AUDIO | PURPLE_MEDIA_ELEMENT_SINK);
	set_element_info_cond(old_video_src, video_src, "pidgindefaultvideosrc");
	set_element_info_cond(old_video_sink, video_sink, "pidgindefaultvideosink");
	set_element_info_cond(old_audio_src, audio_src, "pidgindefaultaudiosrc");
	set_element_info_cond(old_audio_sink, audio_sink, "pidgindefaultaudiosink");
	return TRUE;
}
