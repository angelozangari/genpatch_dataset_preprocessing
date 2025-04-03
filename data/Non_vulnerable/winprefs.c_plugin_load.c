 */
static gboolean plugin_load(PurplePlugin *plugin) {
	handle = plugin;
	/* blist docking init */
	if(purple_get_blist() && PIDGIN_BLIST(purple_get_blist())
			&& PIDGIN_BLIST(purple_get_blist())->window) {
		blist_create_cb(purple_get_blist(), NULL);
	}
	/* This really shouldn't happen anymore generally, but if for some strange
	   reason, the blist is recreated, we need to set it up again. */
	purple_signal_connect(pidgin_blist_get_handle(), "gtkblist-created",
		plugin, PURPLE_CALLBACK(blist_create_cb), NULL);
	purple_signal_connect((void*)purple_get_core(), "quitting", plugin,
		PURPLE_CALLBACK(purple_quit_cb), NULL);
	purple_prefs_connect_callback(handle, PREF_BLIST_ON_TOP,
		winprefs_set_blist_ontop, NULL);
	purple_prefs_connect_callback(handle, PREF_DBLIST_DOCKABLE,
		winprefs_set_blist_dockable, NULL);
	return TRUE;
}
