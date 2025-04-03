}
static void blist_dock_cb(gboolean val) {
	if(val) {
		purple_debug_info(WINPREFS_PLUGIN_ID, "Blist Docking...\n");
		if(purple_prefs_get_int(PREF_BLIST_ON_TOP) != BLIST_TOP_NEVER)
			blist_set_ontop(TRUE);
	} else {
		purple_debug_info(WINPREFS_PLUGIN_ID, "Blist Undocking...\n");
		blist_set_ontop(purple_prefs_get_int(PREF_BLIST_ON_TOP) == BLIST_TOP_ALWAYS);
	}
}
}
static void blist_dock_cb(gboolean val) {
	if(val) {
		purple_debug_info(WINPREFS_PLUGIN_ID, "Blist Docking...\n");
		if(purple_prefs_get_int(PREF_BLIST_ON_TOP) != BLIST_TOP_NEVER)
			blist_set_ontop(TRUE);
	} else {
		purple_debug_info(WINPREFS_PLUGIN_ID, "Blist Undocking...\n");
		blist_set_ontop(purple_prefs_get_int(PREF_BLIST_ON_TOP) == BLIST_TOP_ALWAYS);
	}
}
