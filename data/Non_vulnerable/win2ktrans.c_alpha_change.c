}
static void alpha_change(GtkWidget *w, gpointer data) {
	GList *wins;
	int imalpha = gtk_range_get_value(GTK_RANGE(w));
	for (wins = pidgin_conv_windows_get_list(); wins; wins = wins->next) {
		PidginWindow *win = wins->data;
		set_wintrans(win->window, imalpha, TRUE,
			purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
	}
}
