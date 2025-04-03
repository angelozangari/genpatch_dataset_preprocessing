static GList *
actions(PurplePlugin *plugin, gpointer context)
{
	GList *l = NULL;
	PurplePluginAction *act = NULL;
	act = purple_plugin_action_new(_("XMPP Console"), create_console);
	l = g_list_append(l, act);
	return l;
}
