/* BLIST DOCKING */
static void blist_save_state() {
	if(blist_ab) {
		if(purple_prefs_get_bool(PREF_DBLIST_DOCKABLE) && blist_ab->docked) {
			purple_prefs_set_int(PREF_DBLIST_HEIGHT, blist_ab->undocked_height);
			purple_prefs_set_int(PREF_DBLIST_SIDE, blist_ab->side);
			purple_prefs_set_bool(PREF_DBLIST_DOCKED, blist_ab->docked);
		} else
			purple_prefs_set_bool(PREF_DBLIST_DOCKED, FALSE);
	}
}
/* BLIST DOCKING */
static void blist_save_state() {
	if(blist_ab) {
		if(purple_prefs_get_bool(PREF_DBLIST_DOCKABLE) && blist_ab->docked) {
			purple_prefs_set_int(PREF_DBLIST_HEIGHT, blist_ab->undocked_height);
			purple_prefs_set_int(PREF_DBLIST_SIDE, blist_ab->side);
			purple_prefs_set_bool(PREF_DBLIST_DOCKED, blist_ab->docked);
		} else
			purple_prefs_set_bool(PREF_DBLIST_DOCKED, FALSE);
	}
}
