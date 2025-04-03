 * if we're only transparent when unfocused, deal with transparency */
static gboolean focus_blist_win_cb(GtkWidget *w, GdkEventFocus *e, gpointer d) {
	if (purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED)
			&& purple_prefs_get_bool(OPT_WINTRANS_BL_ONFOCUS)) {
		GtkWidget *window = (GtkWidget *) d;
		if (e->in) { /* Focused */
			set_wintrans(window, 0, FALSE,
				purple_prefs_get_bool(OPT_WINTRANS_BL_ONTOP));
		} else {
			set_wintrans(window,
				purple_prefs_get_int(OPT_WINTRANS_BL_ALPHA),
				TRUE,
				purple_prefs_get_bool(OPT_WINTRANS_BL_ONTOP));
		}
	}
	return FALSE;
}
 * if we're only transparent when unfocused, deal with transparency */
static gboolean focus_blist_win_cb(GtkWidget *w, GdkEventFocus *e, gpointer d) {
	if (purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED)
			&& purple_prefs_get_bool(OPT_WINTRANS_BL_ONFOCUS)) {
		GtkWidget *window = (GtkWidget *) d;
		if (e->in) { /* Focused */
			set_wintrans(window, 0, FALSE,
				purple_prefs_get_bool(OPT_WINTRANS_BL_ONTOP));
		} else {
			set_wintrans(window,
				purple_prefs_get_int(OPT_WINTRANS_BL_ALPHA),
				TRUE,
				purple_prefs_get_bool(OPT_WINTRANS_BL_ONTOP));
		}
	}
	return FALSE;
}
