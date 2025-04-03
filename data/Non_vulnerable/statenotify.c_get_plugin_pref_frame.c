static PurplePluginPrefFrame *
get_plugin_pref_frame(PurplePlugin *plugin)
{
	PurplePluginPrefFrame *frame;
	PurplePluginPref *ppref;
	frame = purple_plugin_pref_frame_new();
	ppref = purple_plugin_pref_new_with_label(_("Notify When"));
	purple_plugin_pref_frame_add(frame, ppref);
	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/statenotify/notify_away", _("Buddy Goes _Away"));
	purple_plugin_pref_frame_add(frame, ppref);
	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/statenotify/notify_idle", _("Buddy Goes _Idle"));
	purple_plugin_pref_frame_add(frame, ppref);
	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/statenotify/notify_signon", _("Buddy _Signs On/Off"));
	purple_plugin_pref_frame_add(frame, ppref);
	return frame;
}
