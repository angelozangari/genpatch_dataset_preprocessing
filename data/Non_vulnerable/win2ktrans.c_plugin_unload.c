}
static gboolean plugin_unload(PurplePlugin *plugin) {
	purple_debug_info(WINTRANS_PLUGIN_ID, "Unloading win2ktrans plugin\n");
	remove_convs_wintrans(TRUE);
	if (blist) {
		if (purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED))
			set_wintrans(blist, 0, FALSE, FALSE);
		/* Remove the focus cbs */
		g_signal_handlers_disconnect_by_func(G_OBJECT(blist),
			G_CALLBACK(focus_blist_win_cb), blist);
	}
	return TRUE;
}
