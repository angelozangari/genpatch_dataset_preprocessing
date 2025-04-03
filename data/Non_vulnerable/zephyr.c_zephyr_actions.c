}
static GList *zephyr_actions(PurplePlugin *plugin, gpointer context)
{
	GList *list = NULL;
	PurplePluginAction *act = NULL;
	act = purple_plugin_action_new(_("Resubscribe"), zephyr_action_resubscribe);
	list = g_list_append(list, act);
	act = purple_plugin_action_new(_("Retrieve subscriptions from server"), zephyr_action_get_subs_from_server);
	list = g_list_append(list,act);
	return list;
}
