/* Listen for the first time the window stops being withdrawn */
static void blist_visible_cb(const char *pref, PurplePrefType type,
		gconstpointer value, gpointer user_data) {
	if(purple_prefs_get_bool(pref)) {
		gtk_appbar_dock(blist_ab,
			purple_prefs_get_int(PREF_DBLIST_SIDE));
		if(purple_prefs_get_int(PREF_BLIST_ON_TOP)
				== BLIST_TOP_DOCKED)
			blist_set_ontop(TRUE);
		/* We only need to be notified about this once */
		purple_prefs_disconnect_callback(blist_visible_cb_id);
	}
}
