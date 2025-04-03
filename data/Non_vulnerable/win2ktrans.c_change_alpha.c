}
static void change_alpha(GtkWidget *w, gpointer data) {
	int alpha = gtk_range_get_value(GTK_RANGE(w));
	purple_prefs_set_int(OPT_WINTRANS_IM_ALPHA, alpha);
	/* If we're in no-transparency on focus mode,
	 * don't take effect immediately */
	if (!purple_prefs_get_bool(OPT_WINTRANS_IM_ONFOCUS))
		set_wintrans(GTK_WIDGET(data), alpha, TRUE,
			purple_prefs_get_bool(OPT_WINTRANS_IM_ONTOP));
}
