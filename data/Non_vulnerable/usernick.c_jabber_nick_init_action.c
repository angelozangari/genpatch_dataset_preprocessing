}
void jabber_nick_init_action(GList **m) {
	PurplePluginAction *act = purple_plugin_action_new(_("Set Nickname..."), do_nick_set_nick);
	*m = g_list_append(*m, act);
}
