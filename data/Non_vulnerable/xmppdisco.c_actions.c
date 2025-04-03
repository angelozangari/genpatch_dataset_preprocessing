static GList *
actions(PurplePlugin *plugin, gpointer context)
{
	GList *l = NULL;
	PurplePluginAction *action = NULL;
	action = purple_plugin_action_new(_("XMPP Service Discovery"),
	                                  create_dialog);
	l = g_list_prepend(l, action);
	return l;
}
