static PurplePluginPrefFrame *
get_plugin_pref_frame(PurplePlugin *plugin)
{
	PurplePluginPrefFrame *frame;
	PurplePluginPref *ppref;
	char *tmp;
	frame = purple_plugin_pref_frame_new();
	ppref = purple_plugin_pref_new_with_label(_("Timestamp Format Options"));
	purple_plugin_pref_frame_add(frame, ppref);
	tmp = g_strdup(_("_Force timestamp format:"));
	ppref = purple_plugin_pref_new_with_name_and_label(
			"/plugins/gtk/timestamp_format/force",
			tmp);
	purple_plugin_pref_set_type(ppref, PURPLE_PLUGIN_PREF_CHOICE);
	purple_plugin_pref_add_choice(ppref, _("Use system default"), "default");
	purple_plugin_pref_add_choice(ppref, _("12 hour time format"), "force12");
	purple_plugin_pref_add_choice(ppref, _("24 hour time format"), "force24");
	purple_plugin_pref_frame_add(frame, ppref);
	g_free(tmp);
	ppref = purple_plugin_pref_new_with_label(_("Show dates in..."));
	purple_plugin_pref_frame_add(frame, ppref);
	ppref = purple_plugin_pref_new_with_name_and_label(
			"/plugins/gtk/timestamp_format/use_dates/conversation",
			_("Co_nversations:"));
        purple_plugin_pref_set_type(ppref, PURPLE_PLUGIN_PREF_CHOICE);
        purple_plugin_pref_add_choice(ppref, _("For delayed messages"), "automatic");
        purple_plugin_pref_add_choice(ppref, _("For delayed messages and in chats"), "chats");
        purple_plugin_pref_add_choice(ppref, _("Always"), "always");
	purple_plugin_pref_frame_add(frame, ppref);
	ppref = purple_plugin_pref_new_with_name_and_label(
			"/plugins/gtk/timestamp_format/use_dates/log",
			_("_Message Logs:"));
        purple_plugin_pref_set_type(ppref, PURPLE_PLUGIN_PREF_CHOICE);
        purple_plugin_pref_add_choice(ppref, _("For delayed messages"), "automatic");
        purple_plugin_pref_add_choice(ppref, _("For delayed messages and in chats"), "chats");
        purple_plugin_pref_add_choice(ppref, _("Always"), "always");
	purple_plugin_pref_frame_add(frame, ppref);
	return frame;
}
