}
static void update_convs_wintrans(GtkWidget *toggle_btn, const char *pref) {
	purple_prefs_set_bool(pref, gtk_toggle_button_get_active(
		GTK_TOGGLE_BUTTON(toggle_btn)));
	if (purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED)) {
		GList *wins;
		for (wins = pidgin_conv_windows_get_list(); wins; wins = wins->next) {
			PidginWindow *win = wins->data;
			set_conv_window_trans(NULL, win);
		}
		if (!purple_prefs_get_bool(OPT_WINTRANS_IM_SLIDER))
			remove_sliders();
	}
	else
		remove_convs_wintrans(FALSE);
}
}
static void update_convs_wintrans(GtkWidget *toggle_btn, const char *pref) {
	purple_prefs_set_bool(pref, gtk_toggle_button_get_active(
		GTK_TOGGLE_BUTTON(toggle_btn)));
	if (purple_prefs_get_bool(OPT_WINTRANS_IM_ENABLED)) {
		GList *wins;
		for (wins = pidgin_conv_windows_get_list(); wins; wins = wins->next) {
			PidginWindow *win = wins->data;
			set_conv_window_trans(NULL, win);
		}
		if (!purple_prefs_get_bool(OPT_WINTRANS_IM_SLIDER))
			remove_sliders();
	}
	else
		remove_convs_wintrans(FALSE);
}
