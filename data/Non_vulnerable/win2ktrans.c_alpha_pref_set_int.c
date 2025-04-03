}
static void alpha_pref_set_int (GtkWidget *w, GdkEventFocus *e, const char *pref)
{
	int alpha = gtk_range_get_value(GTK_RANGE(w));
	purple_prefs_set_int(pref, alpha);
}
