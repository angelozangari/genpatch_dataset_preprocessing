static GList *
actions(PurplePlugin *plugin, gpointer context)
{
	GList *l = NULL;
	PurplePluginAction *act = NULL;
	act = purple_plugin_action_new(_("Edit Buddylist Theme"), pidgin_blist_theme_edit);
	l = g_list_append(l, act);
	act = purple_plugin_action_new(_("Edit Icon Theme"), pidgin_icon_theme_edit);
	l = g_list_append(l, act);
	return l;
}
