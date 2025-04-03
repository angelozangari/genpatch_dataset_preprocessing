static GList *
actions(PurplePlugin *plugin, gpointer context)
{
	GList *l = NULL;
	PurplePluginAction *act = NULL;
	act = purple_plugin_action_new(_("Input and Output Settings"),
		show_config);
	act->user_data = get_plugin_config_frame;
	l = g_list_append(l, act);
	act = purple_plugin_action_new(_("Microphone Test"),
		show_config);
	act->user_data = get_voice_test_frame;
	l = g_list_append(l, act);
	return l;
}
