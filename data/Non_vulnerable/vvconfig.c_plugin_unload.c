static gboolean
plugin_unload(PurplePlugin *plugin)
{
	PurpleMediaManager *manager = purple_media_manager_get();
	purple_media_manager_set_active_element(manager, old_video_src);
	purple_media_manager_set_active_element(manager, old_video_sink);
	purple_media_manager_set_active_element(manager, old_audio_src);
	purple_media_manager_set_active_element(manager, old_audio_sink);
	return TRUE;
}
