static void
blist_created_cb(PurpleBuddyList *purple_blist, gpointer data) {
	if (blist) {
		if (purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED)) {
			set_wintrans(blist,
				purple_prefs_get_int(OPT_WINTRANS_BL_ALPHA),
				TRUE,
				purple_prefs_get_bool(OPT_WINTRANS_BL_ONTOP));
		}
		g_signal_connect(G_OBJECT(blist), "focus_in_event",
			G_CALLBACK(focus_blist_win_cb), blist);
		g_signal_connect(G_OBJECT(blist), "focus_out_event",
			G_CALLBACK(focus_blist_win_cb), blist);
	}
}
static void
blist_created_cb(PurpleBuddyList *purple_blist, gpointer data) {
	if (blist) {
		if (purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED)) {
			set_wintrans(blist,
				purple_prefs_get_int(OPT_WINTRANS_BL_ALPHA),
				TRUE,
				purple_prefs_get_bool(OPT_WINTRANS_BL_ONTOP));
		}
		g_signal_connect(G_OBJECT(blist), "focus_in_event",
			G_CALLBACK(focus_blist_win_cb), blist);
		g_signal_connect(G_OBJECT(blist), "focus_out_event",
			G_CALLBACK(focus_blist_win_cb), blist);
	}
}
