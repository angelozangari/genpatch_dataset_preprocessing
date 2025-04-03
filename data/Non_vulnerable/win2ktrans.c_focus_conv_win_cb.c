 * deal with transparency */
static gboolean focus_conv_win_cb(GtkWidget *w, GdkEventFocus *e, gpointer d) {
	if (purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED)
			&& purple_prefs_get_bool(OPT_WINTRANS_IM_ONFOCUS)) {
		GtkWidget *window = (GtkWidget *) d;
		if (e->in) { /* Focused */
			set_wintrans(window, 0, FALSE,
				purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
		} else {
			set_wintrans(window,
				purple_prefs_get_int(OPT_WINTRANS_IM_ALPHA),
				TRUE,
				purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
		}
	}
	return FALSE;
}
 * deal with transparency */
static gboolean focus_conv_win_cb(GtkWidget *w, GdkEventFocus *e, gpointer d) {
	if (purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED)
			&& purple_prefs_get_bool(OPT_WINTRANS_IM_ONFOCUS)) {
		GtkWidget *window = (GtkWidget *) d;
		if (e->in) { /* Focused */
			set_wintrans(window, 0, FALSE,
				purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
		} else {
			set_wintrans(window,
				purple_prefs_get_int(OPT_WINTRANS_IM_ALPHA),
				TRUE,
				purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
		}
	}
	return FALSE;
}
