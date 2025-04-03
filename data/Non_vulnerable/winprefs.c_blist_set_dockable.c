}
static void blist_set_dockable(gboolean val) {
	if(val) {
		if(blist_ab == NULL && blist != NULL) {
			blist_ab = gtk_appbar_add(blist);
			gtk_appbar_add_dock_cb(blist_ab, blist_dock_cb);
		}
	} else {
		if(blist_ab != NULL) {
			gtk_appbar_remove(blist_ab);
			blist_ab = NULL;
		}
		blist_set_ontop(purple_prefs_get_int(PREF_BLIST_ON_TOP) == BLIST_TOP_ALWAYS);
	}
}
}
static void blist_set_dockable(gboolean val) {
	if(val) {
		if(blist_ab == NULL && blist != NULL) {
			blist_ab = gtk_appbar_add(blist);
			gtk_appbar_add_dock_cb(blist_ab, blist_dock_cb);
		}
	} else {
		if(blist_ab != NULL) {
			gtk_appbar_remove(blist_ab);
			blist_ab = NULL;
		}
		blist_set_ontop(purple_prefs_get_int(PREF_BLIST_ON_TOP) == BLIST_TOP_ALWAYS);
	}
}
