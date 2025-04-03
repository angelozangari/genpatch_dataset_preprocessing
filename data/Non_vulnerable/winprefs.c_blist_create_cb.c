}
static void blist_create_cb(PurpleBuddyList *purple_blist, void *data) {
	purple_debug_info(WINPREFS_PLUGIN_ID, "buddy list created\n");
	blist = PIDGIN_BLIST(purple_blist)->window;
	if(purple_prefs_get_bool(PREF_DBLIST_DOCKABLE)) {
		blist_set_dockable(TRUE);
		if(purple_prefs_get_bool(PREF_DBLIST_DOCKED)) {
			blist_ab->undocked_height = purple_prefs_get_int(PREF_DBLIST_HEIGHT);
			if(!(gdk_window_get_state(blist->window)
					& GDK_WINDOW_STATE_WITHDRAWN)) {
				gtk_appbar_dock(blist_ab,
					purple_prefs_get_int(PREF_DBLIST_SIDE));
				if(purple_prefs_get_int(PREF_BLIST_ON_TOP)
						== BLIST_TOP_DOCKED)
					blist_set_ontop(TRUE);
			} else {
				g_idle_add(listen_for_blist_visible_cb, NULL);
			}
		}
	}
	if(purple_prefs_get_int(PREF_BLIST_ON_TOP) == BLIST_TOP_ALWAYS)
		blist_set_ontop(TRUE);
}
