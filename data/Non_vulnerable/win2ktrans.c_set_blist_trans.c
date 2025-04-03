}
static void set_blist_trans(GtkWidget *w, const char *pref) {
	gboolean enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));
	purple_prefs_set_bool(pref, enabled);
	if (blist) {
		set_wintrans(blist, purple_prefs_get_int(OPT_WINTRANS_BL_ALPHA),
			purple_prefs_get_bool(OPT_WINTRANS_BL_ENABLED),
			purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
	}
}
